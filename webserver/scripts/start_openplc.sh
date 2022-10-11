#!/bin/bash
D="`realpath "$0"`"
cd "${D%/*}/webserver"
python2.7 webserver.py
