from flask import Flask, Blueprint, jsonify, request
from flask_sqlalchemy import SQLAlchemy

from flask_jwt_extended import create_access_token, current_user, jwt_required, JWTManager, verify_jwt_in_request, get_jwt
from werkzeug.security import generate_password_hash, check_password_hash

import logging
from typing import Callable, Optional

import config
import os
env = os.getenv("FLASK_ENV", "development")

app_restapi = Flask(__name__)

if env == "production":
    app_restapi.config.from_object(config.ProdConfig)
else:
    app_restapi.config.from_object(config.DevConfig)

logger = logging.getLogger(__name__)
restapi_bp = Blueprint('restapi_blueprint', __name__)
_handler_callback_get: Optional[Callable[[str, dict], dict]] = None
_handler_callback_post: Optional[Callable[[str, dict], dict]] = None
jwt = JWTManager(app_restapi)
db = SQLAlchemy(app_restapi)

jwt_blacklist = set()

@jwt.token_in_blocklist_loader
def check_if_token_revoked(jwt_header, jwt_payload):
    jti = jwt_payload["jti"]
    return jti in jwt_blacklist


class User(db.Model):
    id: int = db.Column(db.Integer, primary_key=True)
    username: str = db.Column(db.Text, nullable=False, unique=True)
    password_hash: str = db.Column(db.Text, nullable=False)
    # TODO implement roles
    # For now, we will just use "user" and "admin"
    # In the future, we can implement more roles like "guest", "editor", etc
    # and use them to control access to different parts of the API
    role: str = db.Column(db.String(20), default="user")

    # Use PBKDF2 with SHA256 and 600,000 iterations for password hashing
    derivation_method: str = "pbkdf2:sha256:600000"

    def set_password(self, password: str) -> str:
        password = password + app_restapi.config["PEPPER"]
        self.password_hash = generate_password_hash(password,
                                            method=self.derivation_method)
        logger.debug(f"Password set for user {self.username} | {self.password_hash}")
        return self.password_hash

    def check_password(self, password: str) -> bool:
        password = password + app_restapi.config["PEPPER"]
        logger.debug(f"Checking password {self.password_hash} | {password}")
        return check_password_hash(self.password_hash, password)

    def to_dict(self):
        return {"id": self.id, "username": self.username, "role": self.role}


@jwt.user_identity_loader
def user_identity_lookup(user):
    return str(user.id)

@jwt.user_lookup_loader
def user_lookup_callback(_jwt_header, jwt_data):
    identity = jwt_data["sub"]
    return User.query.filter_by(id=identity).one_or_none()

def register_callback_get(callback: Callable[[str, dict], dict]):
    global _handler_callback_get
    _handler_callback_get = callback
    print("GET Callback registered successfully for rest_blueprint!")

def register_callback_post(callback: Callable[[str, dict], dict]):
    global _handler_callback_post
    _handler_callback_post = callback
    print("POST Callback registered successfully for rest_blueprint!")

@restapi_bp.route("/create-user", methods=["POST"])
def create_user():
    # check if there are any users in the database
    try:
        users_exist = User.query.first() is not None
    except Exception as e:
        print(f"Error checking for users: {e}")
        return jsonify({"msg": "User creation error"}), 401

    # if there are no users, we don't need to verify JWT
    if users_exist and verify_jwt_in_request(optional=True) is None:
        return jsonify({"msg": "User already created!"}), 401

    data = request.get_json()
    username = data.get("username")
    password = data.get("password")
    role = data.get("role", "user")
    
    if not username or not password:
        return jsonify({"msg": "Missing username or password"}), 400
    
    if User.query.filter_by(username=username).first():
        return jsonify({"msg": "Username already exists"}), 409

    # Create a new user
    user = User(username=username, role=role)
    user.set_password(password)
    db.session.add(user)
    db.session.commit()

    return jsonify({"msg": "User created", "id": user.id}), 201


# verify existing users individually
@restapi_bp.route("/get-user-info/<int:user_id>", methods=["GET"])
@jwt_required()
def get_user_info(user_id):
    try:
        user = User.query.get(user_id)
    except Exception as e:
        print(f"Error retrieving user: {e}")
        return jsonify({"msg": "User retrieval error"}), 500
    
    if not user:
        return jsonify({"msg": "User not found"}), 404

    return jsonify(user.to_dict())

