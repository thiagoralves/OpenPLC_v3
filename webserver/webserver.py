# -----------------------------------------------------------------------------
# Copyright 2015 Thiago Alves
# This file is part of the OpenPLC Software Stack.

# OpenPLC is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# OpenPLC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with OpenPLC.  If not, see <http://www.gnu.org/licenses/>.
# ----------------------------------------------------------------------------

import sqlite3
from sqlite3 import Error
import os
import subprocess
import platform
import serial.tools.list_ports
import random
import datetime
import time
import sys

import flask
from flask import redirect, request, render_template, url_for

import flask_login
from flask_login import current_user, login_required

from flask_socketio import SocketIO

from . import HERE_DIR, ROOT_DIR, ETC_DIR, SCRIPTS_DIR, CURR_PROGRAM_FILE, CURR_DRIVER_FILE
from . import openplc
from . import pages
from . import monitoring
from . import ut

#WORK_DIR = None

# -----------------------------------------------------
# Database stuff
# -----------------------------------------------------
DB_FILE = "openplc.db"

def db_connection():
    global DB_FILE
    try:
        conn = sqlite3.connect(DB_FILE)
        return conn
    except Error as e:
        print("db_connection():", e)
    return None

def db_query(sql, args=(), single=False, as_list=False):
    """ Opens db, execute query, close then return results.
    Querying is one function atmo, as issues with multithread app.

    :param sql: str with sql and :params placeholders
    :param args: dict with arg values
    :param single: return only one row
    :param as_list: True returns a row in a list, otherwise a dict
    :return: rows/row, err
    """
    conn = db_connection()
    if not conn:
        return None, "Cannot connect db"
    try:
        cur = conn.cursor()
        cur.execute(sql, args)

        rows = cur.fetchall()
        cur.close()
        conn.close()

        ## This is a workaround until schema changes maybe, converts to dict
        col_names = [c[0].lower() for c in cur.description]
        row_dict = []
        for ridx, row in enumerate(rows):
            d = {}
            for cidx, cn in enumerate(col_names):
                d[cn] = row[cidx]
            row_dict.append(d)

        if not single:
            return rows if as_list else row_dict, None

        if len(rows) == 0:
            return None, "No Rows"

        if len(rows) > 1:
            return None, "More than one row"

        return rows[0] if as_list else row_dict[0], None

    except Error as e:
        print("error connecting to the database: " + str(e))
        return None, str(e)


def db_insert(table, flds):
    sql = "insert into %s (" % table
    keys = sorted(flds.keys())
    sql += ",".join(keys)
    sql += ") values ("
    sql += ",".join(["?" for i in range(0, len(keys))])
    sql += ");"

    conn = db_connection()
    if not conn:
        return None, "Cannot connect db"

    try:
        cur = conn.cursor()
        cur.execute(sql, [flds[k] for k in keys])
        conn.commit()

        ## get inserted id
        sql = "select last_insert_rowid()"
        cur.execute(sql)
        row = cur.fetchone()
        return row[0], None

    except Error as e:
        return None, str(e)


def db_update(table, flds, p_name, p_id):
    sql = "update %s set " % table
    keys = sorted(flds.keys())
    sql += ",".join(["%s=?" % k for k in keys])
    sql += " where %s = ?;" % p_name
    vals = [flds[k] for k in keys]
    vals.append(p_id)

    conn = db_connection()
    if not conn:
        return "Cannot connect db"
    try:
        cur = conn.cursor()
        cur.execute(sql, vals)
        conn.commit()
        return None

    except Error as e:
        print("error connecting to the database" + str(e))
        return str(e)

def db_execute(sql, vars):
    conn = db_connection()
    if not conn:
        return None, "Cannot connect db"
    try:
        print("db_execute", sql, vars)
        cur = conn.cursor()
        cur.execute(sql,vars)
        conn.commit()
        return None

    except Error as e:
        return str(e)

#------------------------------------------------
# Flask app and socketio
app = flask.Flask(__name__)
app.secret_key = "openplciscool1234564561" #str(os.urandom(16))

app.config['SECRET_KEY'] = app.secret_key
app.config['TEMPLATES_AUTO_RELOAD'] = True

socketio = SocketIO(app)

@socketio.on('connect')
def ws_connect():
    print("client connected")
    socketio.emit('xmessage', {'data': 'Connected'})

@socketio.on('disconnect')
def ws_disconnect():
    print('Client disconnected')



#------------------------------------------------
#  Login + Security
login_manager = flask_login.LoginManager()
login_manager.login_view = "/login" # FIXME
login_manager.init_app(app)

class User:
    def __init__(self):

        self.is_active = True
        self.is_authenticated = True

        self.user_id = None
        self.username = None
        self.name = None
        self.pict_file = None

    def get_id(self):
        return self.user_id

@login_manager.user_loader
def user_loader(user_id):
    """Get user details for flask admin"""
    sql = "SELECT user_id, username, name, pict_file FROM Users "
    sql += ' where user_id=? '
    row, err = db_query(sql, [user_id], single=True)
    if err:
        print(err)
        return None

    user = User()
    user.user_id = "%s" % row["user_id"]
    user.username = row["username"]
    user.name = row["name"]
    user.pict_file = str(row["pict_file"])
    return user


@login_manager.unauthorized_handler
def unauthorized_handler():
    return 'Unauthorized'


#------------------------------------------------------------------
#  OpenPLC Runtime
#------------------------------------------------------------------
openplc_runtime = openplc.Runtime(socketio)

monitor = monitoring.Monitor(socketio)

def get_settings():
    rows, err = db_query("SELECT * FROM Settings")
    dic = {}
    for r in rows:
        dic[r['key'].lower()] = r['value']
    return dic

DEVICE_PROTOCOLS = [
    {'protocol': 'Uno', 'label': 'Arduino Uno'},
    {'protocol': 'Mega', 'label': 'Arduino Mega'},
    {'protocol': 'ESP32', 'label': 'ESP32'},
    {'protocol': 'ESP8266', 'label': 'ESP8266'},
    {'protocol': 'TCP', 'label': 'Generic Modbus TCP Device'},
    {'protocol': 'RTU', 'label': 'Generic Modbus RTU Device'}
]

def configure_runtime():
    global openplc_runtime

    rows, err = db_query("SELECT * FROM Settings")
    if err:
        print(err)
        # TODO error handling
        return

    for row in rows:
        if row[0] == "Modbus_port":
            if row[1] != "disabled":
                print("Enabling Modbus on port " + str(int(row[1])))
                openplc_runtime.start_modbus(int(row[1]))
            else:
                print("Disabling Modbus")
                openplc_runtime.stop_modbus()

        elif row[0] == "Dnp3_port":
            if row[1] != "disabled":
                print("Enabling DNP3 on port " + str(int(row[1])))
                openplc_runtime.start_dnp3(int(row[1]))
            else:
                print("Disabling DNP3")
                openplc_runtime.stop_dnp3()

        elif row[0] == "Enip_port":
            if row[1] != "disabled":
                print("Enabling EtherNet/IP on port " + str(int(row[1])))
                openplc_runtime.start_enip(int(row[1]))
            else:
                print("Disabling EtherNet/IP")
                openplc_runtime.stop_enip()

        elif row[0] == "Pstorage_polling":
            if row[1] != "disabled":
                print("Enabling Persistent Storage with polling rate of " + str(int(row[1])) + " seconds")
                openplc_runtime.start_pstorage(int(row[1]))
            else:
                print("Disabling Persistent Storage")
                openplc_runtime.stop_pstorage()
                delete_persistent_file()


def delete_persistent_file():
    if os.path.isfile("persistent.file"):
        os.remove("persistent.file")
    print("persistent.file removed!")

