from . import db
from .user import getUserInfo
from sqlite3 import connect, Row
from sql.scripts import insert
from sql.utils import convertData
from bcrypt import hashpw
from base64 import b64decode as dec64


def validateLogin(username, password):

    try:
        userInfo = getUserInfo(username)
    except:
        return False

    salt = dec64(userInfo["salt"].encode("utf-8"))
    passHash = hashpw(password.encode("utf-8"), salt)

    h = dec64(userInfo["password"].encode("utf-8"))

    return passHash == h
