from flask import Blueprint, send_file

blueprint = Blueprint("hardwareApi", __name__)


@blueprint.route("/hardware", methods=["GET", "POST"])
def hardware():
    return send_file("static/html/hardware.html")


@blueprint.route("/hardware/hardwareTypes", methods=["GET", "POST"])
def hardwareTypes():
    return send_file("static/json/hardwareTypes.json")


@blueprint.route("/hardware/python/exampleCode", methods=["GET", "POST"])
def pythonCode():
    return send_file("static/python/psmDefaultCode.py")
