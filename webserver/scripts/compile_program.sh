#!/bin/bash
if [ $# -eq 0 ]; then
    echo "Error: You must provide a file to be compiled as argument"
    exit 1
fi

#move into the scripts folder if you're not there already
cd scripts &>/dev/null

OPENPLC_PLATFORM=$(cat openplc_platform)
ETHERCAT_OPT=$(cat ethercat)
OPENPLC_DRIVER=$(cat openplc_driver)

#store the active program filename
echo "$1" > ../active_program

#compiling the ST file into C
cd ..
echo "Generating C files..."
./iec2c -f -l -p -r -R -a ./st_files/"$1"
if [ $? -ne 0 ]; then
    echo "Error generating C files"
    echo "Compilation finished with errors!"
    exit 1
fi

# stick reference to ethercat_src in there for CoE access etc functionality that needs to be accessed from PLC
if [ "$ETHERCAT_OPT" = "ethercat" ]; then
    sed -i '7s/^/#include "ethercat_src.h" /' Res0.c
fi

# I prefer copying every time these two (small files) because could be useful to have a copy of them for testing
echo "Including Siemens S7 Protocol via snap7"
cp -f ../utils/snap7_src/wrapper/oplc_snap7.* ./core

echo "Moving Files..."
mv -f POUS.c POUS.h LOCATED_VARIABLES.h VARIABLES.csv Config0.c Config0.h Res0.c ./core/
if [ $? -ne 0 ]; then
    echo "Error moving files"
    echo "Compilation finished with errors!"
    exit 1
fi

if [ "$ETHERCAT_OPT" = "ethercat" ]; then
    echo "Including EtherCAT"
    ETHERCAT_INC="-L../../utils/ethercat_src/build/lib -lethercat_src -I../../utils/ethercat_src/src -D _ethercat_src"
else
    ETHERCAT_INC=""
fi

# IceOryx2 DEL (Data Exchange Layer) setup
ICEORYX2_INC=""
if [ -f scripts/iceoryx2 ] && [ "$(cat scripts/iceoryx2 2>/dev/null)" = "iceoryx2" ]; then
    echo "Including IceOryx2 DEL"
    ICEORYX2_INC="-D_iceoryx2_src -std=gnu++17"
    # Try pkg-config first, fall back to local build paths
    if pkg-config --exists iceoryx2-c 2>/dev/null; then
        ICEORYX2_INC="$ICEORYX2_INC $(pkg-config --cflags --libs iceoryx2-c)"
    else
        ICEORYX2_INC="$ICEORYX2_INC -I../../utils/iceoryx2_src/iceoryx2-ffi/ffi/include -L../../utils/iceoryx2_src/target/release -liceoryx2_ffi"
    fi
fi

#compiling for each platform
cd core
if [ "$OPENPLC_PLATFORM" = "win" ]; then
    echo "Compiling for Windows"
    echo "Generating object files..."
    g++ -I ./lib -c Config0.c -w
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    g++ -I ./lib -c Res0.c -w
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Generating glueVars..."
    ./glue_generator
    echo "Generating IceOryx2 bridge..."
    ./iceoryx2_generator
    echo "Compiling main program..."
    g++ *.cpp *.o -o openplc -I ./lib -pthread -fpermissive -I /usr/local/include/modbus -L /usr/local/lib snap7.lib -lmodbus -w $ICEORYX2_INC
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    exit 0

elif [ "$OPENPLC_PLATFORM" = "linux" ]; then
    echo "Compiling for Linux"
    echo "Generating object files..."
    if [ "$OPENPLC_DRIVER" = "sl_rp4" ]; then
        g++ -std=gnu++11 -I ./lib -c Config0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w -DSL_RP4
    elif [ "$OPENPLC_DRIVER" = "synergy_logic" ]; then
        g++ -std=gnu++11 -I ./lib -c Config0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w -DSYNERGY
    else
        g++ -std=gnu++11 -I ./lib -c Config0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    fi
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    if [ "$OPENPLC_DRIVER" = "sl_rp4" ]; then
        g++ -std=gnu++11 -I ./lib -c Res0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $ETHERCAT_INC -DSL_RP4
    elif [ "$OPENPLC_DRIVER" = "synergy_logic" ]; then
        g++ -std=gnu++11 -I ./lib -c Res0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $ETHERCAT_INC -DSYNERGY
    else
        g++ -std=gnu++11 -I ./lib -c Res0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $ETHERCAT_INC 
    fi
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Generating glueVars..."
    ./glue_generator
    echo "Generating IceOryx2 bridge..."
    ./iceoryx2_generator
    echo "Compiling main program..."
    if [ "$OPENPLC_DRIVER" = "sl_rp4" ]; then
        g++ -std=gnu++11 *.cpp *.o -o openplc -I ./lib -pthread -fpermissive `pkg-config --cflags --libs libmodbus` -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -lrt -w $ETHERCAT_INC $ICEORYX2_INC -DSL_RP4
    elif [ "$OPENPLC_DRIVER" = "synergy_logic" ]; then
        g++ -std=gnu++11 *.cpp *.o -o openplc -I ./lib -pthread -fpermissive `pkg-config --cflags --libs libmodbus` -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -lrt -w $ETHERCAT_INC $ICEORYX2_INC -DSYNERGY
    else
        g++ -std=gnu++11 *.cpp *.o -o openplc -I ./lib -pthread -fpermissive `pkg-config --cflags --libs libmodbus` -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -lrt -w $ETHERCAT_INC $ICEORYX2_INC
    fi
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    exit 0

elif [ "$OPENPLC_PLATFORM" = "rpi" ]; then
    echo "Compiling for Raspberry Pi"
    echo "Generating object files..."
    if [ "$OPENPLC_DRIVER" = "sequent" ]; then
        g++ -std=gnu++11 -I ./lib -c Config0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w -DSEQUENT
    else
        g++ -std=gnu++11 -I ./lib -c Config0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    fi
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    if [ "$OPENPLC_DRIVER" = "sequent" ]; then
        g++ -std=gnu++11 -I ./lib -c Res0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w -DSEQUENT
    else
        g++ -std=gnu++11 -I ./lib -c Res0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    fi
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Generating glueVars..."
    ./glue_generator
    echo "Generating IceOryx2 bridge..."
    ./iceoryx2_generator
    echo "Compiling main program..."
    if [ "$OPENPLC_DRIVER" = "sequent" ]; then
        g++ -DSEQUENT -std=gnu++11 *.cpp *.o -o openplc -I ./lib -lrt -lwiringPi -lpthread -fpermissive `pkg-config --cflags --libs libmodbus` -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $ICEORYX2_INC
    else
        g++ -std=gnu++11 *.cpp *.o -o openplc -I ./lib -lrt -lwiringPi -lpthread -fpermissive `pkg-config --cflags --libs libmodbus` -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $ICEORYX2_INC
    fi
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    exit 0

elif [ "$OPENPLC_PLATFORM" = "opi" ]; then
    WIRINGOP_INC="-I/usr/local/include -L/usr/local/lib -lwiringPi -lwiringPiDev"
    echo "Compiling for Orange Pi"
    echo "Generating object files..."
    g++ -std=gnu++11 -I ./lib -c Config0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $WIRINGOP_INC
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    g++ -std=gnu++11 -I ./lib -c Res0.c -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $WIRINGOP_INC
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Generating glueVars..."
    ./glue_generator
    echo "Generating IceOryx2 bridge..."
    ./iceoryx2_generator
    echo "Compiling main program..."
    g++ -std=gnu++11 *.cpp *.o -o openplc -I ./lib -lrt -lpthread -fpermissive `pkg-config --cflags --libs libmodbus` -lsnap7 -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $WIRINGOP_INC $ICEORYX2_INC
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
