from os.path import exists, dirname
from sqlite3 import connect
from sql.createTables import allTables
from sql.scripts import insert
from sql.utils import convertData
from mirror.user import User, UserNullable
from bcrypt import gensalt, hashpw
from base64 import b64encode as enc64

db = dirname(__file__) + "/database.sqlite"


def getMainUserScript():
    user = {"name": "OpenPLC", "username": "openplc", "email": "openplc@openplc.com"}

    salt = gensalt()
    passb = "openplc".encode("utf-8")
    h = hashpw(passb, salt)

    user["password"] = enc64(h).decode("utf-8")
    user["salt"] = enc64(salt).decode("utf-8")

    convertData(user, User, UserNullable)

    return insert("User", user)


if not exists(db):
    with open(db, "w"):
        pass
    with connect(db) as c:
        c.execute("PRAGMA foreign_keys = 1;")
        for t in allTables:
            c.execute(t)
        c.execute(getMainUserScript())
        c.commit()
