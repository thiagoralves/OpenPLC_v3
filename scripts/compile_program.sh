#!/bin/bash
if [ $# -eq 0 ]; then
    echo "Error: You must provide a file to be compiled as argument"
    exit 1
fi

HERE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
source $HERE_DIR/common.sh

C_LIBS_DIR="$CORE_DIR/lib"
#move into the scripts folder if you're not there already
#cd scripts &>/dev/null


OPENPLC_PLATFORM=$(cat $PLATFORM_FILE)

echo "#### $1 ###"
#store the active program filename
echo "$1" > "$ACTIVE_PROGAM_FILE"

OUT_ST="$BUILD_DIR/opti.st"

#compiling the ST file into C
#cd ..
echo "Optimizing ST program..."
$BUILD_DIR/st_optimizer "$1" "$OUT_ST"
echo "Generating C files..."
#$BUILD_DIR/iec2c $BUILD_DIR/st_files/ "$OUT_ST"
$BUILD_DIR/iec2c -I "$ETC_DIR/lib" -T $BUILD_DIR "$OUT_ST"
if [ $? -ne 0 ]; then
    echo "Error generating C files"
    echo "Compilation finished with errors!"
    exit 1
fi
#echo "Moving Files..."
#mv -f POUS.c POUS.h LOCATED_VARIABLES.h VARIABLES.csv Config0.c Config0.h Res0.c ./core/
#if [ $? -ne 0 ]; then
#    echo "Error moving files"
#    echo "Compilation finished with errors!"
#    exit 1
#fi

#---------------------------------------
# compile for each platform

cd $BUILD_DIR

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
    g++ *.cpp *.o -o openplc -I ./lib -pthread -fpermissive -I /usr/local/include/modbus -L /usr/local/lib -lmodbus -w
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
    g++ -std=gnu++11 -I $C_LIBS_DIR -c Config0.c -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    if [ $? -ne 0 ]; then
        echo "Error compiling C Config0.c files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    g++ -std=gnu++11 -I $C_LIBS_DIR -c Res0.c -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    if [ $? -ne 0 ]; then
        echo "Error compiling C  Res0.c files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Generating glueVars..."
    $BUILD_DIR/glue_generator
    echo "Compiling main program..."
    g++ -std=gnu++11 *.cpp *.o -o openplc -I $C_LIBS_DIR -pthread -fpermissive `pkg-config --cflags --libs libmodbus` -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    if [ $? -ne 0 ]; then
        echo "Error compiling C openplc files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Compilation finished successfully!"
    exit 0
    
elif [ "$OPENPLC_PLATFORM" = "rpi" ]; then
    echo "Compiling for Raspberry Pi"
    echo "Generating object files..."
    g++ -std=gnu++11 -I ./lib -c Config0.c -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    g++ -std=gnu++11 -I ./lib -c Res0.c -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Generating glueVars..."
    ./glue_generator
    echo "Compiling main program..."
    g++ -std=gnu++11 *.cpp *.o -o openplc -I ./lib -lrt -lwiringPi -lpthread -fpermissive `pkg-config --cflags --libs libmodbus` -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
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
