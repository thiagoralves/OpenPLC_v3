#!/bin/bash

if [ $# -eq 0 ]; then
    echo ""
    echo "Error: You must provide a platform name as argument"
    echo ""
    echo "Usage: ./install.sh [platform]   where [platform] can be"
    echo "  win           Install OpenPLC on Windows over Cygwin"
    echo "  linux         Install OpenPLC on a Debian-based Linux distribution"
    echo "  alpine        Install OpenPLC on an Alpine-based Linux distribution"
    echo "  docker        Install OpenPLC in a Docker container"
    echo "  rpi           Install OpenPLC on a Raspberry Pi"
    echo "  neuron        Install OpenPLC on a UniPi Neuron PLC"
    echo "  custom        Skip all specific package installation and tries to install"
    echo "                OpenPLC assuming your system already has all dependencies met."
    echo "                This option can be useful if you're trying to install OpenPLC"
    echo "                on an unsuported Linux platform or had manually installed"
    echo "                all the dependency packages before."
    echo ""
    exit 1
fi

WORKING_DIR=$(pwd)
mkdir -p bin
touch etc/Config0.c
touch etc/Res0.c
touch etc/glueVars.cpp
touch etc/oplc_platform
touch etc/openplc_driver
touch etc/active_program
cp webserver/openplc.db ./etc/

# arg1: sudo or blank
function linux_install_deps {
    $1 apt-get update
    $1 apt-get install -y build-essential pkg-config bison flex autoconf \
                          automake libtool make git python3 python3-pip   \
                          sqlite3 cmake git
}

function alpine_install_deps {
    $1 apk add --no-cache autoconf automake bison cmake flex g++ make \
                          libtool linux-headers pkgconf python3 py3-flask \
                          py3-flask-login py3-pip py3-pyserial sqlite
    $1 pip3 install pymodbus
}

function install_py_deps {
    $1 pip3 install -r requirements.txt
}

function OPLC_background_service {
    echo ""
    echo "[OPENPLC SERVICE]"
    echo -e "[Unit]\nDescription=OpenPLC Service\nAfter=network.target\n\n[Service]\nType=simple\nRestart=always\nRestartSec=1\nUser=root\nGroup=root\nWorkingDirectory=$WORKING_DIR\nExecStart=$WORKING_DIR/start_openplc.sh\n\n[Install]\nWantedBy=multi-user.target" >> openplc.service
    $1 cp -rf ./openplc.service /lib/systemd/system/
    rm -rf openplc.service
    echo "Enabling OpenPLC Service..."
    $1 systemctl daemon-reload
    $1 systemctl enable openplc.service
}

# arg1: additional CMAKE args
function cmake_build_and_test {
    echo ""
    echo "Building the application"
    mkdir build
    cd build
    cmake .. -DOPLC_ALL=ON $1
    make

    echo "Executing platform tests"
    cd ../bin

    if [ "$1" == "win" ]; then
        ./gg_unit_test.exe
        # fakeit doesn't support O3 optimizations, and we don't have a way
        # to detect optimizations, so disable for now.
        #./oplc_unit_test.exe
    else
        ./gg_unit_test
        # fakeit doesn't support O3 optimizations, and we don't have a way
        # to detect optimizations, so disable for now.
        #./oplc_unit_test
    fi
}

if [ "$1" == "win" ]; then
    echo "Installing OpenPLC on Windows"
     fileformat=unix
    cp ./utils/apt-cyg/apt-cyg ./
    cp ./utils/apt-cyg/wget.exe /bin
    install apt-cyg /bin
    apt-cyg update
    apt-cyg install lynx
    rm -f /bin/wget.exe
    apt-cyg remove gcc-core gcc-g++ pkg-config automake autoconf libtool make python3 python3-pip sqlite3
    apt-cyg install wget gcc-core gcc-g++ git pkg-config automake autoconf libtool make python3 python3-pip sqlite3
    lynx -source https://bootstrap.pypa.io/get-pip.py > get-pip.py
    python3 get-pip.py
    pip install -r requirements.txt

    rm apt-cyg
    rm get-pip.py

    cmake_build_and_test
    
    if [ $? -ne 0 ]; then
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    
    cd ../scripts
    ./change_hardware_layer.sh blank

elif [ "$1" == "linux" ]; then
    echo "Installing OpenPLC on Linux"
    linux_install_deps sudo
    install_py_deps
    install_py_deps "sudo -H"

    cmake_build_and_test

    if [ $? -ne 0 ]; then
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    
    if [ "$2" != "-d" ]; then
        OPLC_background_service sudo
    fi
    
    cd ../scripts
    ./change_hardware_layer.sh blank_linux

elif [ "$1" == "alpine" ]; then
    echo "Installing OpenPLC on Alpine Linux"
    alpine_install_deps sudo

    cmake_build_and_test -DOPLC_MUSL=ON

    if [ $? -ne 0 ]; then
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"

    touch ../etc/alpine
    cd ../scripts
    ./change_hardware_layer.sh blank_linux

elif [ "$1" == "docker" ]; then
    echo "Installing OpenPLC on Linux inside Docker"

    cmake_build_and_test   
    
    if [ $? -ne 0 ]; then
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    
    cd ../scripts
    ./change_hardware_layer.sh blank_linux
    

elif [ "$1" == "rpi" ]; then
    echo "Installing OpenPLC on Raspberry Pi"

    linux_install_deps sudo
    sudo apt-get install -y wiringpi

    install_py_deps
    install_py_deps "sudo -H" 

    cmake_build_and_test
    if [ $? -ne 0 ]; then
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    
    if [ "$2" != "-d" ]; then
    OPLC_background_service sudo
    fi
    
    cd ../scripts
    ./change_hardware_layer.sh blank_linux
    

elif [ "$1" == "neuron" ]; then
    echo "Installing OpenPLC on UniPi Neuron PLC"
    echo ""
    echo "[DISABLING UNIPI SERVICES]"
    sudo systemctl stop neuronhost.service
    sudo systemctl disable neuronhost.service
    sudo systemctl stop neurontcp.service
    sudo systemctl disable neurontcp.service
    sudo systemctl stop evok.service
    sudo systemctl disable evok.service
    
    linux_install_deps sudo

    install_py_deps
    install_py_deps sudo "sudo -H"
    
    cmake_build_and_test  
    
    if [ $? -ne 0 ]; then
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
     
    if [ "$2" != "-d" ]; then
        OPLC_background_service sudo
    fi
    
    cd ../scripts
    ./change_hardware_layer.sh blank_linux
    
elif [ "$1" == "custom" ]; then
    echo "Installing OpenPLC on Custom Platform"
    
    cmake_build_and_test  

    if [ $? -ne 0 ]; then
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    
    cd ../scripts
    ./change_hardware_layer.sh blank_linux

else
    echo ""
    echo "Error: You must provide a platform name as argument"
    echo ""
    echo "Usage: ./install.sh [platform]   where [platform] can be"
    echo "  win           Install OpenPLC on Windows over Cygwin"
    echo "  linux         Install OpenPLC on a Debian-based Linux distribution"
    echo "  docker        Install OpenPLC in a Docker container"
    echo "  rpi           Install OpenPLC on a Raspberry Pi"
    echo "  neuron        Install OpenPLC on a UniPi Neuron PLC"
    echo "  custom        Skip all specific package installation and tries to install"
    echo "                OpenPLC assuming your system already has all dependencies met."
    echo "                This option can be useful if you're trying to install OpenPLC"
    echo "                on an unsuported Linux platform or had manually installed"
    echo "                all the dependency packages before."
    echo ""
    exit 1
fi
