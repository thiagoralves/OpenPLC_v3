from flask import Blueprint, send_file, request
from flask_login import login_required
from database.settings import getSettings, saveSettings
import json

blueprint = Blueprint("settingsApi", __name__)


@blueprint.route("/settings", methods=["GET", "POST"])
@login_required
def settings():
    if request.method == "GET" and request.args.get("data"):
        return json.dumps(getSettings())

    elif request.method == "POST":
        data = dict(request.form)
        saveSettings(data)
    return send_file("static/html/settings.html")
