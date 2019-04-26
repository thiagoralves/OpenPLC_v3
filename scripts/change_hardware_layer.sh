#!/bin/bash


if [ $# -eq 0 ]; then
    echo "Error: You must provide a hardware layer as argument"
    exit 1
fi


HERE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
source $HERE_DIR/common.sh

mkdir -p $BIN_DIR
mkdir -p $SRC_GEN_DIR



if [ "$1" == "blank" ]; then
    echo "Activating Blank driver"
    cp $HARDWARE_DIR/blank.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo win > $PLATFORM_FILE
    echo blank > $DRIVER_FILE
    
elif [ "$1" == "blank_linux" ]; then
    echo "Activating Blank driver"
    cp $HARDWARE_DIR/blank.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo linux > $PLATFORM_FILE
    echo blank_linux > $DRIVER_FILE
    
elif [ "$1" == "fischertechnik" ]; then
    echo "Activating Fischertechnik driver"
    cp $HARDWARE_DIR/fischertechnik.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo fischertechnik > $DRIVER_FILE
    
elif [ "$1" == "neuron" ]; then
    echo "Activating Neuron driver"
    cp $HARDWARE_DIR/neuron.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo linux > $PLATFORM_FILE
    echo neuron > $DRIVER_FILE

elif [ "$1" == "pixtend" ]; then
    echo "Activating PiXtend driver"
    cp $HARDWARE_DIR/pixtend.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo pixtend > $DRIVER_FILE

elif [ "$1" == "pixtend_2s" ]; then
    echo "Activating PiXtend 2S driver"
    cp $HARDWARE_DIR/pixtend2s.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo pixtend_2s > $DRIVER_FILE
    
elif [ "$1" == "pixtend_2l" ]; then
    echo "Activating PiXtend 2L driver"
    cp $HARDWARE_DIR/pixtend2l.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo pixtend_2l > $DRIVER_FILE

elif [ "$1" == "rpi" ]; then
    echo "Activating Raspberry Pi driver"
    cp $HARDWARE_DIR/raspberrypi.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo rpi > $DRIVER_FILE

elif [ "$1" == "rpi_old" ]; then
    echo "Activating Raspberry Pi 2011 driver"
    cp $HARDWARE_DIR/raspberrypi_old.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo rpi_old > $DRIVER_FILE

elif [ "$1" == "simulink" ]; then
    echo "Activating Simulink driver"
    cp $HARDWARE_DIR/simulink.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo win > $PLATFORM_FILE
    echo simulink > $DRIVER_FILE
    
elif [ "$1" == "simulink_linux" ]; then
    echo "Activating Simulink driver"
    cp $HARDWARE_DIR/simulink.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo linux > $PLATFORM_FILE
    echo simulink_linux > $DRIVER_FILE

elif [ "$1" == "unipi" ]; then
    echo "Activating UniPi 1.1 driver"
    cp $HARDWARE_DIR/unipi.cpp $SRC_GEN_DIR/hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo unipi > $DRIVER_FILE

else
    echo "Error: Invalid hardware layer"

fi
