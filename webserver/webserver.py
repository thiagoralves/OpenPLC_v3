#!/usr/bin/env python

import argparse
import os
import sys
import subprocess
import platform
import serial.tools.list_ports
import random
import datetime
import time
import sqlite3
from sqlite3 import Error

import flask
from flask import redirect, request, render_template, url_for

import flask_login
from flask_login import current_user, login_required

from flask_socketio import SocketIO #, send, emit

#import pages
import openplc

HERE_DIR = os.path.abspath(os.path.dirname(__file__))
ROOT_DIR = os.path.abspath(os.path.join(HERE_DIR, ".."))
WORK_DIR = None
DB_FILE_NAME = "openplc.db"
DB_FILE_PATH = None
#------------------------------------------------
# Flask app
app = flask.Flask(__name__)

# str(os.urandom(16)) causes cookies to loose between restarts in dev
app.secret_key = "openplciscoolandneedshelp" #str(os.urandom(16))
app.config['SECRET_KEY'] = app.secret_key

app.config['TEMPLATES_AUTO_RELOAD'] = True

socketio = SocketIO(app)

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
    user.user_id = unicode(row["user_id"])
    user.username = row["username"]
    user.name = row["name"]
    user.pict_file = str(row["pict_file"])
    return user


@login_manager.unauthorized_handler
def unauthorized_handler():
    return 'Unauthorized'


"""
@login_manager.request_loader
def request_loader(request):
    username = request.form.get('username')

    database = "openplc.db"
    conn = create_connection(database)
    if (conn != None):
        try:
            cur = conn.cursor()
            cur.execute("SELECT username, password, name, pict_file FROM Users")
            rows = cur.fetchall()
            cur.close()
            conn.close()

            for row in rows:
                if (row[0] == username):
                    user = User()
                    user.id = row[0]
                    user.name = row[2]
                    user.pict_file = str(row[3])
                    user.is_authenticated = (request.form['password'] == row[1])
                    return user
            return

        except Error as e:
            print("error connecting to the database" + str(e))
            return
    else:
        return
"""
"""
@app.before_request
def before_request():
    flask.session.permanent = True
    app.permanent_session_lifetime = datetime.timedelta(minutes=59)
    flask.session.modified = True
"""


#------------------------------------------------
#  OpenPLC Runtime
openplc_runtime = openplc.Runtime(socketio)

def get_settings():
    rows, err = db_query("SELECT * FROM Settings")
    #print rows
    dic = {}
    for r in rows:
        dic[r['key'].lower()] = r['value']
    return dic

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


DEVICE_PROTOCOLS = [
    {'protocol': 'Uno', 'label': 'Arduino Uno'},
    {'protocol': 'Mega', 'label': 'Arduino Mega'},
    {'protocol': 'ESP32', 'label': 'ESP32'},
    {'protocol': 'ESP8266', 'label': 'ESP8266'},
    {'protocol': 'TCP', 'label': 'Generic Modbus TCP Device'},
    {'protocol': 'RTU', 'label': 'Generic Modbus RTU Device'}
]



