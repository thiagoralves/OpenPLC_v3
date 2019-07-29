#!/bin/bash
./background_installer.sh $1 $2 2>&1 | tee install_log.txt ; test ${PIPESTATUS[0]} -eq 0
