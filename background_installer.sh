#!/bin/bash
OPENPLC_DIR="$PWD"
SWAP_FILE="$OPENPLC_DIR/swapfile"
WIRINGPI_VERSION="3.4"
VENV_DIR="$OPENPLC_DIR/.venv"

function print_help_and_exit {
    echo ""
    echo "Error: You must provide a platform name as argument"
    echo ""
    echo "Usage: ./install.sh [platform]   where [platform] can be"
    echo "  win           Install OpenPLC on Windows over Cygwin"
    echo "  linux         Install OpenPLC on a Debian-based Linux distribution"
    echo "  docker        Install OpenPLC in a Docker container"
    echo "  rpi           Install OpenPLC on a Raspberry Pi"
    echo "  opi           Install OpenPLC on a Orange Pi"
    echo "  neuron        Install OpenPLC on a UniPi Neuron PLC"
    echo "  unipi         Install OpenPLC on a Raspberry Pi with UniPi v1.1 PLC"
    echo "  custom        Skip all specific package installation and tries to install"
    echo "                OpenPLC assuming your system already has all dependencies met."
    echo "                This option can be useful if you're trying to install OpenPLC"
    echo "                on an unsuported Linux platform or had manually installed"
    echo "                all the dependency packages before."
    echo ""
    exit 1
}

