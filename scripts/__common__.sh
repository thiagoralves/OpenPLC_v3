#!/usr/bin/env bash

SCRIPTS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT_DIR=`realpath $SCRIPTS_DIR/..`

ETC_DIR="$ROOT_DIR/etc"
RUNTIME_DIR="$ROOT_DIR/runtime"
CORE_DIR="$RUNTIME_DIR/core"
C_LIBS_DIR="$RUNTIME_DIR/core/lib"
HARDWARE_DIR="$RUNTIME_DIR/hardware_layers"
VENDOR_DIR="$RUNTIME_DIR/vendor"

BUILD_DIR="$ROOT_DIR/build"
BIN_DIR="$ROOT_DIR/build/bin"
SRC_GEN_DIR="$BUILD_DIR/src_gen"

# openplc_* persist target files in build/
PLATFORM_FILE="$ETC_DIR/openplc_platform.txt"
DRIVER_FILE="$ETC_DIR/openplc_driver"
ACTIVE_PROGAM_FILE="$ETC_DIR/active_program.txt"

BLANK_ST_FILE="$ETC_DIR/st_files/blank_program.st"

