from . import db
from sqlite3 import connect, Row
from mirror.settings import SettingsType, SettingsOptions
from sql.scripts import update, select
from sql.utils import convertData


def getSettings():
    database = connect(db)
    database.row_factory = Row

    script = select("Settings")

    try:
        settings = {}
        c = database.execute(script)
        s = c.fetchall()
        for setting in s:
            settings[setting[0]] = setting[1]
        database.commit()
        return settings
    except:
        raise Exception("Failed getting settings")


def saveSettings(settings):
    database = connect(db)
    database.row_factory = Row

    for key in SettingsOptions:
        if key not in settings.keys():
            settings[key] = "disabled"

    scripts = []

    for k, v in settings.items():
        data = {"key": k, "value": v}
        convertData(data, SettingsType)
        filter = {"key": data.pop("key")}
        scripts.append(update("Settings", data, filter))

    try:
        for script in scripts:
            database.execute(script)
        database.commit()
        return settings
    except:
        raise Exception("Failed updating settings")
