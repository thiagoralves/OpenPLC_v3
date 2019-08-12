
import os
import bcrypt


from . import ut

#-----------------------------------------------------------------------
# Workspace related
#-----------------------------------------------------------------------
WORK_DIR = None

def init_workspace():

    schema = os.path.join(WORK_DIR, "__schema__")
    if not os.path.exists(schema):
        os.mkdir(schema)




#-----------------------------------------------------------------------
# Settings related
#-----------------------------------------------------------------------
defaultSettings = dict(
    modbus_enabled="false",
    modbus_port=502,

    dnp3_enabled="false",
    dnp3_port=20000,

    enip_enabled="false",
    enip_port=20000,

    Pstorage_enabled="false",
    Pstorage_polling=10,

    Start_run_mode="false",
    Slave_polling=100,
    Slave_timeout=1000
)

settings = {}


def load_settings():
    """Returns dict of settings"""
    global settings
    settings, err = ut.read_json(os.path.join(WORK_DIR, "settings.json"))
    return err

def set_setting(key, value):
    """Sets the Key (currently maybe case sensitive)"""
    if get_setting(key) == None:
        sql = "INSERT into Settings(key, value)values(?,?)"
        return db_execute(sql, (key, value,))

    sql = "UPDATE Settings SET Value = ? WHERE Key = '%s' " % key
    return db_execute(sql, (value,))


#------------------------------------------------
# User releated
#------------------------------------------------

def list_users_dir():
    files, err = ut.list_dir(WORK_DIR, "users")
    if err:
        return None, err
    return files, err

def get_users():
    files, err = list_users_dir()
    if err:
        return None, err

    ret = []
    for fn in files:
        data, err = ut.read_json_file(os.path.join(WORK_DIR, "users", fn))
        if err:
            print(err)
        else:
            ret.append(data)
    return ret, None

def get_user(username=None, user_id=None):

    if user_id == None and user_id == None:
        return None, "Need a variable"

    if user_id:
        ## got user_id so read file
        user_file = os.path.join(WORK_DIR, "users", "%s.json" % user_id)
        return ut.read_json_file(user_file)

    ## else scan for username
    files, err = list_users_dir()
    if err:
        return None, err
    for fn in files:
        data, err = ut.read_json_file(os.path.join(WORK_DIR, "users", fn))
        if err:
            return None, err
        if data['username'] == username:
            return data, None
    return None, None

def auth_user(username=None, password=None):
    rec, err = get_user(username)
    if err:
        return None, err
    if rec == None:
        return None, None
    # TODO
    #if bcrypt.checkpw(password.encode('utf-8'), rec['password']):
    #    return rec, None
    return rec, None

def gen_password(password):
    return str(bcrypt.hashpw(password.encode('utf-8'), bcrypt.gensalt()))



#-----------------------------------------------------------------------
# Programs related
#-----------------------------------------------------------------------

def get_programs():
    return ut.read_json_dir(os.path.join(WORK_DIR, "programs"))

def get_program(prog_id):
    file_path = os.path.join(WORK_DIR, "programs", "%s.json" % prog_id)
    return ut.read_json_file(file_path)

def save_program(prog_id, rec):
    file_path = os.path.join(WORK_DIR, "programs", "%s.json" % prog_id)
    return ut.write_json_file(file_path, rec)


#-----------------------------------------------------------------------
# Devices & Slaves related
#-----------------------------------------------------------------------
def get_devices():
    return ut.read_json_dir(os.path.join(WORK_DIR, "devices"))


def get_device(dev_id):
    file_path = os.path.join(WORK_DIR, "devices", "%s.json" % dev_id)
    return ut.read_json_file(file_path)


#-----------------------------------------------------------------------
# Schema related
#-----------------------------------------------------------------------
SCHEMA_INDEX = "https://openplcproject.gitlab.io/openplc_schema/json/index.json"

def fetch_schema():
    schema_dir = os.path.join(WORK_DIR, "__schema__")

    ## get index
    url_list, err = ut.http_fetch(SCHEMA_INDEX)
    if err:
        return err
    fn = os.path.join(schema_dir, os.path.basename(SCHEMA_INDEX))
    ut.write_json_file(fn, url_list)
    print(SCHEMA_INDEX)

    ## get files listed in index
    for url in url_list:
        schema, err = ut.http_fetch(url)
        fn = os.path.join(schema_dir, os.path.basename(url))
        ut.write_json_file(fn, schema)
        print(fn)
