# restblueprint.py
from flask import Blueprint, jsonify, request
from typing import Callable, Optional

# Define the Blueprint
restapi_bp = Blueprint('restapi_blueprint', __name__)

# Global variable to store the single callback for this blueprint
_handler_callback: Optional[Callable[[str, dict], dict]] = None

def register_callback(callback: Callable[[str, dict], dict]):
    """Registers the business logic callback function."""
    global _handler_callback
    _handler_callback = callback
    print("Callback registered successfully for rest_blueprint!")

@restapi_bp.route("/<some_argument>", methods=["GET"])
def restapi_start_plc(some_argument):
    """
    Handles GET requests to /start_plc/<some_argument>.
    Data is expected via query parameters.
    """
    if _handler_callback is None:
        return jsonify({"error": "No handler registered"}), 500

    try:
        # For GET requests, get data from query parameters
        # Example: /start_plc/echo?key1=value1&key2=value2
        data = request.args.to_dict() # Converts query parameters to a dictionary

        result = _handler_callback(some_argument, data)
        return jsonify(result), 200
    except Exception as e:
        print(f"Error in restapi_start_plc: {e}") # Log the error
        return jsonify({"error": str(e)}), 500

@restapi_bp.route("/<some_argument>", methods=["POST"])
def restapi_start_plc_post(some_argument):
    """
    Handles POST requests to /start_plc_post/<some_argument>.
    Data is expected in the JSON body.
    """
    if _handler_callback is None:
        return jsonify({"error": "No handler registered"}), 500

    try:
        # For POST requests, get data from JSON body
        # request.get_json(silent=True) avoids raising an error if no JSON
        data = request.get_json(silent=True) or {} # Default to empty dict if no JSON

        result = _handler_callback(some_argument, data)
        return jsonify(result), 200
    except Exception as e:
        print(f"Error in restapi_start_plc_post: {e}") # Log the error
        return jsonify({"error": str(e)}), 500