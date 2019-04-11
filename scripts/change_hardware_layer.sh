#!/bin/bash


if [ $# -eq 0 ]; then
    echo "Error: You must provide a hardware layer as argument"
    exit 1
fi

##move into the scripts folder if you're not there already
##cd scripts &>/dev/null
#SCRIPTS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
#ROOT_DIR=`realpath $SCRIPTS_DIR/..`
#
#BUILD_DIR="$ROOT_DIR/build"
#ETC_DIR="$ROOT_DIR/etc"
#CORE_DIR="$ETC_DIR/core"
#
##= openplc_* persist target files in build/
#PLATFORM_FILE="$BUILD_DIR/openplc_platform.txt"
#DRIVER_FILE="$BUILD_DIR/openplc_driver.txt"

HERE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
source $HERE_DIR/common.sh

mkdir $SRC_GEN_DIR
echo "BUILD_DIR=$BUILD_DIR"

#move to the core folder
#cd ../core

if [ "$1" == "blank" ]; then
    echo "Activating Blank driver"
    cp $CORE_DIR/hardware_layers/blank.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo win > $PLATFORM_FILE
    echo blank > $DRIVER_FILE
    
elif [ "$1" == "blank_linux" ]; then
    echo "Activating Blank driver"
    cp $CORE_DIR/hardware_layers/blank.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo linux > $PLATFORM_FILE
    echo blank_linux > $DRIVER_FILE
    
elif [ "$1" == "fischertechnik" ]; then
    echo "Activating Fischertechnik driver"
    cp $CORE_DIR/hardware_layers/fischertechnik.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo fischertechnik > $DRIVER_FILE
    
elif [ "$1" == "neuron" ]; then
    echo "Activating Neuron driver"
    cp $CORE_DIR/hardware_layers/neuron.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo linux > $PLATFORM_FILE
    echo neuron > $DRIVER_FILE

elif [ "$1" == "pixtend" ]; then
    echo "Activating PiXtend driver"
    cp $CORE_DIR/hardware_layers/pixtend.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo pixtend > $DRIVER_FILE

elif [ "$1" == "pixtend_2s" ]; then
    echo "Activating PiXtend 2S driver"
    cp $CORE_DIR/hardware_layers/pixtend2s.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo pixtend_2s > $DRIVER_FILE
    
elif [ "$1" == "pixtend_2l" ]; then
    echo "Activating PiXtend 2L driver"
    cp $CORE_DIR/hardware_layers/pixtend2l.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo pixtend_2l > $DRIVER_FILE

elif [ "$1" == "rpi" ]; then
    echo "Activating Raspberry Pi driver"
    cp $CORE_DIR/hardware_layers/raspberrypi.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo rpi > $DRIVER_FILE

elif [ "$1" == "rpi_old" ]; then
    echo "Activating Raspberry Pi 2011 driver"
    cp $CORE_DIR/hardware_layers/raspberrypi_old.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo rpi_old > $DRIVER_FILE

elif [ "$1" == "simulink" ]; then
    echo "Activating Simulink driver"
    cp $CORE_DIR/hardware_layers/simulink.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo win > $PLATFORM_FILE
    echo simulink > $DRIVER_FILE
    
elif [ "$1" == "simulink_linux" ]; then
    echo "Activating Simulink driver"
    cp $CORE_DIR/hardware_layers/simulink.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo linux > $PLATFORM_FILE
    echo simulink_linux > $DRIVER_FILE

elif [ "$1" == "unipi" ]; then
    echo "Activating UniPi 1.1 driver"
    cp $CORE_DIR/hardware_layers/unipi.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo unipi > $DRIVER_FILE

else
    echo "Error: Invalid hardware layer"
fi