def generate_mbconfig():

    rows, err = db_query("SELECT * FROM Slave_dev")
    if err:
        print "errr=", err
        #TODO
        return err


    mbconfig = 'Num_Devices = "%s"' % len(rows)

    for device_counter_int, row in enumerate(rows):

        device_idx = str(device_counter_int)

        mbconfig += """
# ------------
#   DEVICE """
        mbconfig += device_idx
        mbconfig += """
# ------------
"""
        def mb_line(idx, name, val):
            return 'device%s.%s = "%s"\n' % (idx, name, val);

        mbconfig += mb_line(device_idx,'name', row["dev_name"])
        mbconfig += mb_line(device_idx, 'slave_id', row["slave_id"])

        if row["dev_type"] in ['ESP32', 'ESP8266', 'TCP']:
            mbconfig += mb_line(device_idx, 'protocol',"TCP")
            mbconfig += mb_line(device_idx, 'address', row["ip_address"])
        else:
            mbconfig += mb_line(device_idx, 'protocol', "RTU")
            if str(row["com_port"]).startswith("COM"):
                port_name = "/dev/ttyS" + str(int(str(row["com_port"]).split("COM")[1]) - 1)
            else:
                port_name = row["com_port"]
            mbconfig += mb_line(device_idx, 'address', port_name)

        mbconfig += mb_line(device_idx, 'IP_Port', row["ip_port"])

        mbconfig += mb_line(device_idx, 'RTU_Baud_Rate', row["baud_rate"])
        mbconfig += mb_line(device_idx, 'RTU_Parity', row["parity"])
        mbconfig += mb_line(device_idx, 'RTU_Data_Bits', row["data_bits"])
        mbconfig += mb_line(device_idx, 'RTU_Stop_Bits', row["stop_bits"])

        mbconfig += mb_line(device_idx, 'Discrete_Inputs_Start', row["di_start"])
        mbconfig += mb_line(device_idx, 'Discrete_Inputs_Size', row["di_size"])

        mbconfig += mb_line(device_idx, 'Coils_Start', row["coil_start"])
        mbconfig += mb_line(device_idx, 'Coils_Size', row["coil_size"])

        mbconfig += mb_line(device_idx, 'Input_Registers_Start', row["ir_start"])
        mbconfig += mb_line(device_idx, 'Input_Registers_Size', row["ir_size"])

        mbconfig += mb_line(device_idx, 'Holding_Registers_Read_Start', row["hr_read_start"])
        mbconfig += mb_line(device_idx, 'Holding_Registers_Read_Size', row["hr_read_size"])

        mbconfig += mb_line(device_idx, 'Holding_Registers_Start', row["hr_write_start"])
        mbconfig += mb_line(device_idx, 'Holding_Registers_Size', row["hr_write_size"])

    print(mbconfig)
    cnf_file = os.path.join(ETC_DIR, "mbconfig.cfg")
    with open(cnf_file, 'w+') as f:
        f.write(mbconfig)
        f.close()
    return None

# ### OLDE
# def generate_mbconfig():
#     database = "../etc/openplc.db"
#     conn = create_connection(database)
#     if (conn != None):
#         try:
#             cur = conn.cursor()
#             cur.execute("SELECT COUNT(*) FROM Slave_dev")
#             row = cur.fetchone()
#             num_devices = int(row[0])
#             mbconfig = 'Num_Devices = "' + str(num_devices) + '"'
#             cur.close()
#
#             cur=conn.cursor()
#             cur.execute("SELECT * FROM Settings")
#             rows = cur.fetchall()
#             cur.close()
#
#             for row in rows:
#                 if (row[0] == "Slave_polling"):
#                     slave_polling = str(row[1])
#                 elif (row[0] == "Slave_timeout"):
#                     slave_timeout = str(row[1])
#
#             mbconfig += '\nPolling_Period = "' + slave_polling + '"'
#             mbconfig += '\nTimeout = "' + slave_timeout + '"'
#
#             cur = conn.cursor()
#             cur.execute("SELECT * FROM Slave_dev")
#             rows = cur.fetchall()
#             cur.close()
#             conn.close()
#
#             device_counter = 0
#             for row in rows:
#                 mbconfig += """
# # ------------
# #   DEVICE """
#                 mbconfig += str(device_counter)
#                 mbconfig += """
# # ------------
# """
#                 mbconfig += 'device' + str(device_counter) + '.name = "' + str(row[1]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.slave_id = "' + str(row[3]) + '"\n'
#                 if (str(row[2]) == 'ESP32' or str(row[2]) == 'ESP8266' or str(row[2]) == 'TCP'):
#                     mbconfig += 'device' + str(device_counter) + '.protocol = "TCP"\n'
#                     mbconfig += 'device' + str(device_counter) + '.address = "' + str(row[9]) + '"\n'
#                 else:
#                     mbconfig += 'device' + str(device_counter) + '.protocol = "RTU"\n'
#                     if (str(row[4]).startswith("COM")):
#                         port_name = "/dev/ttyS" + str(int(str(row[4]).split("COM")[1]) - 1)
#                     else:
#                         port_name = str(row[4])
#                     mbconfig += 'device' + str(device_counter) + '.address = "' + port_name + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.IP_Port = "' + str(row[10]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.RTU_Baud_Rate = "' + str(row[5]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.RTU_Parity = "' + str(row[6]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.RTU_Data_Bits = "' + str(row[7]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.RTU_Stop_Bits = "' + str(row[8]) + '"\n\n'
#
#                 mbconfig += 'device' + str(device_counter) + '.Discrete_Inputs_Start = "' + str(row[11]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.Discrete_Inputs_Size = "' + str(row[12]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.Coils_Start = "' + str(row[13]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.Coils_Size = "' + str(row[14]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.Input_Registers_Start = "' + str(row[15]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.Input_Registers_Size = "' + str(row[16]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.Holding_Registers_Read_Start = "' + str(row[17]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.Holding_Registers_Read_Size = "' + str(row[18]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.Holding_Registers_Start = "' + str(row[19]) + '"\n'
#                 mbconfig += 'device' + str(device_counter) + '.Holding_Registers_Size = "' + str(row[20]) + '"\n'
#                 device_counter += 1
#
#             with open('./mbconfig.cfg', 'w+') as f: f.write(mbconfig)
#
#         except Error as e:
#             print("error connecting to the database" + str(e))
#     else:
#         print("Error opening DB")
                

#-----------------------------------------------------------------
#=  Template Stuff
#-----------------------------------------------------------------

# site navigation; Note selected is passed a {{c.page}}
# see https://fontawesome.bootstrapcheatsheets.com/ for icons
Nav = [
    {"label": "Dashboard", "page": "dashboard", "icon": "fa-home"},
    {"label": "Programs", "page": "programs", "icon": "fa-braille"},
    {"label": "Slave Devices", "page": "slaves", "icon": "fa-sitemap"},
    {"label": "Monitoring", "page": "monitoring", "icon": "fa-signal"},
    {"label": "Hardware", "page": "hardware", "icon": "fa-microchip"},
    {"label": "Users", "page": "users", "icon": "fa-users"},
    {"label": "Settings", "page": "settings", "icon": "fa-cog"},
    {"label": "Logout", "page": "logout", "icon": "fa-sign-out"},
]
def nav_icon(page, default="fa-stop"):
    if page:
        for n in Nav:
            if n['page'] == page:
                return n['icon']
    return default

@app.context_processor
def inject_template_vars():
    # Put the runtime and navigation into template context
    return dict(nav=Nav, runtime=openplc_runtime)

class TemplateContext(object):
    pass

def make_context(page, **kwargs):
    c = TemplateContext()
    c.error = None
    c.page = page
    c.icon = nav_icon(page)
    return c





    
def draw_compiling_page():
    return_str = draw_blank_page()
    return_str += """
                    <h2>Compiling program</h2>
                    <textarea id='mytextarea' style='height:500px; resize:vertical'>
loading logs...
                    </textarea>
                    <br><br><center><a href='dashboard' id='dashboard_button' class='button' style='background-color: #EDEDED; pointer-events: none; width: 310px; height: 53px; margin: 0px 20px 0px 20px;'><b>Go to Dashboard</b></a></center>
                </div>
            </div>
        </div>
    </body>
    
    <script>
        (function (global)
        {
            if(typeof (global) === "undefined")
            {
                throw new Error("window is undefined");
            }
            
            var _hash = "!";
            var noBackPlease = function () 
            {
                global.location.href += "#";

                // making sure we have the fruit available for juice....
                // 50 milliseconds for just once do not cost much (^__^)
                global.setTimeout(function () 
                {
                    global.location.href += "!";
                }, 50);
            };
            
            // Earlier we had setInerval here....
            global.onhashchange = function () 
            {
                if (global.location.hash !== _hash) 
                {
                    global.location.hash = _hash;
                }
            };

            global.onload = function () 
            {
                loadData();
                noBackPlease();
                
                // disables backspace on page except on input fields and textarea..
                document.body.onkeydown = function (e) 
                {
                    var elm = e.target.nodeName.toLowerCase();
                    if (e.which === 8 && (elm !== 'input' && elm  !== 'textarea')) 
                    {
                        e.preventDefault();
                    }
                    // stopping event bubbling up the DOM tree..
                    e.stopPropagation();
                };
            };
        })(window);
        
        var req;
        
        function loadData()
        {
            url = 'compilation-logs'
            try
            {
                req = new XMLHttpRequest();
            } catch (e) 
            {
                try
                {
                    req = new ActiveXObject('Msxml2.XMLHTTP');
                } catch (e) 
                {
                    try 
                    {
                        req = new ActiveXObject('Microsoft.XMLHTTP');
                    } catch (oc) 
                    {
                        alert('No AJAX Support');
                        return;
                    }
                }
            }
            
            req.onreadystatechange = processReqChange;
            req.open('GET', url, true);
            req.send(null);
        }
        
        function processReqChange()
        {
            //If req shows 'complete'
            if (req.readyState == 4)
            {
                compilation_logs = document.getElementById('mytextarea');
                dashboard_button = document.getElementById('dashboard_button');
                
                //If 'OK'
                if (req.status == 200)
                {
                    //Update textarea text
                    compilation_logs.value = req.responseText;
                    if ((req.responseText.search("Compilation finished with errors!") != -1) || (req.responseText.search("Compilation finished successfully!") != -1))
                    {
                        dashboard_button.style.background='#E02222'
                        dashboard_button.style.pointerEvents='auto'
                    }
                    
                    //Start a new update timer
                    timeoutID = setTimeout('loadData()', 1000);
                }
                else
                {
                    compilation_logs.value = 'There was a problem retrieving the logs. Error: ' + req.statusText;
                }
            }
        }
    </script>
</html>"""
    return return_str