[ $# -eq 0 ] && print_help_and_exit

function fail {
    echo "$*"
    echo "OpenPLC was NOT installed!"
    exit 1
}

#set -x 
# arg1: sudo or blank
function linux_install_deps {
    if [ -x /bin/yum ]; then
        yum clean expire-cache
        yum check-update
        $1 yum -q -y install curl make automake gcc gcc-c++ kernel-devel pkg-config bison flex autoconf libtool openssl-devel cmake python3
    #Installing dependencies for Ubuntu/Mint/Debian
    elif [ -x /usr/bin/apt-get ]; then
        $1 apt-get update
        $1 apt-get install -y build-essential pkg-config bison flex autoconf \
                              automake libtool make git \
                              sqlite3 cmake curl python3 python3-venv
    #Installing dependencies for opensuse tumbleweed
    elif [ -x /usr/bin/zypper ]; then
        $1 zypper ref
        $1 zypper in -y curl make automake gcc gcc-c++ kernel-devel pkg-config bison flex autoconf libtool openssl-devel cmake libmodbus-devel
        $1 zypper in -y python python-xml python3 python3-pip 
    else
        fail "Unsupported linux distro."
    fi
}

function install_wiringpi {
    echo "[WIRINGPI]"
    echo "Trying distribution package..."
    sudo apt-get install -y wiringpi && return 0

    echo "Falling back to direct download..."
    local FILE="wiringpi_${WIRINGPI_VERSION}_arm64.deb"
    local URL="https://github.com/WiringPi/WiringPi/releases/download/$WIRINGPI_VERSION/$FILE"
    (
        set -e
        wget -c -O "$OPENPLC_DIR/$FILE" "$URL"
        sudo dpkg -i "$OPENPLC_DIR/$FILE"
        sudo apt install -f
        rm -f "$OPENPLC_DIR/$FILE"
    ) || fail "Failed to install wiringpi."
}

function install_pigpio {
    echo "[PIGPIO]"
    echo "Trying distribution package..."
    sudo apt-get install -y pigpio && return 0

    echo "Falling back to direct download..."
    local URL="https://github.com/joan2937/pigpio/archive/master.zip"
    (
        set -e
        wget -c "$URL"
        unzip master.zip
        cd pigpio-master
        make
        sudo make install
        rm -f master.zip
    )
}

function install_wiringop {
    echo "[WIRINGOP]"
    local URL="https://github.com/orangepi-xunlong/wiringOP/archive/master.zip"
    (
        set -e
        rm -f master.zip
        wget -c "$URL"
        unzip -o master.zip
        cd wiringOP-master
        sudo ./build clean
        sudo ./build
        rm -f ../master.zip
    )
}

function install_py_deps {
    python3 -m venv "$VENV_DIR"
    "$VENV_DIR/bin/python3" -m pip install --upgrade pip
    if [ "$1" == "neuron" ]; then
        "$VENV_DIR/bin/python3" -m pip install flask==2.2.5 werkzeug==2.2.2 flask-login==0.6.2 pyserial pymodbus==2.5.3
    else
        "$VENV_DIR/bin/python3" -m pip install flask==2.3.3 werkzeug==2.3.7 flask-login==0.6.2 pyserial pymodbus==2.5.3
    fi
    python3 -m pip install pymodbus==2.5.3
}

function swap_on {
    echo "creating swapfile..."

    # Fallocate is instantaneous. Only use dd as fallback.
    $1 touch "$SWAP_FILE"
    $1 fallocate -zl 1G "$SWAP_FILE" ||
    $1 dd if=/dev/zero of="$SWAP_FILE" bs=1M count=1000

    $1 mkswap "$SWAP_FILE"
    $1 swapon "$SWAP_FILE"
}

function swap_off {
    echo "removing swapfile..."
    $1 swapoff "$SWAP_FILE"
    $1 rm -f "$SWAP_FILE"
}

function install_matiec {
    echo "[MATIEC COMPILER]"
    cd "$OPENPLC_DIR/utils/matiec_src"
    autoreconf -i
    ./configure
    make
    cp ./iec2c "$OPENPLC_DIR/webserver/" || fail "Error compiling MatIEC"
    cd "$OPENPLC_DIR"
}

function install_st_optimizer {
    echo "[ST OPTIMIZER]"
    cd "$OPENPLC_DIR/utils/st_optimizer_src"
    g++ st_optimizer.cpp -o "$OPENPLC_DIR/webserver/st_optimizer" || fail "Error compiling ST Optimizer"
    cd "$OPENPLC_DIR"
}

function install_glue_generator {
    echo "[GLUE GENERATOR]"
    cd "$OPENPLC_DIR/utils/glue_generator_src"
    g++ -std=c++11 glue_generator.cpp -o "$OPENPLC_DIR/webserver/core/glue_generator" || fail "Error compiling Glue Generator"
    cd "$OPENPLC_DIR"
}

function install_ethercat {
    echo "[EtherCAT]"
    cd "$OPENPLC_DIR/utils/ethercat_src"
    ./install.sh || fail "Error compiling EtherCAT"
    echo ethercat > "$OPENPLC_DIR/webserver/scripts/ethercat"
    cd "$OPENPLC_DIR"
}

function disable_ethercat {
    echo "" > "$OPENPLC_DIR/webserver/scripts/ethercat"
}

function install_opendnp3 {
    echo "[OPEN DNP3]"
    cd "$OPENPLC_DIR/utils/dnp3_src"
    swap_on "$1"
    cmake .
    make
    $1 make install || fail "Error installing OpenDNP3"
    $1 ldconfig
    swap_off "$1"
    cd "$OPENPLC_DIR"
}

function disable_opendnp3 {
    echo ""
    echo "[OPEN DNP3]"
    cd "$OPENPLC_DIR/webserver/core"
    if test -f dnp3.cpp; then
        mv dnp3.cpp dnp3.disabled || fail "Error disabling OpenDNP3"
    fi
    if test -f dnp3_dummy.disabled; then
        mv dnp3_dummy.disabled dnp3_dummy.cpp || fail "Error disabling OpenDNP3"
    fi
    cd "$OPENPLC_DIR"
}

function install_libmodbus {
    echo "[LIBMODBUS]"
    cd "$OPENPLC_DIR/utils/libmodbus_src"
    ./autogen.sh
    ./configure
    $1 make install || fail "Error installing Libmodbus"
    $1 ldconfig
    cd "$OPENPLC_DIR"

    # Fix for RPM-based distros
    if [ -x /bin/yum ]; then
        sudo cp /usr/local/lib/pkgconfig/libmodbus.pc /usr/share/pkgconfig/
        sudo cp /usr/local/lib/lib*.* /lib64/
    fi
}

function install_systemd_service() {
    if [ "$1" == "sudo" ]; then
        echo "[OPENPLC SERVICE]"
        $1 tee /lib/systemd/system/openplc.service > /dev/null <<EOF
[Unit]
Description=OpenPLC Service
After=network.target

[Service]
Type=simple
Restart=always
RestartSec=1
User=root
Group=root
WorkingDirectory=$OPENPLC_DIR
ExecStart=$OPENPLC_DIR/start_openplc.sh

[Install]
WantedBy=multi-user.target
EOF
        echo "Enabling OpenPLC Service..."
        $1 systemctl daemon-reload
        $1 systemctl enable openplc
    fi
}

function install_all_libs {
    install_matiec "$1"
    install_st_optimizer "$1"
    install_glue_generator "$1"
    install_opendnp3 "$1"
    disable_ethercat "$1"
    install_libmodbus "$1"
}

function finalize_install {
    echo "[FINALIZING]"
    cd "$OPENPLC_DIR/webserver/scripts"
    if [ "$1" == "win" ]; then
        ./change_hardware_layer.sh blank
    else
        ./change_hardware_layer.sh blank_linux
    fi
    ./compile_program.sh blank_program.st
    cat > "$OPENPLC_DIR/start_openplc.sh" <<EOF
#!/bin/bash
if [ -d "/docker_persistent" ]; then
    mkdir -p /docker_persistent/st_files
    cp -n /workdir/webserver/dnp3_default.cfg /docker_persistent/dnp3.cfg
    cp -n /workdir/webserver/openplc_default.db /docker_persistent/openplc.db
    cp -n /workdir/webserver/st_files_default/* /docker_persistent/st_files/
    cp -n /dev/null /docker_persistent/persistent.file
    cp -n /dev/null /docker_persistent/mbconfig.cfg
fi
cd "$OPENPLC_DIR/webserver"
"$OPENPLC_DIR/.venv/bin/python3" webserver.py
EOF
    chmod a+x "$OPENPLC_DIR/start_openplc.sh"
    cd "$OPENPLC_DIR"
}

if [ "$1" == "win" ]; then
    echo "Installing OpenPLC on Windows"
    cp ./utils/apt-cyg/apt-cyg ./
    install apt-cyg /bin
    apt-cyg update
    apt-cyg install lynx
    
    apt-cyg install gcc-core gcc-g++ git pkg-config automake autoconf libtool make sqlite3 python3
    lynx -source https://bootstrap.pypa.io/pip/get-pip.py > get-pip3.py

    #Setting up venv
    python3 -m venv "$VENV_DIR"
    "$VENV_DIR/bin/python3" get-pip3.py
    "$VENV_DIR/bin/python3" -m pip install flask==2.3.3 werkzeug==2.3.7 flask-login==0.6.2 pyserial pymodbus==2.5.3
    
    echo ""
    echo "[MATIEC COMPILER]"
    cp ./utils/matiec_src/bin_win32/*.* ./webserver/
    if [ $? -ne 0 ]; then
        echo "Error compiling MatIEC"
        echo "OpenPLC was NOT installed!"
        exit 1
    fi

    install_st_optimizer
    install_glue_generator
    disable_opendnp3
    install_libmodbus
    finalize_install win

elif [ "$1" == "win_msys2" ]; then
    echo "Installing OpenPLC on Windows"

    pacman -Suy --noconfirm
    pacman -S gcc git pkg-config automake autoconf libtool make sqlite3 python3 lynx --noconfirm
    
    lynx -source https://bootstrap.pypa.io/pip/get-pip.py > get-pip3.py

    #Setting up venv
    python3 -m venv "$VENV_DIR"
    "$VENV_DIR/bin/python3" get-pip3.py
    "$VENV_DIR/bin/python3" -m pip install flask==2.3.3 werkzeug==2.3.7 flask-login==0.6.2 pyserial pymodbus==2.5.3
    
    echo ""
    echo "[MATIEC COMPILER]"
    cp ./utils/matiec_src/bin_win32/*.* ./webserver/
    if [ $? -ne 0 ]; then
        echo "Error compiling MatIEC"
        echo "OpenPLC was NOT installed!"
        exit 1
    fi

    install_st_optimizer
    install_glue_generator
    disable_opendnp3
    install_libmodbus
    cp /usr/include/modbus/*.h /usr/include/
    finalize_install win

elif [ "$1" == "linux" ]; then

    echo "Installing OpenPLC on Linux"
    linux_install_deps sudo
    install_py_deps
    install_all_libs sudo
    [ "$2" == "ethercat" ] && install_ethercat
    install_systemd_service sudo
    finalize_install linux

elif [ "$1" == "docker" ]; then
    echo "Installing OpenPLC on Linux inside Docker"
    linux_install_deps
    install_py_deps
    install_all_libs
    # Create persistent folder for docker
    if [[ ! -d "/docker_persistent" ]]
    then
        mkdir /docker_persistent
    fi
    finalize_install linux

elif [ "$1" == "rpi" ]; then
    echo "Installing OpenPLC on Raspberry Pi"
    linux_install_deps sudo
    install_pigpio
    install_py_deps
    install_all_libs sudo
    install_systemd_service sudo
    finalize_install linux

elif [ "$1" == "opi" ]; then
    echo "Installing OpenPLC on Orange Pi"
    linux_install_deps sudo
    install_wiringop
    install_py_deps
    install_all_libs sudo
    install_systemd_service sudo
    finalize_install linux

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
    sudo systemctl stop unipitcp.service
    sudo systemctl disable unipitcp.service
    

    linux_install_deps sudo
    install_py_deps neuron
    install_all_libs sudo
    install_systemd_service sudo
    finalize_install linux

elif [ "$1" == "unipi" ]; then
    echo "Installing OpenPLC on Raspberry Pi with UniPi v1.1 PLC"
    linux_install_deps sudo
    install_wiringpi
    install_py_deps
    install_all_libs sudo
    install_systemd_service sudo
    finalize_install linux

elif [ "$1" == "custom" ]; then
    echo "Installing OpenPLC on Custom Platform"
    install_all_libs
    install_systemd_service sudo
    finalize_install linux

else
    print_help_and_exit
fi
