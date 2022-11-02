from flask import Blueprint, send_file

blueprint = Blueprint("settingsApi", __name__)


@blueprint.route("/settings", methods=["GET", "POST"])
def settings():
    return send_file("static/html/settings.html")
