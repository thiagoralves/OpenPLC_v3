##############################################################################
#
# Checks and initializes default database if needed
#
##############################################################################


import sqlite3
from sqlite3 import Error
import os
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad
import key_create
import base64
import json

builddir = r"build/"
dbfile = r"build/openplc.db"

createTablePrograms = r"""CREATE TABLE `Programs` (
    `Prog_ID`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    `Name`	TEXT NOT NULL,
    `Description`	TEXT,
    `File`	TEXT NOT NULL,
    `Date_upload`	INTEGER NOT NULL
)"""

insertblankProgram = r"INSERT INTO Programs VALUES (1, 'Blank Program', 'Dummy empty program', 'blank_program.st', 1527184953)"

createTableSettings = r"""CREATE TABLE `Settings` (
    `Key`	TEXT NOT NULL UNIQUE,
    `Value`	TEXT NOT NULL,
    PRIMARY KEY(`Key`)
)"""


createTableSlave_dev = r"""CREATE TABLE "Slave_dev" (
    "dev_id"	INTEGER NOT NULL UNIQUE,
    "dev_name"	TEXT NOT NULL UNIQUE,
    "dev_type"	TEXT NOT NULL,
    "slave_id"	INTEGER NOT NULL,
    "com_port"	TEXT,
    "baud_rate"	INTEGER,
    "parity"	TEXT,
    "data_bits"	INTEGER,
    "stop_bits"	INTEGER,
    "ip_address"	TEXT,
    "ip_port"	INTEGER,
    "di_start"	INTEGER NOT NULL,
    "di_size"	INTEGER NOT NULL,
    "coil_start"	INTEGER NOT NULL,
    "coil_size"	INTEGER NOT NULL,
    "ir_start"	INTEGER NOT NULL,
    "ir_size"	INTEGER NOT NULL,
    "hr_read_start"	INTEGER NOT NULL,
    "hr_read_size"	INTEGER NOT NULL,
    "hr_write_start"	INTEGER NOT NULL,
    "hr_write_size"	INTEGER NOT NULL,
    "pause"	INTEGER,
    PRIMARY KEY("dev_id" AUTOINCREMENT)
)"""

createTableUsers = r"""CREATE TABLE "Users" (
    `user_id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
    `name`	TEXT NOT NULL,
    `username`	TEXT NOT NULL UNIQUE,
    `email`	TEXT,
    `password`	TEXT NOT NULL,
    `pict_file`	TEXT
)"""


gettablesQuery = r"""SELECT name FROM sqlite_master  
  WHERE type='table';"""
getsettingsQuery = r"""SELECT Key FROM Settings"""

# Security Measures
def getKey():
    with open('key.bin', 'rb') as keyfile:
        key = keyfile.read()
        keyfile.close()
        return key

def getIV():
    with open('iv.bin', 'rb') as ivfile:
        iv = ivfile.read()
        ivfile.close()
        return iv
    
def usr_encryption(input):
    key_create.main()
    key = getKey()
    iv = getIV()
    cipher1 = AES.new(iv, AES.MODE_CBC, key)
    enc_pwd = cipher1.encrypt(pad(input.encode(), 16))
    enc_encoded = base64.b64encode(enc_pwd).decode() 
    return enc_encoded

def pwd_encryption(input):
    key_create.main()
    key = getKey()
    iv = getIV()
    cipher1 = AES.new(key, AES.MODE_CBC, iv)
    enc_pwd = cipher1.encrypt(pad(input.encode(), 16))
    enc_encoded = base64.b64encode(enc_pwd).decode() 
    return enc_encoded

# return true if created fresh table
def checkTableExists(conn, tablename, tablecreatecommand):
    cur = conn.cursor()
    cur.execute(gettablesQuery)
    rows = cur.fetchall()
    for row in rows:
        if row[0] == tablename:
            cur.close()
            return False
    print(tablename, " didn't exist, creating")
    cur.execute(tablecreatecommand)
    cur.close()
    return True

def checkTablePrograms(conn):
    if checkTableExists(conn, "Programs", createTablePrograms):
        cur = conn.cursor()
        cur.execute(insertblankProgram)
        cur.close()
    return

def checkTableUsers(conn):
    if checkTableExists(conn, "Users", createTableUsers):
        username = usr_encryption('openplc')
        password = pwd_encryption('openplc')
        cur = conn.cursor()
        cur.execute("INSERT INTO Users (user_id, name, username, email, password, pict_file) VALUES (?, ?, ?, ?, ?, ?)", (10, 'OpenPLC User', username, 'openplc@openplc.com', password, 'NULL'))
        cur.close()
    return

# if code has new features, old database might not have required settings
def checkSettingExists(conn, setting, defaultvalue):
    cur = conn.cursor()
    cur.execute(getsettingsQuery)
    rows = cur.fetchall()
    for row in rows:
        if row[0] == setting:
            cur.close()
            return False
    print(setting, " didn't exist, creating")
    cur.execute("INSERT INTO Settings VALUES (?, ?)", (setting, defaultvalue))
    cur.close()
    return

def checkTableSettings(conn):
    checkTableExists(conn, "Settings", createTableSettings)
    checkSettingExists(conn, 'Modbus_port', '502')
    checkSettingExists(conn, 'Dnp3_port', '20000')
    checkSettingExists(conn, 'Start_run_mode', 'false')
    checkSettingExists(conn, 'snap7', 'false')
    checkSettingExists(conn, 'Slave_polling', '100')
    checkSettingExists(conn, 'Slave_timeout', '1000')
    checkSettingExists(conn, 'Enip_port', '44818')
    checkSettingExists(conn, 'Pstorage_polling', 'disabled')
    return

def checkTableSlave_dev(conn):
    checkTableExists(conn, "Slave_dev", createTableSlave_dev)
    return

def create_connection():
    """ create a database connection to a SQLite database """
    if not os.path.exists(builddir):
        os.mkdir(builddir)

    conn = None
    try:
        conn = sqlite3.connect(dbfile)
        checkTablePrograms(conn)
        checkTableUsers(conn)
        checkTableSettings(conn)
        checkTableSlave_dev(conn)
    except Error as e:
        print(sqlite3.version)
        print(e)
    finally:
        if conn:
            # NB, if you close without commiting, last actions might not be saved
            conn.commit()
            return conn


if __name__ == '__main__':
    conn = create_connection()
    conn.close()