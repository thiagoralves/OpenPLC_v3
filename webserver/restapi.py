from hmac import compare_digest

from flask import Flask, Blueprint, jsonify, request
from flask_sqlalchemy import SQLAlchemy

from flask_jwt_extended import create_access_token, current_user, jwt_required, JWTManager, verify_jwt_in_request
from werkzeug.security import generate_password_hash, check_password_hash

from typing import Callable, Optional
import config

import os
env = os.getenv("FLASK_ENV", "development")

app_restapi = Flask(__name__)

if env == "production":
    app_restapi.config.from_object(config.ProdConfig)
else:
    app_restapi.config.from_object(config.DevConfig)

restapi_bp = Blueprint('restapi_blueprint', __name__)
_handler_callback_get: Optional[Callable[[str, dict], dict]] = None
_handler_callback_post: Optional[Callable[[str, dict], dict]] = None
jwt = JWTManager(app_restapi)
db = SQLAlchemy(app_restapi)

class User(db.Model):
    id: int = db.Column(db.Integer, primary_key=True)
    username: str = db.Column(db.Text, nullable=False, unique=True)
    password_hash: str = db.Column(db.Text, nullable=False, unique=True)
    # Use PBKDF2 with SHA256 and 600,000 iterations for password hashing
    derivation_method: str = "pbkdf2:sha256:600000"

    def set_password(self, password, pepper):
        password = password + pepper
        self.password_hash = generate_password_hash(password,
                                            method=self.derivation_method)
        print(f"Password set for user {self.username} | {self.password_hash}")
        return self.password_hash

    def check_password(self, password):
        password = password + app_restapi.config["PEPPER"]
        print(f"Checking password {self.password_hash} | {password}")
        return check_password_hash(self.password_hash, password)


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


@restapi_bp.route("/users", methods=["POST"])
def create_user():
    # check if there are any users in the database
    users_exist = User.query.first() is not None

    # if there are no users, we don't need to verify JWT
    if users_exist:
        if verify_jwt_in_request(optional=True) is None:
            return jsonify({"msg": "User already created!"}), 401

    data = request.get_json()
    username = data.get("username")
    password = data.get("password")

    if not username or not password:
        return jsonify({"msg": "Missing fields"}), 400

    if User.query.filter_by(username=username).first():
        return jsonify({"msg": "Username already exists"}), 409

    user = User(username=username)
    user.set_password(password, app_restapi.config["PEPPER"])

    db.session.add(user)
    db.session.commit()

    return jsonify({"msg": "User created", "id": user.id}), 201


# verify existing users
@restapi_bp.route("/check_user/<int:user_id>", methods=["GET"])
@jwt_required()
def update_user(user_id):
    user = User.query.get(user_id)
    if not user:
        return jsonify({"msg": "User not found"}), 404

    # data = request.get_json()
    # user.username = data.get("username", user.username)
    # db.session.commit()

    return jsonify({"msg": "User updated", "id": user.id})

# TODO test password change
@restapi_bp.route("/change_password", methods=["POST"])
@jwt_required()
def change_password():
    data = request.get_json()
    old_password = data.get("old_password")
    new_password = data.get("new_password")

    if not old_password or not new_password:
        return jsonify({"msg": "Both old and new passwords are required"}), 400

    if not current_user.check_password(old_password):
        return jsonify({"msg": "Old password is incorrect"}), 403

    current_user.password_hash = generate_password_hash(new_password)
    db.session.commit()

    return jsonify({"msg": "Password updated successfully"}), 200


@restapi_bp.route("/login", methods=["POST"])
def login():
    username = request.json.get("username", None)
    password = request.json.get("password", None)

    user = User.query.filter_by(username=username).one_or_none()
    print(f"User found: {user}")
    if not user or not user.check_password(password):
        return jsonify("Wrong username or password"), 401

    # TODO check to use as cookie
    access_token = create_access_token(identity=user)
    return jsonify(access_token=access_token)


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
        print(f"Error in restapi_plc_get: {e}")
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
        print(f"Error in restapi_plc_post: {e}")
        return jsonify({"error": str(e)}), 500
