#!/bin/bash

if [ $# -eq 0 ]; then
    echo ""
    echo "Error: You must provide a platform name as argument"
    echo ""
    echo "Usage: ./install.sh [platform]  where [platform] can be"
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

set -x

ERROR_MESS="OpenPLC was NOT installed!"
SCRIPTS_DIR="scripts"
BUILD_DIR="build"

# arg1: sudo or blank
function linux_install_deps {
    #$1 apt-get update
    $1 apt-get install -y build-essential pkg-config bison flex \
                          autoconf automake libtool make cmake \
                          python2.7 python-pip  \
                          sqlite3
}

function install_py_deps {
    $1 pip install -r requirements.txt
}

function install_all_libs {

    mkdir build

    echo ""
    echo "[MATIEC COMPILER]"
    cd utils/matiec_src
    autoreconf -i
    ./configure
    make
    cp ./iec2c ../../build/iec2c
    if [ $? -ne 0 ]; then
        echo "Error compiling MatIEC"
        echo "OpenPLC was NOT installed!"
        exit 1
    fi
    cd ../..

    echo ""
    echo "[ST OPTIMIZER]"
    cd utils/st_optimizer_src
    g++ st_optimizer.cpp -o ../../build/st_optimizer
    if [ $? -ne 0 ]; then
        echo "Error compiling ST Optimizer"
        echo "$ERROR_MESS"
        exit 1
    fi
    cd ../..

    echo ""
    echo "[GLUE GENERATOR]"
    cd utils/glue_generator_src
    g++ glue_generator.cpp -o ../../build/glue_generator
    if [ $? -ne 0 ]; then
        echo "Error compiling Glue Generator"
        echo "$ERROR_MESS"
        exit 1
    fi
    cd ../..

    echo ""
    echo "[OPEN DNP3]"
    mkdir build/dnp3
    #cd utils/dnp3_src
    cd build/dnp3
    echo "creating swapfile..."
    $1 dd if=/dev/zero of=swapfile bs=1M count=1000
    $1 mkswap swapfile
    $1 swapon swapfile
    #cmake ../dnp3_src
    cmake ../../utils/dnp3_src
    make
    $1 make install
    if [ $? -ne 0 ]; then
        echo "Error installing OpenDNP3"
        echo "OpenPLC was NOT installed!"
        exit 1
    fi
    $1 ldconfig
    echo "removing swapfile..."
    $1 swapoff swapfile
    $1 rm -f ./swapfile
    cd ../..

    echo ""
    echo "[LIBMODBUS]"
    cd utils/libmodbus_src
    ./autogen.sh
    ./configure
    $1 make install
    if [ $? -ne 0 ]; then
        echo "Error installing Libmodbus"
        echo "OpenPLC was NOT installed!"
        exit 1
    fi
    $1 ldconfig
    cd ../..

    if [ "$1" == "sudo" ]; then
        echo ""
        echo "[OPENPLC SERVICE]"
        WORKING_DIR=$(pwd)
        echo -e "[Unit]\n\
Description=OpenPLC Service\n\
After=network.target\n\
\n\
[Service]\n\
Type=simple\n\
Restart=always\n\
RestartSec=1\n\
User=root\n\
Group=root\n\
WorkingDirectory=$WORKING_DIR\n\
ExecStart=$WORKING_DIR/start_openplc.sh\n\
\n\
[Install]\n\
WantedBy=multi-user.target" >> ./build/openplc.service
        $1 cp -rf ./build/openplc.service /lib/systemd/system/
        rm -rf ./build/openplc.service
        $1 "Enabling OpenPLC Service..."
        $1 systemctl daemon-reload
        $1 systemctl enable openplc
    fi
}


if [ "$1" == "win" ]; then
    echo "Installing OpenPLC on Windows"
    cp ./utils/apt-cyg/apt-cyg ./
    cp ./utils/apt-cyg/wget.exe /bin
    install apt-cyg /bin
    apt-cyg install lynx
    rm -f /bin/wget.exe
    apt-cyg install wget gcc-core gcc-g++ git pkg-config automake autoconf libtool make python2 python2-pip sqlite3
    lynx -source https://bootstrap.pypa.io/get-pip.py > get-pip.py
    python get-pip.py
    pip install flask
    pip install flask-login
    pip install pyserial

    echo ""
    echo "[MATIEC COMPILER]"
    cp ./utils/matiec_src/bin_win32/*.* ./webserver/
    if [ $? -ne 0 ]; then
        echo "Error compiling MatIEC"
        echo "$ERROR_MESS"
        exit 1
    fi

    echo ""
    echo "[ST OPTIMIZER]"
    cd utils/st_optimizer_src
    g++ st_optimizer.cpp -o st_optimizer
    cp ./st_optimizer.exe ../../webserver/
    if [ $? -ne 0 ]; then
        echo "Error compiling ST Optimizer"
        echo "$ERROR_MESS"
        exit 1
    fi
    cd ../..

    echo ""
    echo "[GLUE GENERATOR]"
    cd utils/glue_generator_src
    g++ glue_generator.cpp -o glue_generator
    cp ./glue_generator.exe ../../webserver/core
    if [ $? -ne 0 ]; then
        echo "Error compiling Glue Generator"
        echo "$ERROR_MESS"
        exit 1
    fi
    cd ../..

    echo ""
    echo "[OPEN DNP3]"
    cd webserver/core
    mv dnp3.cpp dnp3.disabled
    if [ $? -ne 0 ]; then
        echo "Error disabling OpenDNP3"
        echo "$ERROR_MESS"
        exit 1
    fi
    mv dnp3_dummy.disabled dnp3_dummy.cpp
    if [ $? -ne 0 ]; then
        echo "Error disabling OpenDNP3"
        echo "$ERROR_MESS"
        exit 1
    fi
    cd ../..

    echo ""
    echo "[LIBMODBUS]"
    cd utils/libmodbus_src
    ./autogen.sh
    ./configure
    make install
    if [ $? -ne 0 ]; then
        echo "Error installing Libmodbus"
        echo "$ERROR_MESS"
        exit 1
    fi
    cd ../..

    echo ""
    echo "[FINALIZING]"
    cd webserver/scripts
    ./change_hardware_layer.sh blank
    ./compile_program.sh blank_program.st
    #cp ./start_openplc.sh ../../


elif [ "$1" == "linux" ]; then
    echo "Installing OpenPLC on Linux"
    #linux_install_deps sudo
    
    #install_py_deps
    #install_py_deps sudo

    install_all_libs sudo

    echo ""
    echo "[FINALIZING]"
    cd webserver/scripts
    ./change_hardware_layer.sh blank_linux
    ./compile_program.sh blank_program.st


elif [ "$1" == "docker" ]; then
    echo "Installing OpenPLC on Linux inside Docker"
    linux_install_deps
    install_py_deps
    install_all_libs

    echo ""
    echo "[FINALIZING]"
    cd webserver/scripts
    ./change_hardware_layer.sh blank_linux
    ./compile_program.sh blank_program.st

elif [ "$1" == "rpi" ]; then
    echo "Installing OpenPLC on Raspberry Pi"
    
    linux_install_deps sudo
    sudo apt-get install -y wiringpi

    install_py_deps
    install_py_deps sudo 

    install_all_libs sudo

    echo ""
    echo "[FINALIZING]"
    cd webserver/scripts
    ./change_hardware_layer.sh blank_linux
    ./compile_program.sh blank_program.st


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

    install_all_libs sudo

    echo ""
    echo "[FINALIZING]"
    cd webserver/scripts
    ./change_hardware_layer.sh blank_linux
    ./compile_program.sh blank_program.st



elif [ "$1" == "custom" ]; then
    echo "Installing OpenPLC on Custom Platform"

    install_all_libs

    echo ""
    echo "[FINALIZING]"
    cd webserver/scripts
    ./change_hardware_layer.sh blank_linux
    ./compile_program.sh blank_program.st


else
    echo ""
    echo "Error: unrecognized platform"
    echo ""
    echo "Usage: ./install.sh [platform]   where [platform] can be"
    echo "  win           Install OpenPLC on Windows over Cygwin"
    echo "  linux         Install OpenPLC on a Debian-based Linux distribution"
    echo "  rpi           Install OpenPLC on a Raspberry Pi"
    echo "  custom        Skip all specific package installation and tries to install"
    echo "                OpenPLC assuming your system already has all dependencies met."
    echo "                This option can be useful if you're trying to install OpenPLC"
    echo "                on an unsuported Linux platform or had manually installed"
    echo "                all the dependency packages before."
    echo ""
    exit 1
fi
