#!/bin/bash
if [ $# -eq 0 ]; then
    echo "Error: You must provide a file to be compiled as argument"
    exit 1
fi

#move into the scripts folder if you're not there already
cd scripts &>/dev/null

OPENPLC_PLATFORM=$(cat ../etc/openplc_platform)
echo "compiling program for $OPENPLC_PLATFORM"
echo ""
    
if [ "$OPENPLC_PLATFORM" = "linux" ]; then
    cd ../build
    cmake .. -Dprogram_name=$1
    make
    if [ $? -ne 0 ]; then
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    exit 0
    
elif [ "$OPENPLC_PLATFORM" = "win" ]; then
    cd ../build
    cmake .. -Dprogram_name=$1
    make
    if [ $? -ne 0 ]; then
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    exit 0

elif [ "$OPENPLC_PLATFORM" = "rpi" ]; then
    
    cd ../build
    cmake .. -Dprogram_name=$1 -DOPLC_PLATFORM_RPI=ON
    make
    
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    exit 0


else
    echo "Error: Undefined platform! OpenPLC can only compile for Windows, Linux and Raspberry Pi environments"
    echo "Compilation finished with errors!"
    exit 1
fi

