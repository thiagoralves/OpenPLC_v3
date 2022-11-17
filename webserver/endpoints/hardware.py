from flask import Blueprint, send_file
from flask_login import login_required

blueprint = Blueprint("hardwareApi", __name__)


@blueprint.route("/hardware", methods=["GET", "POST"])
@login_required
def hardware():
    return send_file("static/html/hardware.html")


@blueprint.route("/hardware/hardwareTypes", methods=["GET", "POST"])
@login_required
def hardwareTypes():
    return send_file("static/json/hardwareTypes.json")


@blueprint.route("/hardware/python/exampleCode", methods=["GET", "POST"])
@login_required
def pythonCode():
    return send_file("static/python/psmDefaultCode.py")
