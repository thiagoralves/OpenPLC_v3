#!/usr/bin/python

from __future__ import print_function
import os
import sys
import argparse

import webserver.webserver


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="OpenPLC webserver")
    parser.add_argument("-a", "--address", help="ip address to listen [127.0.0.1]", action="store", type=str, default="127.0.0.1")
    parser.add_argument("-p", "--port", help="http port [8080]", action="store", type=int, default=8080)
    parser.add_argument("-d", "--debug", help="Print debug messages", action="store_true",  default=False)
    parser.add_argument("work_dir", help="Work storage directory", action="store", type=str)
    args = parser.parse_args()

    work_dir = args.work_dir.strip()
    if not os.path.exists(work_dir):
        sys.exit("Workspace dir `%s` does not exist")
    work_dir = os.path.abspath(work_dir)

    db_file = "%s/openplc.db" % work_dir
    if not os.path.exists(db_file):
        s = "\nERROR: The db file `%s` does not exist\n\nOptions are:\n" % db_file
        s += " q - if you have existing file then copy/move to `%s` \n" % db_file
        s += " c - create default db "
        print(s)
        ans = raw_input(": ")
        print(ans)
        if ans == "c":
            os.system("cp ./etc/openplc_default.db %s/openplc.db" % work_dir)
            os.system("mkdir %s/st_files" % work_dir)
            os.system("./etc/st_files/blank_program.st", "%s/st_files/blank_program.st" % work_dir)
        else:
            sys.exit("Quit")


    webserver.webserver.run_server(work_dir, address=args.address, port=args.port, debug_enabled=args.debug)