#
# @login_manager.user_loader
# def user_loader(username):
#     database = "../etc/openplc.db"
#     conn = create_connection(database)
#     if (conn != None):
#         try:
#             cur = conn.cursor()
#             cur.execute("SELECT username, password, name, pict_file FROM Users")
#             rows = cur.fetchall()
#             cur.close()
#             conn.close()
#
#             for row in rows:
#                 if (row[0] == username):
#                     user = User()
#                     user.id = row[0]
#                     user.name = row[2]
#                     user.pict_file = str(row[3])
#                     return user
#             return
#
#         except Error as e:
#             print("error connecting to the database" + str(e))
#             return
#     else:
#         return


# @login_manager.request_loader
# def request_loader(request):
#     username = request.form.get('username')
#
#     database = "../etc/openplc.db"
#     conn = create_connection(database)
#     if (conn != None):
#         try:
#             cur = conn.cursor()
#             cur.execute("SELECT username, password, name, pict_file FROM Users")
#             rows = cur.fetchall()
#             cur.close()
#             conn.close()
#
#             for row in rows:
#                 if (row[0] == username):
#                     user = User()
#                     user.id = row[0]
#                     user.name = row[2]
#                     user.pict_file = str(row[3])
#                     user.is_authenticated = (request.form['password'] == row[1])
#                     return user
#             return
#
#         except Error as e:
#             print("error connecting to the database" + str(e))
#             return
#     else:
#         return

#
# @app.before_request
# def before_request():
#     flask.session.permanent = True
#     app.permanent_session_lifetime = datetime.timedelta(minutes=5)
#     flask.session.modified = True
        
#=========================================================================
# http handlers
# - HTML page handlers with "p_" prefix
# - ajax request  with 2ax_" prefix)
#=========================================================================


#---------------- auth handlers --------------------------------

@app.route('/login', methods=['GET', 'POST'])
def p_login():
    # This is login page.. so if auth, goto dashboard
    if current_user.is_authenticated:
        return redirect(url_for('dashboard'))

    ctx = make_context("login")

    if request.method == "POST":
        username = request.form['oplc_username']
        password = request.form['oplc_secret']

        if username and password:

            sql = "SELECT user_id, username, password, name, pict_file FROM Users"
            sql += " where username=? and password=? "
            row, err = db_query(sql, (username, password), single=True)
            if err:
                ctx.error = err

            if not row:
                ctx.error = "Incorrect User or Password "

            else:
                # Login User
                user = User()
                user.user_id = str(row['user_id'])
                user.username = row['username']
                user.name = row['name']
                user.pict_file = row['pict_file']
                print(user)
                flask_login.login_user(user, remember=True, fresh=True, )
                return redirect(flask.url_for('p_dashboard'))

    return render_template("login.html", c=ctx)

@app.route('/logout')
def p_logout():
    monitor.stop_monitor()
    flask_login.logout_user()
    return redirect(url_for('p_login'))

#---------------- home pages --------------------------------
@app.route('/')
def index():
    if flask_login.current_user.is_authenticated:
        return flask.redirect(flask.url_for('p_dashboard'))
    else:
        return flask.redirect(flask.url_for('p_login'))

@app.route('/dashboard')
@login_required
def p_dashboard():

    ctx = make_context(page="dashboard")

    return render_template("dashboard.html", c=ctx)


#---------------- runtime --------------------------------

@app.route('/start_plc')
def start_plc():
    global openplc_runtime
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        monitor.stop_monitor()
        openplc_runtime.start_runtime()
        time.sleep(1)
        configure_runtime()
        monitor.cleanup()
        monitor.parse_st(openplc_runtime.project_file)
        return flask.redirect(flask.url_for('dashboard'))


@app.route('/stop_plc')
def stop_plc():
    global openplc_runtime
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        openplc_runtime.stop_runtime()
        time.sleep(1)
        monitor.stop_monitor()
        return flask.redirect(flask.url_for('dashboard'))


@app.route('/runtime_logs')
def runtime_logs():
    global openplc_runtime
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        return openplc_runtime.logs()





#-----------------
# Progam related
@app.route('/programs', methods=['GET'])
@login_required
def p_programs():

    if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()


    ctx = make_context("programs")

    list_all = request.args.get('list_all') == '1'
    sql = 'SELECT Prog_ID, Name, File, Date_upload FROM Programs ORDER BY Date_upload DESC '
    if not list_all:
        sql += " limit 20"


    ctx.programs, ctx.error = db_query(sql)

    return render_template("programs.html", c=ctx)

@app.route('/program/<int:prog_id>', methods=['GET', 'POST'])
@login_required
def p_program_edit(prog_id):

    if openplc_runtime.is_compiling():
        return draw_compiling_page()

    ctx = make_context("programs")
    ctx.prog_id = prog_id
    ctx.program = {}
    ctx.title = "New Program" if prog_id == 0 else "Progam Detail"

    if request.method == "POST":

        prog_file = request.files['file']

        vars = {}
        vars["Name"] = request.form["name"]
        vars["Description"] = request.form["description"]
        vars["Date_upload"] = str(int(time.time()))
        vars['File'] = prog_file.filename

        if prog_id == 0:
            ctx.prog_id, err = db_insert("Programs", vars)

        else:
            db_update("Programs", vars, "Prog_id", prog_id)

        ## FIXME this needs to be somewhere else
        st_dir = os.path.join(ETC_DIR, "st_files")
        save_path = os.path.join(st_dir, "prog.%s.st" % prog_id)
        prog_file.save(save_path)



    ctx.program, ctx.error = db_query("SELECT * FROM Programs WHERE Prog_ID = ?", [str(prog_id)], single=True)

    st_path = os.path.join(WORK_DIR, "st_files", str(prog_id))
    ctx.st_source, err = ut.read_file(st_path)

    return render_template("program_reload.html", c=ctx)


@app.route('/reload-program', methods=['GET', 'POST'])
def reload_program():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
        prog_id = flask.request.args.get('table_id')
        return_str = pages.w3_style + pages.style + draw_top_div()
        return_str += """
            <div class='main'>
                <div class='w3-sidebar w3-bar-block' style='width:250px; background-color:#3D3D3D'>
                    <br>
                    <br>
                    <a href="dashboard" class="w3-bar-item w3-button"><img src="/static/home-icon-64x64.png" alt="Dashboard" style="width:47px;height:32px;padding:0px 15px 0px 0px;float:left"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Dashboard</p></a>
                    <a href="programs" class="w3-bar-item w3-button" style="background-color:#E02222; padding-right:0px;padding-top:0px;padding-bottom:0px"><img src="/static/programs-icon-64x64.png" alt="Programs" style="width:47px;height:39px;padding:7px 15px 0px 0px;float:left"><img src="/static/arrow.png" style="width:17px;height:49px;padding:0px 0px 0px 0px;margin: 0px 0px 0px 0px; float:right"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 10px 0px 0px 0px'>Programs</p></a>
                    <a href='modbus' class='w3-bar-item w3-button'><img src='/static/modbus-icon-512x512.png' alt='Modbus' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Slave Devices</p></a>
                    <a href='monitoring' class='w3-bar-item w3-button'><img src='/static/monitoring-icon-64x64.png' alt='Monitoring' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Monitoring</p></a>
                    <a href='hardware' class='w3-bar-item w3-button'><img src='/static/hardware-icon-980x974.png' alt='Hardware' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Hardware</p></a>
                    <a href='users' class='w3-bar-item w3-button'><img src='/static/users-icon-64x64.png' alt='Users' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Users</p></a>
                    <a href='settings' class='w3-bar-item w3-button'><img src='/static/settings-icon-64x64.png' alt='Settings' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Settings</p></a>
                    <a href='logout' class='w3-bar-item w3-button'><img src='/static/logout-icon-64x64.png' alt='Logout' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Logout</p></a>
                    <br>
                    <br>"""
        return_str += draw_status()
        return_str += """
            </div>
            <div style="margin-left:320px; margin-right:70px">
                <div style="w3-container">
                    <br>
                    <h2>Program Info</h2>
                    <br>"""
        database = "../etc/openplc.db"
        conn = create_connection(database)
        if (conn != None):
            try:
                cur = conn.cursor()
                cur.execute("SELECT * FROM Programs WHERE Prog_ID = ?", (int(prog_id),))
                row = cur.fetchone()
                cur.close()
                conn.close()
                return_str += "<label for='prog_name'><b>Name</b></label><input type='text' id='prog_name' name='program_name' value='" + str(row[1]) + "' disabled>"
                return_str += "<label for='prog_descr'><b>Description</b></label><textarea type='text' rows='10' style='resize:vertical' id='prog_descr' name='program_descr' disabled>" + str(row[2]) + "</textarea>"
                return_str += "<label for='prog_file'><b>File</b></label><input type='text' id='prog_file' name='program_file' value='" + str(row[3]) + "' disabled>"
                return_str += "<label for='prog_date'><b>Date Uploaded</b></label><input type='text' id='prog_date' name='program_date' value='" + time.strftime('%b %d, %Y - %I:%M%p', time.localtime(row[4])) + "' disabled>"
                return_str += "<br><br><center><a href='compile-program?file=" + str(row[3]) + "' class='button' style='width: 310px; height: 53px; margin: 0px 20px 0px 20px;'><b>Reload program</b></a><a href='remove-program?id=" + str(prog_id) + "' class='button' style='width: 310px; height: 53px; margin: 0px 20px 0px 20px;'><b>Remove program</b></a></center>"
                return_str += """
                </div>
            </div>
        </div>
    </body>
</html>"""

            except Error as e:
                print("error connecting to the database" + str(e))
                return_str += 'Error connecting to the database. Make sure that your ../etc/openplc.db file is not corrupt.<br><br>Error: ' + str(e)
        else:
            return_str += 'Error connecting to the database. Make sure that your ../etc/openplc.db file is not corrupt.'
        
        return return_str


