from flask import Blueprint, send_file
from flask_login import login_required

blueprint = Blueprint("settingsApi", __name__)


@blueprint.route("/settings", methods=["GET", "POST"])
@login_required
def settings():
    return send_file("static/html/settings.html")
