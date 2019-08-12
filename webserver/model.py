
import os
import bcrypt


from . import ut

WORK_DIR = None


settings = {}


def load_settings():
    """Returns dict of settings"""
    global settings
    settings, err = ut.read_json(os.path.join(WORK_DIR, "settings.json"))
    return err

def get_setting(key):
    """Return setting for key, or default"""
    return get_settings().get(key, defaultSettings.get(key))

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