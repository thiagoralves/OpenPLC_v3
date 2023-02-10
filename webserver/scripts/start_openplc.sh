#!/bin/bash
if [[ ! -f "/persistent/dnp3.cfg" ]]
then
    cp /workdir/webserver/dnp3_default.cfg /persistent/dnp3.cfg 
fi
if [[ ! -f "/persistent/openplc.db" ]]
then
    cp /workdir/webserver/openplc_default.db /persistent/openplc.db
fi
if [[ ! -d "/persistent/st_files" ]]
then
    mkdir /persistent/st_files
    cp /workdir/webserver/st_files_default/* /persistent/st_files
fi
if [[ ! -f "/persistent/persistent.file" ]]
then
    touch /persistent/persistent.file
fi
if [[ ! -f "/persistent/mbconfig.cfg" ]]
then
    touch /persistent/mbconfig.cfg
fi
cd webserver
python2.7 webserver.py