def generate_mbconfig():

    rows, err = db_query("SELECT * FROM Slave_dev")
    if err:
        print "errr=", err
        #TODO
        return err


    mbconfig = 'Num_Devices = "%s"' % len(rows)
    print(rows)

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
            return 'device%s.%s = "%s"\n';


        #mbconfig += 'device' + device_idx + '.name = "' + str(row[1]) + '"\n'
        mbconfig += mb_line(device_idx,'name', row["dev_name"])
        #mbconfig += 'device' + device_idx + '.slave_id = "' + str(row[3]) + '"\n'
        mbconfig += mb_line(device_idx, 'slave_id', row["slave_id"])

        if row["dev_type"] in ['ESP32', 'ESP8266', 'TCP']:
            #mbconfig += 'device' + device_idx + '.protocol = "TCP"\n'
            mbconfig += mb_line(device_idx, 'protocol',"TCP")
            #mbconfig += 'device' + device_idx + '.address = "' + str(row[9]) + '"\n'
            mbconfig += mb_line(device_idx, 'address', row["ip_address"])
        else:
            #mbconfig += 'device' + device_idx + '.protocol = "RTU"\n'
            mbconfig += mb_line(device_idx, 'protocol', "RTU")
            if str(row["com_port"]).startswith("COM"):
                port_name = "/dev/ttyS" + str(int(str(row["com_port"]).split("COM")[1]) - 1)
            else:
                port_name = row["com_port"]
            #mbconfig += 'device' + device_idx + '.address = "' + port_name + '"\n'
            mbconfig += mb_line(device_idx, 'address', port_name)
        #mbconfig += 'device' + device_idx + '.IP_Port = "' + str(row[10]) + '"\n'
        mbconfig += mb_line(device_idx, 'IP_Port', row["ip_port"])
        #mbconfig += 'device' + device_idx + '.RTU_Baud_Rate = "' + str(row[5]) + '"\n'
        mbconfig += mb_line(device_idx, 'RTU_Baud_Rate', row["baud_reate"])
        # mbconfig += 'device' + device_idx + '.RTU_Parity = "' + str(row[6]) + '"\n'
        mbconfig += mb_line(device_idx, 'RTU_Parity', row["parity"])
        #mbconfig += 'device' + device_idx + '.RTU_Data_Bits = "' + str(row[7]) + '"\n'
        mbconfig += mb_line(device_idx, 'RTU_Data_Bits', row["data_bits"])
        #mbconfig += 'device' + device_idx + '.RTU_Stop_Bits = "' + str(row[8]) + '"\n\n'
        mbconfig += mb_line(device_idx, 'RTU_Stop_Bits', row["stop_bits"])

        #mbconfig += 'device' + device_idx + '.Discrete_Inputs_Start = "' + str(row[11]) + '"\n'
        mbconfig += mb_line(device_idx, 'Discrete_Inputs_Start', row["di_start"])
        #mbconfig += 'device' + device_idx + '.Discrete_Inputs_Size = "' + str(row[12]) + '"\n'
        mbconfig += mb_line(device_idx, 'Discrete_Inputs_Size', row["di_size"])
        #mbconfig += 'device' + device_idx + '.Coils_Start = "' + str(row[13]) + '"\n'
        mbconfig += mb_line(device_idx, 'Coils_Start', row["coil_start"])
        #mbconfig += 'device' + device_idx + '.Coils_Size = "' + str(row[14]) + '"\n'
        mbconfig += mb_line(device_idx, 'Coils_Size', row["coil_size"])
        #mbconfig += 'device' + device_idx + '.Input_Registers_Start = "' + str(row[15]) + '"\n'
        mbconfig += mb_line(device_idx, 'Input_Registers_Start', row["ir_start"])
        #mbconfig += 'device' + device_idx + '.Input_Registers_Size = "' + str(row[16]) + '"\n'
        mbconfig += mb_line(device_idx, 'Input_Registers_Size', row["ir_size"])
        #mbconfig += 'device' + device_idx + '.Holding_Registers_Read_Start = "' + str(row[17]) + '"\n'
        mbconfig += mb_line(device_idx, 'Holding_Registers_Read_Start', row["hr_read_start"])
        #mbconfig += 'device' + device_idx + '.Holding_Registers_Read_Size = "' + str(row[18]) + '"\n'
        mbconfig += mb_line(device_idx, 'Holding_Registers_Read_Size', row["hr_read_size"])
        #mbconfig += 'device' + device_idx + '.Holding_Registers_Start = "' + str(row[19]) + '"\n'
        mbconfig += mb_line(device_idx, 'Holding_Registers_Start', row["hr_write_start"])
        #mbconfig += 'device' + device_idx + '.Holding_Registers_Size = "' + str(row[20]) + '"\n'
        mbconfig += mb_line(device_idx, 'Holding_Registers_Size', row["hr_write_size"])
        #device_counter += 1
        print(mbconfig)
    with open('./mbconfig.cfg', 'w+') as f:
        f.write(mbconfig)
            



    
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

#-------------------------------------------
# Database stuff


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
        #print(err)

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
        print("error connecting to the database" + str(e))
        return None, str(e)
    
def db_insert(table, flds):
    sql = "insert into %s (" % table
    keys = sorted(flds.keys())
    sql += ",".join(keys)
    sql += ") values ("
    sql += ",".join(["?" for i in range(0, len(keys) )])
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
    sql += ",".join(["%s=?" %k for k in keys])
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





#= -------- Template Stuff ---------

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


#= -------- Handlers ---------
@app.route('/')
def p_index():
    if current_user.is_authenticated:
        return redirect(url_for('dashboard'))
    return redirect(url_for('login'))

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
                user.user_id = unicode(row['user_id'])
                user.username = row['username']
                user.name = row['name']
                user.pict_file = row['pict_file']
                print(user)
                flask_login.login_user(user, remember=True, fresh=True, )
                return flask.redirect(flask.url_for('dashboard'))

    return render_template("login.html", c=ctx)


@app.route('/logout')
def p_logout():
    flask_login.logout_user()
    return redirect(url_for('login'))


@app.route('/dashboard')
@login_required
def p_dashboard():
    #global openplc_runtime
    #if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()

    ctx = make_context(page="dashboard")

    return render_template("dashboard.html", c=ctx)


