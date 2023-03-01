from . import db
from sqlite3 import connect, Row
from mirror.user import UserType, UserNullable
from sql.scripts import insert, select
from sql.utils import convertData
from bcrypt import gensalt, hashpw
from base64 import b64encode as enc64


class User:
    __isAuth = True
    __isActive = True
    __isAnon = False

    def __init__(self, id):
        self.__id = id

    def is_authenticated(self):
        return self.__isAuth

    def is_active(self):
        return self.__isActive

    def is_anonymous(self):
        return self.__isAnon

    def get_id(self):
        return self.__id


def getUserInfo(username):
    database = connect(db)
    database.row_factory = Row

    username = UserType["username"](username, False).data

    script = select("User", {"username": username})

    try:
        c = database.execute(script)
        u = c.fetchone()
        k = u.keys()
        u = dict(zip(k, u))
        database.commit()
        return u
    except:
        raise Exception("Failed getting user")


def createUser(user):
    database = connect(db)
    database.row_factory = Row

    salt = gensalt()
    passb = user.get("password", "").encode("utf-8")
    h = hashpw(passb, salt)

    user["password"] = enc64(h).decode("utf-8")
    user["salt"] = enc64(salt).decode("utf-8")

    convertData(user, UserType, UserNullable)

    script = insert("User", user)

    try:
        c = database.execute(script)
        u = c.fetchone()
        k = u.keys()
        u = dict(zip(k, u))
        database.commit()
        return u
    except:
        raise Exception("Failed persisting user in database")
