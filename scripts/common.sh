#!/usr/bin/env bash



SCRIPTS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT_DIR=`realpath $SCRIPTS_DIR/..`

ETC_DIR="$ROOT_DIR/etc"
CORE_DIR="$ETC_DIR/core"
C_LIBS_DIR="$CORE_DIR/lib"

BUILD_DIR="$ROOT_DIR/build"
SRC_GEN_DIR="$BUILD_DIR/src_gen"

# openplc_* persist target files in build/
PLATFORM_FILE="$BUILD_DIR/openplc_platform.txt"
DRIVER_FILE="$BUILD_DIR/openplc_driver.txt"
ACTIVE_PROGAM_FILE="$BUILD_DIR/active_program.txt"

BLANK_ST_FILE="$ETC_DIR/st_files/blank_program.st"

