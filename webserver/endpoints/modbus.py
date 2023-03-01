from flask import Blueprint, send_file
from flask_login import login_required
import serial.tools.list_ports
import platform
import json

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

@blueprint.route("/modbus/deviceTypes", methods=["GET", "POST"])
@login_required
def deviceTypes():
    return send_file("static/json/deviceTypes.json")

@blueprint.route("/modbus/comPorts", methods=["GET", "POST"])
@login_required
def comPorts():
    ports = [comport.device for comport in serial.tools.list_ports.comports()]
    portNames = []
    for port in ports:
        portNames.append(port)
    if (platform.system().startswith("CYGWIN")):
        portNames = list(map(lambda x: "COM" + str(int(x.split("/dev/ttyS")[1]) + 1), portNames))

    return json.dumps(portNames)


    