@app.route('/program/<int:prog_id>/remove', methods=['GET', 'POST'])
@login_required
def p_program_remove(prog_id):

    if request.method == "POST":
        xprog_id = int(request.form.get('prog_id'))

        if xprog_id == prog_id and request.form.get('action') == "do_delete":
            err = db_execute("DELETE FROM Programs WHERE Prog_ID = ?", [prog_id])
            print(err)

    return redirect(url_for('p_programs'))



@app.route('/upload-program', methods=['GET', 'POST'])
def upload_program():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
        if ('file' not in flask.request.files):
            return draw_blank_page() + "<h2>Error</h2><p>You need to select a file to be uploaded!<br><br>Use the back-arrow on your browser to return</p></div></div></div></body></html>"
        prog_file = flask.request.files['file']
        if (prog_file.filename == ''):
            return draw_blank_page() + "<h2>Error</h2><p>You need to select a file to be uploaded!<br><br>Use the back-arrow on your browser to return</p></div></div></div></body></html>"
        
        filename = str(random.randint(1,1000000)) + ".st"
        prog_file.save(os.path.join('../etc/st_files', filename))
        
        return_str = pages.w3_style + pages.style + draw_top_div()
        return_str += """
            <div class='main'>
                <div class='w3-sidebar w3-bar-block' style='width:250px; background-color:#3D3D3D'>
                    <br>
                    <br>
                    <a href="dashboard" class="w3-bar-item w3-button"><img src="/static/home-icon-64x64.png" alt="Dashboard" style="width:47px;height:32px;padding:0px 15px 0px 0px;float:left"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Dashboard</p></a>
                    <a href="programs" class="w3-bar-item w3-button" style="background-color:#E02222; padding-right:0px;padding-top:0px;padding-bottom:0px"><img src="/static/programs-icon-64x64.png" alt="Programs" style="width:47px;height:39px;padding:7px 15px 0px 0px;float:left"><img src="/static/arrow.png" style="width:17px;height:49px;padding:0px 0px 0px 0px;margin: 0px 0px 0px 0px; float:right"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 10px 0px 0px 0px'>Programs</p></a>
                    <a href='modbus' class='w3-bar-item w3-button'><img src='/static/modbus-icon-512x512.png' alt='Modbus' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Slave Devices</p></a>
                    <a href='monitoring' class='w3-bar-item w3-button'><img src='/static/monitoring-icon-64x64.png' alt='Monitoring' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Monitoring</p></a>
                    <a href='hardware' class='w3-bar-item w3-button'><img src='/static/hardware-icon-980x974.png' alt='Hardware' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Hardware</p></a>
                    <a href='users' class='w3-bar-item w3-button'><img src='/static/users-icon-64x64.png' alt='Users' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Users</p></a>
                    <a href='settings' class='w3-bar-item w3-button'><img src='/static/settings-icon-64x64.png' alt='Settings' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Settings</p></a>
                    <a href='logout' class='w3-bar-item w3-button'><img src='/static/logout-icon-64x64.png' alt='Logout' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Logout</p></a>
                    <br>
                    <br>"""
        return_str += draw_status()
        return_str += """
        </div>
            <div style="margin-left:320px; margin-right:70px">
                <div style="w3-container">
                    <br>
                    <h2>Program Info</h2>
                    <br>
                    <form id      = "uploadForm"
                        enctype   = "multipart/form-data"
                        action    = "upload-program-action"
                        method    = "post"
                        onsubmit  = "return validateForm()">
                        
                        <label for='prog_name'><b>Name</b></label>
                        <input type='text' id='prog_name' name='prog_name' placeholder='My Program v1.0'>
                        <label for='prog_descr'><b>Description</b></label>
                        <textarea type='text' rows='10' style='resize:vertical' id='prog_descr' name='prog_descr' placeholder='Insert the program description here'></textarea>"""
        return_str += "<label for='prog_file'><b>File</b></label><input type='text' id='program_file' name='program_file' value='" + filename + "' disabled>"
        return_str += "<label for='prog_date'><b>Date Uploaded</b></label><input type='text' id='prog_date' name='prog_date' value='" + time.strftime('%b %d, %Y - %I:%M%p', time.localtime(int(time.time()))) + "' disabled>"
        return_str += "<input type='hidden' value='" + filename + "' id='prog_file' name='prog_file'/>"
        return_str += "<input type='hidden' value='" + str(int(time.time())) + "' id='epoch_time' name='epoch_time'/>"
        return_str += """
                        <br>
                        <br>
                        <center><input type="submit" class="button" style="font-weight:bold; width: 310px; height: 53px; margin: 0px 20px 0px 20px;" value="Upload program"></center>
                    </form>
                </div>
            </div>
        </div>
    </body>
    
    <script type="text/javascript">
        function validateForm()
        {
            var progname = document.forms["uploadForm"]["prog_name"].value;
            if (progname == "")
            {
                alert("Program Name cannot be blank");
                return false;
            }
            return true;
        }
    </script>
</html>"""

        return return_str


@app.route('/upload-program-action', methods=['GET', 'POST'])
def upload_program_action():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
        prog_name = flask.request.form['prog_name']
        prog_descr = flask.request.form['prog_descr']
        prog_file = flask.request.form['prog_file']
        epoch_time = flask.request.form['epoch_time']
        
        database = "../etc/openplc.db"
        conn = create_connection(database)
        if (conn != None):
            try:
                cur = conn.cursor()
                cur.execute("INSERT INTO Programs (Name, Description, File, Date_upload) VALUES (?, ?, ?, ?)", (prog_name, prog_descr, prog_file, epoch_time))
                conn.commit()
                cur.close()
                conn.close()
                #Redirect back to the compiling page
                return '<!DOCTYPE html><html><head><meta http-equiv="refresh" content="0; url=/compile-program?file=' + prog_file + '"></head></html>'
            
            except Error as e:
                print("error connecting to the database" + str(e))
                return 'Error connecting to the database. Make sure that your ../etc/openplc.db file is not corrupt.<br><br>Error: ' + str(e)
        else:
            return 'Error connecting to the database. Make sure that your ../etc/openplc.db file is not corrupt.'
        

@app.route('/deadcompile-program', methods=['GET', 'POST'])
def compile_program():
    global openplc_runtime
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
        st_file = flask.request.args.get('file')
        
        #load information about the program being compiled into the openplc_runtime object
        database = "../etc/openplc.db"
        conn = create_connection(database)
        if (conn != None):
            try:
                cur = conn.cursor()
                cur.execute("SELECT * FROM Programs WHERE File=?", (st_file,))
                row = cur.fetchone()
                openplc_runtime.project_name = str(row[1])
                openplc_runtime.project_description = str(row[2])
                openplc_runtime.project_file = str(row[3])
                cur.close()
                conn.close()
            except Error as e:
                print("error connecting to the database" + str(e))
        else:
            print("error connecting to the database")
        
        delete_persistent_file()
        openplc_runtime.compile_program(st_file)
        
        return draw_compiling_page()

