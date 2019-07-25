#!/bin/bash

if [ $# -eq 0 ]; then
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
                          automake libtool make git python2.7 python-pip  \
                          sqlite3 cmake git
}

function install_py_deps {
    $1 pip install -r requirements.txt
}

function OPLC_background_service {
    echo ""
    echo "[OPENPLC SERVICE]"
    WORKING_DIR=$(pwd)
    echo -e "[Unit]\nDescription=OpenPLC Service\nAfter=network.target\n\n[Service]\nType=simple\nRestart=always\nRestartSec=1\nUser=root\nGroup=root\nWorkingDirectory=$WORKING_DIR\nExecStart=$WORKING_DIR/start_openplc.sh\n\n[Install]\nWantedBy=multi-user.target" >> openplc.service
    $1 cp -rf ./openplc.service /lib/systemd/system/
    rm -rf openplc.service
    echo "Enabling OpenPLC Service..."
    $1 systemctl daemon-reload
    $1 systemctl enable openplc.service
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
    apt-cyg remove gcc-core gcc-g++ pkg-config automake autoconf libtool make python2 python2-pip sqlite3
    apt-cyg install wget gcc-core gcc-g++ git pkg-config automake autoconf libtool make python2 python2-pip sqlite3
    lynx -source https://bootstrap.pypa.io/get-pip.py > get-pip.py
    python get-pip.py
    pip install flask
    pip install flask-login
    pip install pyserial
    
    rm apt-cyg
    rm get-pip.py

    mkdir build
    cd build
    cmake .. -DOPLC_ALL=ON
    make
    
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
    install_py_deps sudo
    
    mkdir build
    cd build
    cmake .. -DOPLC_ALL_DEPENDENCIES=ON -DOPLC_DNP3_ENABLE=ON
    make
    sudo ldconfig
    cmake .. -DOPLC_BUILD_SOURCE=ON -DOPLC_DNP3_ENABLE=ON
    make    

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

elif [ "$1" == "docker" ]; then
    echo "Installing OpenPLC on Linux inside Docker"
    linux_install_deps
    install_py_deps
    
    mkdir build
    cd build
    cmake .. -DOPLC_ALL_DEPENDENCIES=ON -DOPLC_DNP3_ENABLE=ON
    make
    ldconfig
    cmake .. -DOPLC_BUILD_SOURCE=ON -DOPLC_DNP3_ENABLE=ON
    make    
    
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
    install_py_deps sudo 
    
    cmake .. -DOPLC_ALL_DEPENDENCIES=ON -DOPLC_DNP3_ENABLE=ON
    make
    sudo ldconfig
    cmake .. -DOPLC_BUILD_SOURCE=ON -DOPLC_DNP3_ENABLE=ON
    make    
    
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
    install_py_deps sudo
    
    cmake .. -DOPLC_ALL_DEPENDENCIES=ON -DOPLC_DNP3_ENABLE=ON
    make
    sudo ldconfig
    cmake .. -DOPLC_BUILD_SOURCE=ON -DOPLC_DNP3_ENABLE=ON
    make    
    
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
    
    cmake .. -DOPLC_ALL_DEPENDENCIES=ON -DOPLC_DNP3_ENABLE=ON
    make
    ldconfig
    cmake .. -DOPLC_BUILD_SOURCE=ON -DOPLC_DNP3_ENABLE=ON
    make    

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
