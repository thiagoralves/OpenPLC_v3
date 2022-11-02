from flask import Blueprint, send_file

blueprint = Blueprint("modbusApi", __name__)


@blueprint.route("/modbus", methods=["GET", "POST"])
def modbus():
    return send_file("static/html/modbus.html")


@blueprint.route("/addDevice", methods=["GET", "POST"])
def addDevice():
    return send_file("static/html/modbus/addDevice/addModbusDevice.html")


@blueprint.route("/editDevice", methods=["GET", "POST"])
def editDevice():
    return send_file("static/html/modbus/editDevice/editModbusDevice.html")
