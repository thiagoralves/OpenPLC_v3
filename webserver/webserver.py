# -*- coding: utf-8 -*-
from flask import Flask, jsonify
import json

app = Flask(__name__)
appPort = 2346
serverURL = f'http://localhost:{appPort}'

@app.route('/', methods=['GET', 'POST'])
def index():
    with open('static/html/login.html', 'r') as htmlfile:
        return htmlfile.read()

@app.route('/login', methods=['GET', 'POST'])
def login():
    return "You are logged in"

@app.route('/dashboard', methods=['GET', 'POST'])
def dashboard():
    with open('static/html/dashboard.html', 'r') as htmlfile:
        return htmlfile.read()

@app.route('/programs', methods=['GET', 'POST'])
def programs():
    with open('static/html/programs.html', 'r') as htmlfile:
        return htmlfile.read()

#This is just a test
@app.route('/uploadError', methods=['GET', 'POST'])
def uploadError():
    with open('static/html/programs/uploadProgram/errorPage/uploadError.html', 'r') as htmlfile:
        return htmlfile.read()

#This is just a test
@app.route('/uploadProgram', methods=['GET', 'POST'])
def uploadProgram():
    with open('static/html/programs/uploadProgram/uploadProgram.html', 'r') as htmlfile:
        return htmlfile.read()

#This is just a test
@app.route('/reloadProgram', methods=['GET', 'POST'])
def reloadProgram():
    # Leio o get
    # Pego as informações
    # Posto o conteúdo
    with open('static/html/programs/reloadProgram/reloadProgram.html', 'r') as htmlfile:
        return htmlfile.read()

#This is just a test
@app.route('/updateProgram', methods=['GET', 'POST'])
def updateProgram():
    with open('static/html/programs/updateProgram/updateProgram.html', 'r') as htmlfile:
        return htmlfile.read()

#This is just a test
@app.route('/compileProgram', methods=['GET', 'POST'])
def compileProgram():
    with open('static/html/programs/compileProgram/compileProgram.html', 'r') as htmlfile:
        return htmlfile.read()

@app.route('/hardware', methods=['GET', 'POST'])
def hardware():
    with open('static/html/hardware.html', 'r') as htmlfile:
        return htmlfile.read()

@app.route('/hardware/hardwareTypes', methods=['GET', 'POST'])
def hardwareTypes():
    with open('static/json/hardwareTypes.json', 'r') as hardwareTypes:
        content = json.loads(hardwareTypes.read())
        return jsonify(content)

@app.route('/hardware/python/exampleCode', methods=['GET', 'POST'])
def pythonCode():
    with open('static/python/psmDefaultCode.py', 'r') as defaultCode:
        #print(defaultCode)
        content = defaultCode.read()
        return content

@app.route('/modbus', methods=['GET', 'POST'])
def modbus():
    with open('static/html/modbus.html', 'r') as htmlfile:
        return htmlfile.read()

#This is just a test
@app.route('/addDevice', methods=['GET', 'POST'])
def addDevice():
    with open('static/html/modbus/addDevice/addModbusDevice.html', 'r') as htmlfile:
        return htmlfile.read()

#This is just a test
@app.route('/editDevice', methods=['GET', 'POST'])
def editDevice():
    with open('static/html/modbus/editDevice/editModbusDevice.html', 'r') as htmlfile:
        return htmlfile.read()

@app.route('/monitoring', methods=['GET', 'POST'])
def monitoring():
    with open('static/html/monitoring.html', 'r') as htmlfile:
        return htmlfile.read()

@app.route('/users', methods=['GET', 'POST'])
def users():
    with open('static/html/users.html', 'r') as htmlfile:
        return htmlfile.read()

#This is just a test
@app.route('/addUser', methods=['GET', 'POST'])
def addUser():
    with open('static/html/users/addUser/addUser.html', 'r') as htmlfile:
        return htmlfile.read()

@app.route('/settings', methods=['GET', 'POST'])
def settings():
    with open('static/html/settings.html', 'r') as htmlfile:
        return htmlfile.read()

def main():
    print('\033[93m' + 'Webserver is starting...' + '\033[0m')

if __name__ == '__main__':
    main()
    app.run(debug=False, host='0.0.0.0', threaded=True, port=appPort)
