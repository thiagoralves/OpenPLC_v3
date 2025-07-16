from hmac import compare_digest

from flask import Flask, Blueprint, jsonify, request
from flask_sqlalchemy import SQLAlchemy

from flask_jwt_extended import create_access_token
from flask_jwt_extended import current_user
from flask_jwt_extended import jwt_required
from flask_jwt_extended import JWTManager

from typing import Callable, Optional

app_restapi = Flask(__name__)

app_restapi.config["JWT_SECRET_KEY"] = "openplc"  # TODO change password
app_restapi.config["SQLALCHEMY_DATABASE_URI"] = "sqlite://"
app_restapi.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False

jwt = JWTManager(app_restapi)
db = SQLAlchemy(app_restapi)

class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.Text, nullable=False, unique=True)
    full_name = db.Column(db.Text, nullable=False)

    # NOTE: In a real application make sure to properly hash and salt passwords
    def check_password(self, password):
        return compare_digest(password, "password")


# Register a callback function that takes whatever object is passed in as the
# identity when creating JWTs and converts it to a JSON serializable format.
@jwt.user_identity_loader
def user_identity_lookup(user):
    return str(user.id)

# Register a callback function that loads a user from your database whenever
# a protected route is accessed. This should return any python object on a
# successful lookup, or None if the lookup failed for any reason (for example
# if the user has been deleted from the database).
@jwt.user_lookup_loader
def user_lookup_callback(_jwt_header, jwt_data):
    identity = jwt_data["sub"]
    return User.query.filter_by(id=identity).one_or_none()

# Define the Blueprint
restapi_bp = Blueprint('restapi_blueprint', __name__)

# Global variable to store the single callback for this blueprint
_handler_callback_get: Optional[Callable[[str, dict], dict]] = None
_handler_callback_post: Optional[Callable[[str, dict], dict]] = None


def register_callback_get(callback: Callable[[str, dict], dict]):
    """Registers the business logic callback function."""
    global _handler_callback_get
    _handler_callback_get = callback
    print("GET Callback registered successfully for rest_blueprint!")

def register_callback_post(callback: Callable[[str, dict], dict]):
    """Registers the business logic callback function."""
    global _handler_callback_post
    _handler_callback_post = callback
    print("POST Callback registered successfully for rest_blueprint!")

@restapi_bp.route("/login", methods=["POST"])
def login():
    username = request.json.get("username", None)
    password = request.json.get("password", None)

    user = User.query.filter_by(username=username).one_or_none()
    if not user or not user.check_password(password):
        return jsonify("Wrong username or password"), 401

    # Notice that we are passing in the actual sqlalchemy user object here
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
        print(f"Error in restapi_start_plc: {e}")
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
        print(f"Error in restapi_start_plc_post: {e}")
        return jsonify({"error": str(e)}), 500
