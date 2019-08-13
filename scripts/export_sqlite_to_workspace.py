#!/usr/bin/env python

from __future__ import print_function

import os
import sys
import argparse
import sqlite3
import json
import uuid
import shutil

import bcrypt

"""
This script is not to be taken seriously and does not conform to PEP8

- Instead its a quick hack to export stuff, and indeed legacy
- Sometimes might be faster to create a new scenario, said the writer of this script ! 

"""

def export_db(database=None, workspace=None, openplc=None):

    ## Connect db
    if not os.path.exists(database):
        print("ERROR: No database at '%s'" % database)
        return
    try:
        conn = sqlite3.connect(database)
        cursor = conn.cursor()
        cursor.execute("select 1")
    except sqlite3.Error as e:
        print(e)
        return

    ## Validate workspace
    workspace = os.path.abspath(workspace)
    if not os.path.exists(workspace):
        print("ERROR: No workspace folder at `%s`" % workspace)
        return
    if not os.path.isdir(workspace):
        print("ERROR: File at `%s` instead of a folder" % workspace)
        return

    ## Validate openplc
    openplc = os.path.abspath(openplc)
    if not os.path.exists(openplc):
        print("ERROR: No openplc at  `%s`" % openplc)
        return



    def query_db(sql, args=()):

        try:
            cursor.execute(sql, args)
            rows = cursor.fetchall()

            ## This is a workaround until schema changes maybe, converts to dict
            col_names = [c[0].lower() for c in cursor.description]
            row_dict = []
            for ridx, row in enumerate(rows):
                d = {}
                for cidx, cn in enumerate(col_names):
                    d[cn.lower()] = row[cidx]
                row_dict.append(d)
            return row_dict

        except sqlite3.Error as e:
            print("ERROR: db error: %s" + str(e))

        return None

    def gen_uid():
        #return base64.b64encode(os.urandom(32))[:8]
        return str(uuid.uuid4().hex)[:8]

    def write_json(folder, filename, data):

        if folder:
            target_folder = os.path.join(workspace, folder)
            if not os.path.exists(target_folder):
                os.mkdir(target_folder)
        else:
            target_folder = workspace

        # need to "stick" to this format to makeit versionable, particular sort_keys
        js = json.dumps(data, sort_keys=True, indent=4)
        file_path = os.path.join(target_folder, filename)
        with open(file_path, "w") as file:
            file.write(js)
            file.close()
            print("Wrote: %s" % file_path)

    ## programs
    rows = query_db('select * from programs')
    for row in rows:
        uid = gen_uid()
        row["prog_id"] = uid
        write_json("programs", "%s.json" % uid, row )

    ## devices + slaves
    rows = query_db('select * from slave_dev')
    for row in rows:
        uid = gen_uid()
        row["dev_id"] = uid
        for k in ["ip_address", "com_port"]:
            if row[k] == "":
                row[k] = None
        for k in ["stop_bits", "slave_id", "ip_port", "data_bits"]:
            if row[k] == "":
                row[k] = None
            else:
                row[k] = int(row[k])
        if row['parity'] == "None":
            row['parity'] = None

        write_json("devices", "%s.json" % uid, row)

    ## users
    rows = query_db('select * from users')
    for row in rows:
        uid = gen_uid()
        row["user_id"] = uid
        row['user_active'] = True
        row['full_name'] = row['name']
        row['user'] = row['username']
        del row['name']
        del row['username']
        row['password'] = str(bcrypt.hashpw(row['password'].encode('utf-8'), bcrypt.gensalt()))
        write_json("users",  "%s.json" % uid, row)

    ### settings
    rows = query_db('select * from settings')
    # make current into a dic
    csett = {}
    for row in rows:
        csett[row['key'].lower()] = row['value']

    # defaults
    defSett = dict(
        modbus_enabled = False,
        modbus_port = 502,

        dnp3_enabled = False,
        dnp3_port = 20000,

        enip_enabled = False,
        enip_port = 5555,

        pstorage_enabled = False,
        pstorage_polling = 10,

        slave_polling = 100,
        slave_timeout = 1000,

        start_run_mode = False,
    )

    # loop defaults and check current in case some absent
    # note: introducting _active flags
    sett = {}
    for k, dv in defSett.items():

        parts = k.split("_")
        cv = csett[k] # current value
        #print(k, v, cv, parts)

        if parts[1] == "port":
            if csett.get(k) == "disabled":
                sett[parts[0] + "_enabled"] = False
                sett[k] = dv # copy default port no
            else:
                sett[parts[0] + "_enabled"] = True
                sett[k] = int(csett[k])
        elif k == "pstorage_polling":
            if csett.get(k) == "disabled":
                sett[parts[0] + "_enabled"] = False
                sett[k] = dv  # copy default port no
            else:
                sett["pstorage_enabled"] = True
                sett[k] = int(cv)
        elif k == "start_run_mode":
            sett["start_run_mode"] = bool(cv)
        elif parts[1] == "enabled":
            sett[k] = bool(cv)
        else:
            sett[k] = int(cv)

    write_json(None, "settings.json", sett)
    write_json(None, "settings.default.json", defSett)

    etc_dir = os.path.abspath(os.path.join(openplc, "etc"))
    for fn in "active_program", "openplc_driver","openplc_platform":
        fp = os.path.join(etc_dir, fn)
        contents = ""
        with open(fp, "r") as f:
            contents = f.read()
            f.close()
        sf = os.path.join(workspace, "%s.txt" % fn)
        with open(sf, "w") as f:
            f.write(contents.strip())
            f.close()
            print("Wrote: %s" % sf)

    ## uploads demo
    up = os.path.join(workspace, "uploads")
    if not os.path.exists(up):
        os.mkdir(up)

    st = os.path.join(openplc, "etc", "st_files", "blank_program.st")
    tar = os.path.join(up, "%s.blank_program.st" % gen_uid())
    shutil.copyfile(st, tar)
    print("Copied: %s" % tar)

    ## archive
    conn.close()

    ark = os.path.join(workspace, "archive")
    if not os.path.exists(ark):
        os.mkdir(ark)
    fn = "%s.%s" % (gen_uid(), os.path.basename(database))
    dbak = os.path.join(ark, fn)
    shutil.copyfile(database, dbak)
    print("Copied: %s" % dbak)



    print("DONE :-)")


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Export sqlite to workspace folder")

    parser.add_argument("-w", "--workspace",
                        help="Path to work directory",
                        action="store", type=str)

    parser.add_argument("-d", "--database",
                        help="Path to sqlite db",
                        action="store", type=str)

    parser.add_argument("-o", "--openplc",
                        help="Path to openplc checkout",
                        action="store", type=str)

    args = parser.parse_args()

    if args.database == None:
        print("ERROR: need a -d database")
        parser.print_help()
        sys.exit(0)

    if args.workspace == None:
        print("ERROR: need a -w work directory")
        parser.print_help()
        sys.exit(0)

    if args.openplc == None:
        print("ERROR: Need -o for openplc install")
        parser.print_help()
        sys.exit(0)

    export_db(database=args.database, workspace=args.workspace, openplc=args.openplc)




