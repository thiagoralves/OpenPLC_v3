from flask import Blueprint, send_file
from flask_login import login_required

blueprint = Blueprint("modbusApi", __name__)


@blueprint.route("/modbus", methods=["GET", "POST"])
@login_required
def modbus():
    return send_file("static/html/modbus.html")


@blueprint.route("/addDevice", methods=["GET", "POST"])
@login_required
def addDevice():
    return send_file("static/html/modbus/addDevice/addModbusDevice.html")


@blueprint.route("/editDevice", methods=["GET", "POST"])
@login_required
def editDevice():
    return send_file("static/html/modbus/editDevice/editModbusDevice.html")
