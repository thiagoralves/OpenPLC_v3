from . import db
from .user import getUserInfo, User
from sqlite3 import connect, Row
from sql.scripts import insert
from sql.utils import convertData
from bcrypt import hashpw
from base64 import b64decode as dec64
from flask_login import LoginManager, login_user

loginManager = LoginManager()


def validateLogin(username, password):
    try:
        userInfo = getUserInfo(username)
    except:
        return False

    salt = dec64(userInfo["salt"].encode("utf-8"))
    passHash = hashpw(password.encode("utf-8"), salt)

    h = dec64(userInfo["password"].encode("utf-8"))

    success = passHash == h

    if success:
        login_user(User(str(userInfo["id"])))

    return success