@app.route('/plc/start')
@login_required
def plc_start():
    global openplc_runtime
    openplc_runtime.start_runtime()
    time.sleep(1)
    configure_runtime()
    return redirect(url_for('dashboard'))


@app.route('/stop_plc')
@login_required
def stop_plc():
    global openplc_runtime
    openplc_runtime.stop_runtime()
    time.sleep(1)
    return redirect(url_for('dashboard'))


@app.route('/runtime_logs')
def runtime_logs():
    global openplc_runtime
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        return openplc_runtime.logs()




@app.route('/programs', methods=['GET', 'POST'])
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
            prog_id, err = db_insert("Programs", vars)

        else:
            db_insert("Programs", vars, "Prog_id", prog_id)

        st_dir = os.path.join(WORK_DIR, "st_files")
        if not os.path.exists(st_dir):
            os.makedirs(st_dir)
        save_path = os.path.join(st_dir, "%s" % prog_id)
        prog_file.save(save_path)



    ctx.program, ctx.error = db_query("SELECT * FROM Programs WHERE Prog_ID = ?", [str(prog_id)], single=True)




    return render_template("program_reload.html", c=ctx)

@socketio.on('compile')
def ws_xcommand(data):
    print data
    prog_id = data['prog_id']
    sql = "SELECT * FROM Programs WHERE Prog_id=?"
    row, error = db_query(sql, [str(prog_id)], single=True)
    openplc_runtime.project_name = row["name"]
    openplc_runtime.project_description = row["description"]
    openplc_runtime.project_file = row["file"]

    st_file = os.path.join(WORK_DIR, "st_files", str(row["prog_id"]) )
    openplc_runtime.compile_program(st_file)

    socketio.emit("xmessage", {"data": "foo"})
    #print("xcommand", data)
    #emit('xmessage', {'pong': data['ping']})

@app.route('/program/<int:prog_id>/compile', methods=['GET', 'POST'])
@login_required
def p_program_compile(prog_id):
    ## if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
    #st_file = flask.request.args.get('file')

    # load information about the program being compiled into the openplc_runtime object
    ctx = make_context("compiling")
    ctx.prog_id = prog_id

    sql = "SELECT * FROM Programs WHERE Prog_id=?"
    row, ctx.error = db_query(sql, [str(prog_id)], single=True)
    print ctx.error
    ctx.program = row

    openplc_runtime.project_name = row["name"]
    openplc_runtime.project_description = row["description"]
    openplc_runtime.project_file = row["file"]

    st_file = os.path.join(WORK_DIR, "st_files", str(row["prog_id"]) )
    #openplc_runtime.compile_program(st_file)

    return render_template("compiling.html", c=ctx)

@app.route('/remove-program', methods=['GET', 'POST'])
def remove_program():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
        prog_id = flask.request.args.get('id')
        database = "openplc.db"
        conn = create_connection(database)
        if (conn != None):
            try:
                cur = conn.cursor()
                cur.execute("DELETE FROM Programs WHERE Prog_ID = ?", (int(prog_id),))
                conn.commit()
                cur.close()
                conn.close()
                return flask.redirect(flask.url_for('programs'))
                
            except Error as e:
                print("error connecting to the database" + str(e))
                return 'Error connecting to the database. Make sure that your openplc.db file is not corrupt.<br><br>Error: ' + str(e)
        else:
            return 'Error connecting to the database. Make sure that your openplc.db file is not corrupt.'


@app.route('/deadprogram/0', methods=['GET', 'POST'])
def deadp_program_upload():

    ctx = make_context("programs")

    if request.method == "POST":
        if 'file' in request.files:
            prog_file = request.files['file']


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
        prog_file.save(os.path.join('st_files', filename))
        
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
        
        database = "openplc.db"
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
                return 'Error connecting to the database. Make sure that your openplc.db file is not corrupt.<br><br>Error: ' + str(e)
        else:
            return 'Error connecting to the database. Make sure that your openplc.db file is not corrupt.'
        




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

        #ctx.devices = []

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

            #tx.devices.append( [row["dev_id"], row["dev_name"], row["dev_type"], di, do, ai,ao ])
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
        dev_id, err = db_insert("Slave_dev", vals)

    if request.method == 'GET':

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


@app.route('/modbus/{dev_id}/delete', methods=['GET', 'POST'])
@login_required
def device_delete(dev_id):

    if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
    #devid_db = flask.request.args.get('dev_id')

    dev_id = int(dev_id)
    row, err = db_query("DELETE FROM Slave_dev WHERE dev_id = ?", (dev_id), single=True)
    if err:
        pass
        # todo
        return

    generate_mbconfig()
    return redirect(url_for('modbus'))

            
