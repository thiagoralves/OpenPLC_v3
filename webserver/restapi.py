# restblueprint.py
from flask import Blueprint, jsonify, request
from typing import Callable, Optional

# Define the Blueprint
restapi_bp = Blueprint('restapi_blueprint', __name__)

# Global variable to store the single callback for this blueprint
_handler_callback_get: Optional[Callable[[str, dict], dict]] = None
_handler_callback_get: Optional[Callable[[str, dict], dict]] = None

def register_callback_get(callback: Callable[[str, dict], dict]):
    """Registers the business logic callback function."""
    global _handler_callback_get
    _handler_callback_get = callback
    print("Callback registered successfully for rest_blueprint!")

def register_callback_post(callback: Callable[[str, dict], dict]):
    """Registers the business logic callback function."""
    global _handler_callback_post
    _handler_callback_post = callback
    print("Callback registered successfully for rest_blueprint!")

@restapi_bp.route("/<command>", methods=["GET"])
def restapi_start_plc_get(command):
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
def restapi_start_plc_post(command):
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
