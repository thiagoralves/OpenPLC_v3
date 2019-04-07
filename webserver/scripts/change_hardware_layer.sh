#!/bin/bash
if [ $# -eq 0 ]; then
    echo "Error: You must provide a hardware layer as argument"
    exit 1
fi

#move into the scripts folder if you're not there already
#cd scripts &>/dev/null

BUILD_DIR="../../build"

#= openplc_* persist target files in build/
PLATFORM_FILE="$BUILD_DIR/openplc_platform.txt"
DRIVER_FILE="$BUILD_DIR/openplc_driver.txt"

#move to the core folder
cd ../core

if [ "$1" == "blank" ]; then
    echo "Activating Blank driver"
    cp ./hardware_layers/blank.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo win > $PLATFORM_FILE
    echo blank > $DRIVER_FILE
    
elif [ "$1" == "blank_linux" ]; then
    echo "Activating Blank driver"
    cp ./hardware_layers/blank.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo linux > $PLATFORM_FILE
    echo blank_linux > $DRIVER_FILE
    
elif [ "$1" == "fischertechnik" ]; then
    echo "Activating Fischertechnik driver"
    cp ./hardware_layers/fischertechnik.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo fischertechnik > $DRIVER_FILE
    
elif [ "$1" == "neuron" ]; then
    echo "Activating Neuron driver"
    cp ./hardware_layers/neuron.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo linux > $PLATFORM_FILE
    echo neuron > $DRIVER_FILE

elif [ "$1" == "pixtend" ]; then
    echo "Activating PiXtend driver"
    cp ./hardware_layers/pixtend.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo pixtend > $DRIVER_FILE

elif [ "$1" == "pixtend_2s" ]; then
    echo "Activating PiXtend 2S driver"
    cp ./hardware_layers/pixtend2s.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo pixtend_2s > $DRIVER_FILE
    
elif [ "$1" == "pixtend_2l" ]; then
    echo "Activating PiXtend 2L driver"
    cp ./hardware_layers/pixtend2l.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo pixtend_2l > $DRIVER_FILE

elif [ "$1" == "rpi" ]; then
    echo "Activating Raspberry Pi driver"
    cp ./hardware_layers/raspberrypi.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo rpi > $DRIVER_FILE

elif [ "$1" == "rpi_old" ]; then
    echo "Activating Raspberry Pi 2011 driver"
    cp ./hardware_layers/raspberrypi_old.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo rpi_old > $DRIVER_FILE

elif [ "$1" == "simulink" ]; then
    echo "Activating Simulink driver"
    cp ./hardware_layers/simulink.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo win > $PLATFORM_FILE
    echo simulink > $DRIVER_FILE
    
elif [ "$1" == "simulink_linux" ]; then
    echo "Activating Simulink driver"
    cp ./hardware_layers/simulink.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo linux > $PLATFORM_FILE
    echo simulink_linux > $DRIVER_FILE

elif [ "$1" == "unipi" ]; then
    echo "Activating UniPi 1.1 driver"
    cp ./hardware_layers/unipi.cpp ./hardware_layer.cpp
    echo "Setting Platform"
    echo rpi > $PLATFORM_FILE
    echo unipi > $DRIVER_FILE

else
    echo "Error: Invalid hardware layer"
fi