@app.route('/program/<int:prog_id>/compile', methods=['GET', 'POST'])
@login_required
def p_program_compile(prog_id):

    # The compile is triggered nbu the socketio below
    ctx = make_context("compiling")
    ctx.prog_id = prog_id


    sql = "SELECT * FROM Programs WHERE Prog_id=?"
    row, ctx.error = db_query(sql, [str(prog_id)], single=True)
    #print ctx.error
    ctx.program = row

    openplc_runtime.project_name = row["name"]
    openplc_runtime.project_description = row["description"]
    openplc_runtime.project_file = row["file"]


    return render_template("compiling.html", c=ctx)

@socketio.on('compile')
def ws_xcommand(data):

    prog_id = data['prog_id']
    sql = "SELECT * FROM Programs WHERE Prog_id=?"
    row, error = db_query(sql, [str(prog_id)], single=True)

    openplc_runtime.project_name = row["name"]
    openplc_runtime.project_description = row["description"]
    openplc_runtime.project_file = row["file"]

    socketio.emit("xmessage", {"data": "Starting Compile\n"})
    #st_file = os.path.join(ETC_DIR, "st_files", str(row["prog_id"]) )
    st_file_name = "prog.%s.st" % prog_id

    openplc_runtime.compile_program(st_file_name)


    #print("xcommand", data)
    #emit('xmessage', {'pong': data['ping']})

@app.route('/compilation-logs', methods=['GET', 'POST'])
def compilation_logs():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        return openplc_runtime.compilation_status()


@app.route('/slaves', methods=['GET', 'POST'])
@login_required
def p_slaves():

        if (openplc_runtime.status() == "Compiling"):
            return draw_compiling_page()

        ctx = make_context("slaves")

        sql = "SELECT dev_id, dev_name, dev_type, di_size, coil_size, ir_size, hr_read_size, hr_write_size FROM Slave_dev"
        rows, ctx.error = db_query(sql)

        counter_di = 0
        counter_do = 0
        counter_ai = 0
        counter_ao = 0

        for row in rows:

            #calculate di
            di = "-"
            if row["di_size"] != 0:
                di = "%IX" + str(100 + (counter_di/8)) + "." + str(counter_di%8) + " to "
                counter_di += row["di_size"]
                di += "%IX" + str(100 + ((counter_di-1)/8)) + "." + str((counter_di-1)%8)
            row["di"] = di

            #calculate do
            do = "-"
            if row["coil_size"] != 0:
                do = "%QX" + str(100 + (counter_do/8)) + "." + str(counter_do%8) + " to "
                counter_do += row["coil_size"]
                do += "%QX" + str(100 + ((counter_do-1)/8)) + "." + str((counter_do-1)%8)
            row["do"] = do

            #calculate ai
            ai = "-"
            if row["ir_size"] + row["hr_read_size"] != 0:
                ai = "%IW" + str(100 + counter_ai) + " to "
                counter_ai += row["ir_size"] + row["hr_read_size"]
                ai += "%IW" + str(100 + (counter_ai-1))
            row["ai"] = ai

            #calculate ao
            ao = "-"
            if row["hr_write_size"] != 0:
                ao = "%QW" + str(100 + counter_ao) + " to "
                counter_ao += row["hr_write_size"]
                ao += "%QW" + str(100 + (counter_ao-1))
            row["ao"] = ao

        ctx.devices = rows

        return render_template("slaves.html", c=ctx)

IO_PREFIXES = [
    {"prefix": "di", "label": "Discrete Inputs (%IX100.0)"},
    {"prefix": "coil", "label": "Coils (%QX100.0)"},
    {"prefix": "ir", "label": "Input Registers (%IW100)"},
    {"prefix": "hr_read", "label": "Holding Registers - Read (%IW100)"},
    {"prefix": "hr_write", "label": "Holding Registers - Write (%QW100)"},
]

@app.route('/slave/<int:dev_id>', methods=['GET', 'POST'])
@login_required
def p_slave_edit(dev_id):

    if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()

    ctx = make_context("slaves")
    ctx.dev_id = dev_id

    if request.method == 'POST':

        fnames = ["dev_name", "dev_type", "slave_id", "com_port", "baud_rate", "parity", "data_bits", "stop_bits","ip_address", "ip_port"]
        for ii in IO_PREFIXES:
            fnames.append("%s_start" % ii['prefix'])
            fnames.append("%s_size" % ii['prefix'])

        vals = {}
        for fld in fnames:
            vals[fld] = request.form.get(fld)
        print(vals)
        if dev_id == 0:
            dev_id, err = db_insert("Slave_dev", vals)
        else:
            err = db_update("Slave_dev", vals, "dev_id", dev_id)
        print(err)
        generate_mbconfig()
        return redirect(url_for("p_slaves"))


    ctx.device = {}
    if dev_id > 0:
        ctx.device, ctx.error = db_query("select * from slave_dev where dev_id=?", [dev_id], single=True)

    ctx.DEVICE_PROTOCOLS = DEVICE_PROTOCOLS
    ctx.IO_PREFIXES = IO_PREFIXES

    #= ports for  serial devices
    ctx.ports = []
    is_cygwin = platform.system().startswith("CYGWIN")
    ports = [comport.device for comport in serial.tools.list_ports.comports()]
    for port in ports:
        if is_cygwin:
            port_name = "COM" + str(int(port.split("/dev/ttyS")[1]) + 1)
        else:
            port_name = port

        ctx.ports.append(port_name)


    return render_template("slave_edit.html", c=ctx)


@app.route('/slave/<int:dev_id>/remove', methods=['GET', 'POST'])
@login_required
def p_slave_remove(dev_id):

    if request.method == "POST":
        xid = int(request.form.get('dev_id'))

        if xid == dev_id and request.form.get('action') == "do_delete":
            err = db_execute("DELETE FROM Slave_dev WHERE dev_id = ?", [dev_id])
            print(err)
            generate_mbconfig()

    return redirect(url_for('p_slaves'))

@app.route('/monitoring', methods=['GET', 'POST'])
@login_required
def p_monitoring():

    ctx = make_context("monitoring")
    ctx.debug_vars = []
    monitor.start_monitor()
    if openplc_runtime.is_running():
        monitor.start_monitor()
        ctx.debug_vars = monitor.debug_vars

    return render_template("monitoring.html", c=ctx)
            
@app.route('/monitoring2', methods=['GET', 'POST'])
def monitoring():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
        return_str = "" #pages.w3_style + pages.monitoring_head + draw_top_div()
        return_str += """
            <div class='main'>
                <div class='w3-sidebar w3-bar-block' style='width:250px; background-color:#3D3D3D'>
                    <br>
                    <br>
                    <a href="dashboard" class="w3-bar-item w3-button"><img src="/static/home-icon-64x64.png" alt="Dashboard" style="width:47px;height:32px;padding:0px 15px 0px 0px;float:left"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Dashboard</p></a>
                    <a href='programs' class='w3-bar-item w3-button'><img src='/static/programs-icon-64x64.png' alt='Programs' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Programs</p></a>
                    <a href='modbus' class='w3-bar-item w3-button'><img src='/static/modbus-icon-512x512.png' alt='Modbus' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Slave Devices</p></a>
                    <a href="monitoring" class="w3-bar-item w3-button" style="background-color:#E02222; padding-right:0px;padding-top:0px;padding-bottom:0px"><img src="/static/monitoring-icon-64x64.png" alt="Monitoring" style="width:47px;height:39px;padding:7px 15px 0px 0px;float:left"><img src="/static/arrow.png" style="width:17px;height:49px;padding:0px 0px 0px 0px;margin: 0px 0px 0px 0px; float:right"><p style='font-family:"Roboto", sans-serif; font-size:20px; color:white;margin: 10px 0px 0px 0px'>Monitoring</p></a>                    
                    <a href='hardware' class='w3-bar-item w3-button'><img src='/static/hardware-icon-980x974.png' alt='Hardware' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Hardware</p></a>
                    <a href='users' class='w3-bar-item w3-button'><img src='/static/users-icon-64x64.png' alt='Users' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Users</p></a>
                    <a href='settings' class='w3-bar-item w3-button'><img src='/static/settings-icon-64x64.png' alt='Settings' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Settings</p></a>
                    <a href='logout' class='w3-bar-item w3-button'><img src='/static/logout-icon-64x64.png' alt='Logout' style='width:47px;height:32px;padding:0px 15px 0px 0px;float:left'><p style='font-family:\"Roboto\", sans-serif; font-size:20px; color:white;margin: 2px 0px 0px 0px'>Logout</p></a>
                    <br>
                    <br>"""
        #return_str += draw_status()
        return_str += """
        </div>
            <div style="margin-left:320px; margin-right:70px">
                <div style="w3-container">
                    <br>
                    <h2>Monitoring</h2>
                    <p>The table below displays a list of the OpenPLC points used by the currently running program. By clicking in one of the listed points it is possible to see more information about it and also to force it to be a different value.</p>
                    <div id='monitor_table'>
                        <table>
                            <col width="50"><col width="10"><col width="10"><col width="10"><col width="100">
                            <tr style='background-color: white'>
                                <th>Point Name</th><th>Type</th><th>Location</th><th>Forced</th><th>Value</th>
                            </tr>"""

        if (openplc_runtime.status() == "Running"):
            monitor.start_monitor()
            data_index = 0
            for debug_data in monitor.debug_vars:
                return_str += '<tr style="height:60px" onclick="document.location=\'point-info?table_id=' + str(data_index) + '\'">'
                return_str += '<td>' + debug_data.name + '</td><td>' + debug_data.type + '</td><td>' + debug_data.location + '</td><td>' + debug_data.forced + '</td><td valign="middle">'
                if (debug_data.type == 'BOOL'):
                    if (debug_data.value == 0):
                        return_str += '<img src="/static/bool_false.png" alt="bool_false" style="width:40px;height:40px;vertical-align:middle; margin-right:10px">FALSE</td>'
                    else:
                        return_str += '<img src="/static/bool_true.png" alt="bool_true" style="width:40px;height:40px;vertical-align:middle; margin-right:10px">TRUE</td>'
                else:
                    percentage = (debug_data.value*100)/65535
                    return_str += '<div class="w3-grey w3-round" style="height:40px"><div class="w3-container w3-blue w3-round" style="height:40px;width:' + str(int(percentage)) + '%"><p style="margin-top:10px">' + str(debug_data.value) + '</p></div></div></td>'
                return_str += '</tr>'
                data_index += 1
            return_str += pages.monitoring_tail

        else:
            return_str += """
                        </table>
                    </div>
                </div>
            </div>
        </div>
    </body>
</html>"""

        return return_str

