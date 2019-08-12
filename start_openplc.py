#!/usr/bin/env python

from __future__ import print_function

import sys
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

parser.add_argument("-d", "--debug",
                    help="Switch on debugging",
                    action="store_true")

parser.add_argument("-r", "--request-log",
                    help="Switch on http request logging",
                    action="store_true")

args = parser.parse_args()

if args.workspace == None:
    print("ERROR: Need a workspace")
    parser.print_help()
    sys.exit()
print(args)
webserver.start_server(address=args.address,
                       port=args.port,
                       request_log=args.request_log,
                       workspace=args.workspace,
                       debug=args.debug)