@restapi_bp.route("/get-users-info", methods=["GET"])
def get_users_info():
    # If there are no users, we don't need to verify JWT
    try:
        verify_jwt_in_request()
    except Exception as e:
        print("No JWT token provided, checking for users without authentication")
        try:
            users_exist = User.query.first() is not None
        except Exception as e:
            print(f"Error checking for users: {e}")
            return jsonify({"msg": "User retrieval error"}), 500

        if not users_exist:
            return jsonify({"msg": "No users found"}), 404
        return jsonify({"msg": "Users found"}), 200

    try:
        users = User.query.all()
    except Exception as e:
        print(f"Error retrieving users: {e}")
        return jsonify({"msg": "User retrieval error"}), 500

    return jsonify([user.to_dict() for user in users]), 200


# password change for specific user by any authenticated user
@restapi_bp.route("/password-change/<int:user_id>", methods=["PUT"])
@jwt_required()
def change_password(user_id):
    data = request.get_json()
    old_password = data.get("old_password")
    new_password = data.get("new_password")

    if not old_password or not new_password:
        return jsonify({"msg": "Both old and new passwords are required"}), 400

    try:
        user = User.query.get(user_id)
    except Exception as e:
        print(f"Error retrieving user: {e}")
        return jsonify({"msg": "User retrieval error"}), 500
    
    if not user:
        return jsonify({"msg": "User not found"}), 404

    if not user.check_password(old_password):
        return jsonify({"msg": "Old password is incorrect"}), 403

    user.set_password(new_password)
    db.session.commit()

    return jsonify({"msg": f"Password for user {user.username} updated successfully"}), 200

# delete a user by ID
@restapi_bp.route("/delete-user/<int:user_id>", methods=["DELETE"])
@jwt_required()
def delete_user(user_id):
    try:
        user = User.query.get(user_id)
    except Exception as e:
        logger.error(f"Error retrieving user: {e}")
        return jsonify({"msg": "User retrieval error"}), 500

    if not user:
        return jsonify({"msg": "User not found"}), 404

    db.session.delete(user)
    db.session.commit()
    revoke_jwt()
    return jsonify({"msg": f"User {user.username} deleted successfully"}), 200


# create a new user and return JWT token
# if user already exists, return JWT token for that user
# if no user exists, create a new user with the provided username and password
# and return JWT token for that user
# if no username or password is provided, return 400 Bad Request
@restapi_bp.route("/login", methods=["POST"])
def login():
    username = request.json.get("username", None)
    password = request.json.get("password", None)

    try:
        user = User.query.filter_by(username=username).one_or_none()
        logger.debug(f"User found: {user}")
    except Exception as e:
        logger.error(f"Error retrieving user: {e}")
        return jsonify({"msg": "User retrieval error"}), 500

    if not user or not user.check_password(password):
        return jsonify("Wrong username or password"), 401

    access_token = create_access_token(identity=user)
    return jsonify(access_token=access_token)

# logout endpoint
@restapi_bp.route("/logout", methods=["POST"])
@jwt_required()
def logout():
    revoke_jwt()
    return jsonify({"msg": "User logged out successfully"}), 200

def revoke_jwt():
    jti = get_jwt()["jti"]
    try:
        # Add the JWT ID to the blacklist
        jwt_blacklist.add(jti)
    except Exception as e:
        logger.error(f"Error revoking JWT: {e}")


@restapi_bp.route("/<command>", methods=["GET"])
@jwt_required()
def restapi_plc_get(command):
    if _handler_callback_get is None:
        return jsonify({"error": "No handler registered"}), 500

    try:
        data = request.args.to_dict()
        result = _handler_callback_get(command, data)
        return jsonify(result), 200

    except Exception as e:
        logger.error(f"Error in restapi_plc_get: {e}")
        return jsonify({"error": str(e)}), 500


@restapi_bp.route("/<command>", methods=["POST"])
@jwt_required()
def restapi_plc_post(command):
    if _handler_callback_post is None:
        return jsonify({"error": "No handler registered"}), 500

    try:
        # TODO validate file and limit size
        data = request.get_json(silent=True) or {}

        result = _handler_callback_post(command, data)
        return jsonify(result), 200
    except Exception as e:
        logger.error(f"Error in restapi_plc_post: {e}")
        return jsonify({"error": str(e)}), 500