@app.route('/monitor-update', methods=['GET', 'POST'])
def monitor_update():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        #if (openplc_runtime.status() == "Compiling"): return 'OpenPLC is compiling new code. Please wait'
        return_str = """
                        <table>
                            <col width="50"><col width="10"><col width="10"><col width="10"><col width="100">
                            <tr style='background-color: white'>
                                <th>Point Name</th><th>Type</th><th>Location</th><th>Forced</th><th>Value</th>
                            </tr>"""
        
        #if (openplc_runtime.status() == "Running"):
        if (True):
            monitor.start_monitor()
            data_index = 0
            for debug_data in monitor.debug_vars:
                return_str += '<tr style="height:60px" onclick="document.location=\'point-info?table_id=' + str(data_index) + '\'">'
                return_str += '<td>' + debug_data.name + '</td><td>' + debug_data.type + '</td><td>' + debug_data.location + '</td><td>' + debug_data.forced + '</td><td valign="middle">'
                if (debug_data.type == 'BOOL'):
                    if (debug_data.value == 0):
                        return_str += '<img src="/static/bool_false.png" alt="bool_false" style="width:40px;height:40px;vertical-align:middle; margin-right:10px">FALSE</td>'
                    else:
                        return_str += '<img src="/static/bool_true.png" alt="bool_true" style="width:40px;height:40px;vertical-align:middle; margin-right:10px">TRUE</td>'
                else:
                    percentage = (debug_data.value*100)/65535
                    return_str += '<div class="w3-grey w3-round" style="height:40px"><div class="w3-container w3-blue w3-round" style="height:40px;width:' + str(int(percentage)) + '%"><p style="margin-top:10px">' + str(debug_data.value) + '</p></div></div></td>'
                return_str += '</tr>'
                data_index += 1
        
        return_str += """ 
                        </table>"""
        
        return return_str

HARDWARE_LAYERS = [
    {"value": "blank", "label": "Blank"},
    {"value": "blank_linux", "label": "Blank with DNP3 (Linux only)"},
    {"value": "fischertechnik", "label": "Fischertechnik"},
    {"value": "neuron", "label": "Neuron"},
    {"value": "pixtend", "label": "PiXtend"},
    {"value": "pixtend_2s", "label": "PiXtend 2s"},
    {"value": "pixtend_2l", "label": "PiXtend 2l"},
    {"value": "rpi", "label": "Raspberry Pi"},
    {"value": "rpi_old", "label": "Raspberry Pi - Old Model (2011 model B)"},
    {"value": "simulink", "label": "Simulink"},
    {"value": "simulink_linux", "label": "Simulink with DNP3 (Linux only)"},
    {"value": "unipi", "label": "UniPi v1.1"},
]
            
@app.route('/hardware', methods=['GET', 'POST'])
@login_required
def p_hardware():

    monitor.stop_monitor()

    ctx = make_context("hardware")
    custom_layer_file = os.path.join(ROOT_DIR, "runtime", "core", "custom_layer.h")

    if request.method == "POST":

        hardware_layer = request.form['hardware_layer']
        custom_layer_code = request.form['custom_layer_code']
        print("hardware_layer==", hardware_layer)

        with open(CURR_PROGRAM_FILE) as f:
            current_program = f.read()
            f.close()

        with open(custom_layer_file, 'w+') as f:
            f.write(custom_layer_code)
            f.close()

        #scripts_path = os.path.abspath(os.path.join(self_path, '..', 'scripts'))
        change_hardware_script = os.path.join(SCRIPTS_DIR, 'change_hardware_layer.sh')
        print(change_hardware_script)
        subprocess.call([change_hardware_script, hardware_layer], cwd=SCRIPTS_DIR)
        #return "<head><meta http-equiv=\"refresh\" content=\"0; URL='compile-program?file=" + current_program + "'\" /></head>"
        #redirect(??)
        #TODO CALL compile

    ctx.HARDWARE_LAYERS = HARDWARE_LAYERS

    ctx.current_driver = None
    with open(CURR_DRIVER_FILE) as f:
        ctx.current_driver = f.read().rstrip()

    ctx.custom_layer_code, ctx.error = ut.read_file(custom_layer_file)


    return render_template("hardware.html", c=ctx)


    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:

        if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
        if (flask.request.method == 'GET'):
            driver_path = os.path.abspath(os.path.join(self_path, '..', 'etc', 'openplc_driver'))
            with open(driver_path) as f: current_driver = f.read().rstrip()
            return_str = pages.w3_style + pages.hardware_style + draw_top_div() + pages.hardware_head
            return_str += draw_status()
            return_str += """
            </div>
            <div style="margin-left:320px; margin-right:70px">
                <div style="w3-container">
                    <br>
                    <h2>Hardware</h2>
                    <p>OpenPLC controls inputs and outputs through a piece of code called hardware layer (also known as driver). Therefore, to properly handle the inputs and outputs of your board, you must select the appropriate hardware layer for it. The Blank hardware layer is the default option on OpenPLC, which provides no support for native inputs and outputs.</p>
                    <!-- <p>This section allows you to change the hardware layer used by OpenPLC. It is also possible to augment the current hardware layer through the hardware layer code box. -->
                    <p><b>OpenPLC Hardware Layer</b><p>
                    <form   id    = "uploadForm"
                        enctype   =  "multipart/form-data"
                        action    =  "hardware"
                        method    =  "post">
                        <select id='hardware_layer' name='hardware_layer' style="width:400px;height:30px;font-size: 16px;font-family: 'Roboto', sans-serif;">"""
            if (current_driver == "blank"): return_str += "<option selected='selected' value='blank'>Blank</option>"
            else: return_str += "<option value='blank'>Blank</option>"
            if (current_driver == "blank_linux"): return_str += "<option selected='selected' value='blank_linux'>Blank Linux</option>"
            else: return_str += "<option value='blank_linux'>Blank with DNP3 (Linux only)</option>"
            if (current_driver == "fischertechnik"): return_str += "<option selected='selected' value='fischertechnik'>Fischertechnik</option>"
            else: return_str += "<option value='fischertechnik'>Fischertechnik</option>"
            if (current_driver == "neuron"): return_str += "<option selected='selected' value='neuron'>Neuron</option>"
            else: return_str += "<option value='neuron'>Neuron</option>"
            if (current_driver == "pixtend"): return_str += "<option selected='selected' value='pixtend'>PiXtend</option>"
            else: return_str += "<option value='pixtend'>PiXtend</option>"
            if (current_driver == "pixtend_2s"): return_str += "<option selected='selected' value='pixtend_2s'>PiXtend 2s</option>"
            else: return_str += "<option value='pixtend_2s'>PiXtend 2s</option>"
            if (current_driver == "pixtend_2l"): return_str += "<option selected='selected' value='pixtend_2l'>PiXtend 2l</option>"
            else: return_str += "<option value='pixtend_2l'>PiXtend 2l</option>"
            if (current_driver == "rpi"): return_str += "<option selected='selected' value='rpi'>Raspberry Pi</option>"
            else: return_str += "<option value='rpi'>Raspberry Pi</option>"
            if (current_driver == "rpi_old"): return_str += "<option selected='selected' value='rpi_old'>Raspberry Pi - Old Model (2011 model B)</option>"
            else: return_str += "<option value='rpi_old'>Raspberry Pi - Old Model (2011 model B)</option>"
            if (current_driver == "simulink"): return_str += "<option selected='selected' value='simulink'>Simulink</option>"
            else: return_str += "<option value='simulink'>Simulink</option>"
            if (current_driver == "simulink_linux"): return_str += "<option selected='selected' value='simulink_linux'>Simulink with DNP3 (Linux only)</option>"
            else: return_str += "<option value='simulink_linux'>Simulink with DNP3 (Linux only)</option>"
            if (current_driver == "unipi"): return_str += "<option selected='selected' value='unipi'>UniPi v1.1</option>"
            else: return_str += "<option value='unipi'>UniPi v1.1</option>"
            return_str += """
                        </select>
                        <br>
                        <br>
                        <p><b>Hardware Layer Code Box</b><p>
                        <p>The Hardware Layer Code Box allows you to extend the functionality of the current driver by adding custom code to it, such as reading I2C, SPI and 1-Wire sensors, or controling port expanders to add more outputs to your hardware</p>
                        <textarea wrap="off" spellcheck="false" name="custom_layer_code" id="custom_layer_code">"""
            with open('../runtime/core/custom_layer.h') as f: return_str += f.read()
            return_str += pages.hardware_tail
            
        else:
            hardware_layer = flask.request.form['hardware_layer']
            custom_layer_code = flask.request.form['custom_layer_code']
            program_dir = os.path.abspath(os.path.join(self_path, '..', 'etc', 'active_program'))
            with open(program_dir) as f: current_program = f.read()
            with open('../runtime/core/custom_layer.h', 'w+') as f: f.write(custom_layer_code)
            
            scripts_path = os.path.abspath(os.path.join(self_path, '..', 'scripts'))
            change_hardware_path = os.path.join(scripts_path, 'change_hardware_layer.sh')
            subprocess.call([change_hardware_path, hardware_layer], cwd=scripts_path)
            return "<head><meta http-equiv=\"refresh\" content=\"0; URL='compile-program?file=" + current_program + "'\" /></head>"
        
        return return_str


