from .classes import String

SettingsType = {"key": String, "value": String}

SettingsOptions = [
    "modbus_port",
    "dnp3_port",
    "enip_port",
    "pstorage_polling",
    "start_run_mode",
]