@app.route('/monitoring', methods=['GET', 'POST'])
@login_required
def p_monitoring():

    if openplc_runtime.is_compiling():
        return draw_compiling_page()

    ctx = make_context("monitoring")

    return render_template("monitoring.html", c=ctx)
            
            
@app.route('/hardware', methods=['GET', 'POST'])
@login_required
def p_hardware():

    ctx = make_context("hardware")

    if (flask.request.method == 'DEADGET'):
        with open('./scripts/openplc_driver') as f: current_driver = f.read().rstrip()
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
        if (current_driver == "pixtend"): return_str += "<option selected='selected' value='pixtend'>PiXtend</option>"
        else: return_str += "<option value='pixtend'>PiXtend</option>"
        if (current_driver == "pixtend_2s"): return_str += "<option selected='selected' value='pixtend_2s'>PiXtend 2s</option>"
        else: return_str += "<option value='pixtend_2s'>PiXtend 2s</option>"
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
        with open('./core/custom_layer.h') as f: return_str += f.read()
        return_str += pages.hardware_tail
            
    else:
        hardware_layer = flask.request.form['hardware_layer']
        custom_layer_code = flask.request.form['custom_layer_code']
        with open('./active_program') as f: current_program = f.read()
        with open('./core/custom_layer.h', 'w+') as f: f.write(custom_layer_code)

        subprocess.call(['./scripts/change_hardware_layer.sh', hardware_layer])
        return "<head><meta http-equiv=\"refresh\" content=\"0; URL='compile-program?file=" + current_program + "'\" /></head>"

    return render_template("hardware", c=ctx)


@app.route('/restore_custom_hardware')
def restore_custom_hardware():
    if (flask_login.current_user.is_authenticated == False):
        return flask.redirect(flask.url_for('login'))
    else:
        if (openplc_runtime.status() == "Compiling"): return draw_compiling_page()
        
        #Restore the original custom layer code
        with open('./core/custom_layer.original') as f: original_code = f.read()
        with open('./core/custom_layer.h', 'w+') as f: f.write(original_code)
        return flask.redirect(flask.url_for('hardware'))
        

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
        database = "openplc.db"
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
                return 'Error connecting to the database. Make sure that your openplc.db file is not corrupt.<br><br>Error: ' + str(e)
        else:
            return 'Error connecting to the database. Make sure that your openplc.db file is not corrupt.'


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

    
#----------------------------------------------------------------------------
#Creates a connection with the SQLite database.
#----------------------------------------------------------------------------
""" Create a connection to the database file """
def db_connection():
    try:
        conn = sqlite3.connect(DB_FILE_PATH)
        return conn
    except Error as e:
        print(e)
    return None


@socketio.on('connect')
def ws_connect():
    print("client connected")
    socketio.emit('xmessage', {'data': 'Connected'})

@socketio.on('disconnect')
def ws_disconnect():
    print('Client disconnected')


#----------------------------------------------------------------------------
#Main dummy function. Only displays a message and exits. The app keeps
#running on the background by Flask
#----------------------------------------------------------------------------
def deadmain():
   print("Starting the web interface...")




if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="OpenPLC webserver")
    parser.add_argument("-a", "--address", help="ip address to listen [127.0.0.1]", action="store", type=str, default="127.0.0.1")
    parser.add_argument("-p", "--port", help="http port [8080]", action="store", type=int, default=8080)
    parser.add_argument("workspace", help="Workspace directory", action="store", type=str)
    args = parser.parse_args()

    # TODO check workspace exists
    WORK_DIR = args.workspace
    print WORK_DIR
    if not os.path.exists(WORK_DIR):
        sys.exit("FATAL: workspace dir not exist %s" % WORK_DIR)

    DB_FILE_PATH = os.path.join(WORK_DIR, DB_FILE_NAME )
    if not os.path.exists(DB_FILE_PATH):
        s = "The database file `%s` does not exist\n" % DB_FILE_PATH
        s += "If you have an existing file, please copy to location above."
        s += "Other wise press Y to copy default."

        inp = raw_input("Copy default db Y/N ?")
        if inp.upper() == "Y":
            copy_default # TODO

    # Load information about current program on the openplc_runtime object
    st_file = None
    file_path = os.path.join(WORK_DIR, "active_program")
    if os.path.exists(file_path):
        with open(file_path, "r") as f:
            st_file = f.read().strip()

            if st_file:
                row, err = db_query("SELECT * FROM Programs WHERE File=?", [st_file], single=True)
                if row:
                    openplc_runtime.project_name = row["Name"]
                    openplc_runtime.project_description = row["Description"]
                    openplc_runtime.project_file = row["File"]

    socketio.run(app, host=args.address, port=args.port, debug=True)
    #app.run(debug=True, host=args.address, threaded=False, port=args.port)