@app.route('/restore_custom_hardware')
def restore_custom_hardware():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
        
        #Restore the original custom layer code
        with open('../runtime/core/custom_layer.original') as f: original_code = f.read()
        with open('../runtime/core/custom_layer.h', 'w+') as f: f.write(original_code)
        return flask.redirect(flask.url_for('hardware'))
        

#----------------------- Users pages ------------
@app.route('/users')
@login_required
def p_users():

    if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()

    ctx = make_context("users")

    sql = "SELECT user_id, name, username, email FROM Users order by username asc"
    ctx.users, ctx.error = db_query(sql)

    return render_template("users.html", c=ctx)

@app.route('/user/<int:user_id>', methods=['GET', 'POST'])
@login_required
def p_user_edit(user_id):


    if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()

    ctx = make_context("users")

    ctx.user_id = user_id
    ctx.user = {}

    if request.method == 'POST':

        vars = {}
        vars['name'] = request.form['name']
        vars['username'] = request.form['username']
        vars['email'] = request.form['email']
        vars['password'] = request.form['xpasswd']
        if user_id == 0:
            db_insert("Users", vars)
        else:
            db_update("Users", vars, "user_id", user_id)

        return redirect(url_for('p_users'))


    if user_id > 0:
        sql = "SELECT * FROM Users WHERE user_id = ?"
        ctx.user, ctx.error = db_query(sql, [str(user_id)], single=True)

    return render_template("user_edit.html", c=ctx)


@app.route('/delete-user', methods=['GET', 'POST'])
def delete_user():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
        user_id = flask.request.args.get('user_id')
        database = "../etc/openplc.db"
        conn = create_connection(database)
        if (conn != None):
            try:
                cur = conn.cursor()
                cur.execute("SELECT username FROM Users WHERE user_id = ?", (int(user_id),))
                row = cur.fetchone()
                if (flask_login.current_user.id == row[0]):
                    cur.close()
                    conn.close()
                    return draw_blank_page() + "<h2>Error</h2><p>You cannot delete yourself!<br><br>Use the back-arrow on your browser to return</p></div></div></div></body></html>"
                else:
                    cur = conn.cursor()
                    cur.execute("DELETE FROM Users WHERE user_id = ?", (int(user_id),))
                    conn.commit()
                    cur.close()
                    conn.close()
                    return flask.redirect(flask.url_for('users'))
            except Error as e:
                print("error connecting to the database" + str(e))
                return 'Error connecting to the database. Make sure that your ../etc/openplc.db file is not corrupt.<br><br>Error: ' + str(e)
        else:
            return 'Error connecting to the database. Make sure that your ../etc/openplc.db file is not corrupt.'

@app.route('/settings', methods=['GET', 'POST'])
def p_settings():

    ctx = make_context("settings")


    if request.method == 'POST':

        conn = db_connection()
        cur = conn.cursor()

        modbus_port = request.form.get('modbus_port', "disabled")
        sql = "UPDATE Settings SET Value = ? WHERE Key = 'Modbus_port'"
        cur.execute(sql, (modbus_port,))
        conn.commit()

        dnp3_port = request.form.get('dnp3_port', "disabled")
        sql = "UPDATE Settings SET Value = ? WHERE Key = 'Dnp3_port'"
        cur.execute(sql, [dnp3_port])
        conn.commit()

    ctx.settings = get_settings()

    return render_template("settings.html", c=ctx)


