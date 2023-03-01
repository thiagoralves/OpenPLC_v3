from os.path import exists, dirname
from sqlite3 import connect
from sql.createTables import allTables
from sql.scripts import insert
from sql.utils import convertData
from mirror.user import UserType, UserNullable
from mirror.settings import SettingsType
from bcrypt import gensalt, hashpw
from base64 import b64encode as enc64
import json

db = dirname(__file__) + "/database.sqlite"


def getMainUserScript():
    user = {"name": "OpenPLC", "username": "openplc", "email": "openplc@openplc.com"}

    salt = gensalt()
    passb = "openplc".encode("utf-8")
    h = hashpw(passb, salt)

    user["password"] = enc64(h).decode("utf-8")
    user["salt"] = enc64(salt).decode("utf-8")

    convertData(user, UserType, UserNullable)

    return insert("User", user)


def getDefaultSettingsScript():
    staticFolder = dirname(dirname(__file__)) + "/static"
    settingsFile = staticFolder + "/json/defaultSettings.json"
    scripts = []
    with open(settingsFile, "r") as file:
        settings = json.loads(file.read())
        for k, v in settings.items():
            item = {"key": k, "value": v}
            convertData(item, SettingsType)
            scripts.append(insert("Settings", item))
    return scripts


if not exists(db):
    with open(db, "w"):
        pass
    with connect(db) as c:
        c.execute("PRAGMA foreign_keys = 1;")
        for t in allTables:
            c.execute(t)
        c.execute(getMainUserScript())
        for script in getDefaultSettingsScript():
            c.execute(script)
        c.commit()
