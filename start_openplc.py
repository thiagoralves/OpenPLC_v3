#!/usr/bin/env python

import argparse
from webserver import webserver

parser = argparse.ArgumentParser(description="OpenPLC webserver")

parser.add_argument("-a", "--address",
                    help="ip address to listen [127.0.0.1]",
                    action="store", type=str,
                    default="127.0.0.1")

parser.add_argument("-p", "--port",
                    help="http port [8080]",
                    action="store", type=int,
                    default=8080)

parser.add_argument("-w", "--workspace",
                    help="Workspace directory for projects",
                    action="store", type=str)

parser.add_argument("-d", "--database",
                    help="database file",
                    action="store", type=str,
                    default="openplc.db")

args = parser.parse_args()


webserver.start_server(address=args.address,
                       port=args.port,
                       database=args.database,
                       workspace=args.workspace,
                       debug=True)