@app.route('/settingx', methods=['GET', 'POST'])
def settingsxd():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        monitor.stop_monitor()
        if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
        if (flask.request.method == 'GET'):
            return_str = pages.w3_style + pages.settings_style + draw_top_div() + pages.settings_head
            return_str += draw_status()
            return_str += """
            </div>
            <div style="margin-left:320px; margin-right:70px">
                <div style="w3-container">
                    <br>
                    <h2>Settings</h2>
                    <form id        = "uploadForm"
                        enctype     = "multipart/form-data"
                        action      = "settings"
                        method      = "post"
                        onsubmit    = "return validateForm()">
                        
                        <label class="container">
                            <b>Enable Modbus Server</b>"""
            
            database = "../etc/openplc.db"
            conn = create_connection(database)
            if (conn != None):
                try:
                    cur = conn.cursor()
                    cur.execute("SELECT * FROM Settings")
                    rows = cur.fetchall()
                    cur.close()
                    conn.close()
                    
                    for row in rows:
                        if (row[0] == "Modbus_port"):
                            modbus_port = str(row[1])
                        elif (row[0] == "Dnp3_port"):
                            dnp3_port = str(row[1])
                        elif (row[0] == "Enip_port"):
                            enip_port = str(row[1])
                        elif (row[0] == "Pstorage_polling"):
                            pstorage_poll = str(row[1])
                        elif (row[0] == "Start_run_mode"):
                            start_run = str(row[1])
                        elif (row[0] == "Slave_polling"):
                            slave_polling = str(row[1])
                        elif (row[0] == "Slave_timeout"):
                            slave_timeout = str(row[1])
                    
                    if (modbus_port == 'disabled'):
                        return_str += """
                            <input id="modbus_server" type="checkbox">
                            <span class="checkmark"></span>
                        </label>
                        <label for='modbus_server_port'><b>Modbus Server Port</b></label>
                        <input type='text' id='modbus_server_port' name='modbus_server_port' value='502'>"""
                    else:
                        return_str += """
                            <input id="modbus_server" type="checkbox" checked>
                            <span class="checkmark"></span>
                        </label>
                        <label for='modbus_server_port'><b>Modbus Server Port</b></label>
                        <input type='text' id='modbus_server_port' name='modbus_server_port' value='""" + modbus_port + "'>"
                        
                    return_str += """
                        <br>
                        <br>
                        <br>
                        <label class="container">
                            <b>Enable DNP3 Server</b>"""
                    
                    if (dnp3_port == 'disabled'):
                        return_str += """
                            <input id="dnp3_server" type="checkbox">
                            <span class="checkmark"></span>
                        </label>
                        <label for='dnp3_server_port'><b>DNP3 Server Port</b></label>
                        <input type='text' id='dnp3_server_port' name='dnp3_server_port' value='20000'>"""
                    else:
                        return_str += """
                            <input id="dnp3_server" type="checkbox" checked>
                            <span class="checkmark"></span>
                        </label>
                        <label for='dnp3_server_port'><b>DNP3 Server Port</b></label>
                        <input type='text' id='dnp3_server_port' name='dnp3_server_port' value='""" + dnp3_port + "'>"
                    
                    return_str += """
                        <br>
                        <br>
                        <br>
                        <label class="container">
                            <b>Enable EtherNet/IP Server</b>"""
                            
                    if (enip_port == 'disabled'):
                        return_str += """
                            <input id="enip_server" type="checkbox">
                            <span class="checkmark"></span>
                        </label>
                        <label for='enip_server_port'><b>EtherNet/IP Server Port</b></label>
                        <input type='text' id='enip_server_port' name='enip_server_port' value='44818'>"""
                    else:
                        return_str += """
                            <input id="enip_server" type="checkbox" checked>
                            <span class="checkmark"></span>
                        </label>
                        <label for='enip_server_port'><b>EtherNet/IP Server Port</b></label>
                        <input type='text' id='enip_server_port' name='enip_server_port' value='""" + enip_port + "'>"
                    
                    return_str += """
                        <br>
                        <br>
                        <br>
                        <label class="container">
                            <b>Enable Persistent Storage Thread</b>"""
                            
                    if (pstorage_poll == 'disabled'):
                        return_str += """
                            <input id="pstorage_thread" type="checkbox">
                            <span class="checkmark"></span>
                        </label>
                        <label for='pstorage_thread_poll'><b>Persistent Storage polling rate</b></label>
                        <input type='text' id='pstorage_thread_poll' name='pstorage_thread_poll' value='10'>"""
                    else:
                        return_str += """
                            <input id="pstorage_thread" type="checkbox" checked>
                            <span class="checkmark"></span>
                        </label>
                        <label for='pstorage_thread_poll'><b>Persistent Storage polling rate</b></label>
                        <input type='text' id='pstorage_thread_poll' name='pstorage_thread_poll' value='""" + pstorage_poll + "'>"
                    
                    return_str += """
                        <br>
                        <br>
                        <br>
                        <label class="container">
                            <b>Start OpenPLC in RUN mode</b>"""
                            
                    if (start_run == 'false'):
                        return_str += """
                            <input id="auto_run" type="checkbox">
                            <span class="checkmark"></span>
                        </label>
                        <input type='hidden' value='false' id='auto_run_text' name='auto_run_text'/>"""
                    else:
                        return_str += """
                            <input id="auto_run" type="checkbox" checked>
                            <span class="checkmark"></span>
                        </label>
                        <input type='hidden' value='true' id='auto_run_text' name='auto_run_text'/>"""
                    
                    return_str += """
                        <br>
                        <h2>Slave Devices</h2>
                        <label for='slave_polling_period'><b>Polling Period (ms)</b></label>
                        <input type='text' id='slave_polling_period' name='slave_polling_period' value='""" + slave_polling + "'>"
                    
                    return_str += """
                        <br>
                        <br>
                        <br>
                        <label for='slave_timeout'><b>Timeout (ms)</b></label>
                        <input type='text' id='slave_timeout' name='slave_timeout' value='""" + slave_timeout + "'>"
                    
                    return_str += pages.settings_tail
                    
                except Error as e:
                    return_str += "error connecting to the database" + str(e)
            else:
                return_str += "Error opening DB"

            return return_str

        elif (flask.request.method == 'POST'):
            modbus_port = flask.request.form.get('modbus_server_port')
            dnp3_port = flask.request.form.get('dnp3_server_port')
            enip_port = flask.request.form.get('enip_server_port')
            pstorage_poll = flask.request.form.get('pstorage_thread_poll')
            start_run = flask.request.form.get('auto_run_text')
            slave_polling = flask.request.form.get('slave_polling_period')
            slave_timeout = flask.request.form.get('slave_timeout')
            
            database = "../etc/openplc.db"
            conn = create_connection(database)
            if (conn != None):
                try:
                    cur = conn.cursor()
                    if (modbus_port == None):
                        cur.execute("UPDATE Settings SET Value = 'disabled' WHERE Key = 'Modbus_port'")
                        conn.commit()
                    else:
                        cur.execute("UPDATE Settings SET Value = ? WHERE Key = 'Modbus_port'", (str(modbus_port),))
                        conn.commit()
                        
                    if (dnp3_port == None):
                        cur.execute("UPDATE Settings SET Value = 'disabled' WHERE Key = 'Dnp3_port'")
                        conn.commit()
                    else:
                        cur.execute("UPDATE Settings SET Value = ? WHERE Key = 'Dnp3_port'", (str(dnp3_port),))
                        conn.commit()
                        
                    if (enip_port == None):
                        cur.execute("UPDATE Settings SET Value = 'disabled' WHERE Key = 'Enip_port'")
                        conn.commit()
                    else:
                        cur.execute("UPDATE Settings SET Value = ? WHERE Key = 'Enip_port'", (str(enip_port),))
                        conn.commit()
                        
                    if (pstorage_poll == None):
                        cur.execute("UPDATE Settings SET Value = 'disabled' WHERE Key = 'Pstorage_polling'")
                        conn.commit()
                    else:
                        cur.execute("UPDATE Settings SET Value = ? WHERE Key = 'Pstorage_polling'", (str(pstorage_poll),))
                        conn.commit()
                        
                    if (start_run == 'true'):
                        cur.execute("UPDATE Settings SET Value = 'true' WHERE Key = 'Start_run_mode'")
                        conn.commit()
                    else:
                        cur.execute("UPDATE Settings SET Value = 'false' WHERE Key = 'Start_run_mode'")
                        conn.commit()
                        
                    cur.execute("UPDATE Settings SET Value = ? WHERE Key = 'Slave_polling'", (str(slave_polling),))
                    conn.commit()
                    
                    cur.execute("UPDATE Settings SET Value = ? WHERE Key = 'Slave_timeout'", (str(slave_timeout),))
                    conn.commit()
                    
                    cur.close()
                    conn.close()
                    configure_runtime()
                    generate_mbconfig()
                    return flask.redirect(flask.url_for('dashboard'))
                    
                except Error as e:
                    print("error connecting to the database" + str(e))
                    return 'Error connecting to the database. Make sure that your ../etc/openplc.db file is not corrupt.<br><br>Error: ' + str(e)
            else:
                return 'Error connecting to the database. Make sure that your ../etc/openplc.db file is not corrupt.'
        

@app.route('/logout')
def logout():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        monitor.stop_monitor()
        flask_login.logout_user()
        return flask.redirect(flask.url_for('login'))


@login_manager.unauthorized_handler
def unauthorized_handler():
    return 'Unauthorized'
    
#----------------------------------------------------------------------------
#Creates a connection with the SQLite database.
#----------------------------------------------------------------------------
""" Create a connection to the database file """
def create_connection(db_file):
   try:
      conn = sqlite3.connect(db_file)
      return conn
   except Error as e:
      print(e)

   return None

#----------------------------------------------------------------------------
#Main dummy function. Only displays a message and exits. The app keeps
#running on the background by Flask
#----------------------------------------------------------------------------
def start_server(address="127.0.0.1", port=8080,
                 database=DB_FILE,
                 workspace=None,
                 debug=False):

    global DB_FILE
    DB_FILE = database
    if not os.path.exists(DB_FILE):
        s = "The database file `%s` does not exist\n" % DB_FILE
        print(s)
        return

    global WORK_DIR
    WORK_DIR = workspace

    #Load information about current program on the openplc_runtime object
    program_dir = os.path.abspath(os.path.join(ETC_DIR, 'active_program'))
    file = open(program_dir, "r")
    st_file = file.read()
    st_file = st_file.replace('\r','').replace('\n','')
    
    print("db=", database)
    #sys.setdefaultencoding('UTF8')
    
    # database = "../etc/openplc.db"
    # conn = create_connection(database)
    # if (conn != None):
    #     try:
    #         cur = conn.cursor()
    #         cur.execute("SELECT * FROM Programs WHERE File=?", (st_file,))
    #         #cur.execute("SELECT * FROM Programs")
    #         row = cur.fetchone()
    #         openplc_runtime.project_name = str(row[1])
    #         openplc_runtime.project_description = str(row[2])
    #         openplc_runtime.project_file = str(row[3])
    #
    #         cur.execute("SELECT * FROM Settings")
    #         rows = cur.fetchall()
    #         cur.close()
    #         conn.close()
    #
    #         for row in rows:
    #             if (row[0] == "Start_run_mode"):
    #                 start_run = str(row[1])
    #
    #         if (start_run == 'true'):
    #             print("Initializing OpenPLC in RUN mode...")
    #             openplc_runtime.start_runtime()
    #             time.sleep(1)
    #             configure_runtime()
    #
    #         app.run(debug=False, host='0.0.0.0', threaded=True, port=8080)
    #
    #     except Error as e:
    #         print("error connecting to the database" + str(e))
    # else:
    #     print("error connecting to the database")

    import logging
    log = logging.getLogger('werkzeug')
    log.setLevel(logging.ERROR)

    socketio.run(app, host=address, port=port, debug=debug)

    #app.run(debug=debug, host=address, threaded=False, port=port)