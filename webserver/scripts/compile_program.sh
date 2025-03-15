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
S7_OPT=$(cat s7protocol)

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

#check for S7 Protocol option
if [ "$S7_OPT" = "s7protocol" ]; then
    S7_LIB="-lsnap7"
    S7_DEF="-D _snap7"
    S7_WLIB="snap7.lib"
    cp ../utils/snap7_src/wrapper/oplc_snap7.* ./core
    if [ "$OPENPLC_PLATFORM" = "win" ]; then
        cp ../utils/snap7_src/build/bin/win64/snap7.* ./core
    fi
    echo "Including Siemens S7 Protocol via snap7"
else
    S7_LIB=""
    S7_DEF=""
    S7_WLIB=""
    # remove snap7 references (if any)
    rm -f ./core/oplc_snap7.*
    if [ "$OPENPLC_PLATFORM" = "win" ]; then
        rm -f ./core/snap7.*
    fi
fi

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
    echo "Compiling main program..."
    g++ *.cpp *.o -o openplc -I ./lib -pthread -fpermissive -I /usr/local/include/modbus -L /usr/local/lib $S7_WLIB -lmodbus -w $S7_DEF
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
        g++ -std=gnu++11 -I ./lib -c Config0.c $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w -DSL_RP4
    else
        g++ -std=gnu++11 -I ./lib -c Config0.c $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    fi
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    if [ "$OPENPLC_DRIVER" = "sl_rp4" ]; then
        g++ -std=gnu++11 -I ./lib -c Res0.c $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $ETHERCAT_INC $S7_DEF -DSL_RP4
    else
        g++ -std=gnu++11 -I ./lib -c Res0.c $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $ETHERCAT_INC $S7_DEF 
    fi
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Generating glueVars..."
    ./glue_generator
    echo "Compiling main program..."
    if [ "$OPENPLC_DRIVER" = "sl_rp4" ]; then
        g++ -std=gnu++11 *.cpp *.o -o openplc -I ./lib -pthread -fpermissive `pkg-config --cflags --libs libmodbus` $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $ETHERCAT_INC $S7_DEF -DSL_RP4
    else
        g++ -std=gnu++11 *.cpp *.o -o openplc -I ./lib -pthread -fpermissive `pkg-config --cflags --libs libmodbus` $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $ETHERCAT_INC $S7_DEF
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
        g++ -std=gnu++11 -I ./lib -c Config0.c $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w -DSEQUENT
    else
        g++ -std=gnu++11 -I ./lib -c Config0.c $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    fi
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    if [ "$OPENPLC_DRIVER" = "sequent" ]; then
        g++ -std=gnu++11 -I ./lib -c Res0.c $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w -DSEQUENT
    else
        g++ -std=gnu++11 -I ./lib -c Res0.c $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    fi
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Generating glueVars..."
    ./glue_generator
    echo "Compiling main program..."
    if [ "$OPENPLC_DRIVER" = "sequent" ]; then
        g++ -DSEQUENT -std=gnu++11 *.cpp *.o -o openplc -I ./lib -lrt -lpigpio -lpthread -fpermissive `pkg-config --cflags --libs libmodbus` $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal $S7_DEF -w 
    else    
        g++ -std=gnu++11 *.cpp *.o -o openplc -I ./lib -lrt -lpigpio -lpthread -fpermissive `pkg-config --cflags --libs libmodbus` $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal $S7_DEF -w
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
    g++ -std=gnu++11 -I ./lib -c Config0.c $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $WIRINGOP_INC
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    g++ -std=gnu++11 -I ./lib -c Res0.c $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $WIRINGOP_INC
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Generating glueVars..."
    ./glue_generator
    echo "Compiling main program..."
    g++ -std=gnu++11 *.cpp *.o -o openplc -I ./lib -lrt -lpthread -fpermissive `pkg-config --cflags --libs libmodbus` $S7_LIB -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w $S7_DEF $WIRINGOP_INC
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
