#!/bin/bash
if [ $# -eq 0 ]; then
    echo "Error: You must provide a file to be compiled as argument"
    exit 1
fi

HERE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
source $HERE_DIR/common.sh


OPENPLC_PLATFORM=$(cat $PLATFORM_FILE)

echo "#### $1 ###"
#store the active program filename
echo $(basename $1) > "$ACTIVE_PROGAM_FILE"

OUT_ST="$BUILD_DIR/opti.st"

mkdir $SRC_GEN_DIR

set -x

echo "Optimizing ST program..."
$BUILD_DIR/st_optimizer "$1" "$OUT_ST"
echo "Generating C files..."


#$BUILD_DIR/iec2c -I "$ETC_DIR/lib" -T $SRC_GEN_DIR "$OUT_ST"
cd $BUILD_DIR
./iec2c -I $ETC_DIR/lib -T ./src_gen ./opti.st
if [ $? -ne 0 ]; then
    echo "Error generating C files"
    echo "Compilation finished with errors!"
    exit 1
fi



# Copy cpp files
cp $CORE_DIR/ladder.h $SRC_GEN_DIR
cp $CORE_DIR/custom_layer.h $SRC_GEN_DIR
#cp $CORE_DIR/dnp3.cpp $SRC_GEN_DIR
cp $CORE_DIR/modbus.cpp $SRC_GEN_DIR
cp $CORE_DIR/modbus_master.cpp $SRC_GEN_DIR
cp $CORE_DIR/server.cpp $SRC_GEN_DIR
cp $CORE_DIR/interactive_server.cpp $SRC_GEN_DIR
cp $CORE_DIR/main.cpp $SRC_GEN_DIR

#copy config
#cp $CORE_DIR/dnp3.cfg $BUILD_DIR

#---------------------------------------
# compile for each platform
cd $SRC_GEN_DIR

if [ "$OPENPLC_PLATFORM" = "win" ]; then
    echo "Compiling for Windows"
    echo "Generating object files..."
    g++ -I $C_LIBS_DIR -c Config0.c -w
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    g++ -I $C_LIBS_DIR -c Res0.c -w
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Generating glueVars..."
    $BUILD_DIR/glue_generator
    echo "Compiling main program..."
    g++ *.cpp *.o -o $BUILD_DIR/openplc -I $C_LIBS_DIR -pthread -fpermissive -I /usr/local/include/modbus -L /usr/local/lib -lmodbus -w
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
    g++ -std=gnu++11 *.cpp *.o -o $BUILD_DIR/openplc -I $C_LIBS_DIR  -pthread -fpermissive `pkg-config --cflags --libs libmodbus` -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
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
    g++ -std=gnu++11 -I $C_LIBS_DIR -c Config0.c -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    g++ -std=gnu++11 -I $C_LIBS_DIR -c Res0.c -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
    if [ $? -ne 0 ]; then
        echo "Error compiling C files"
        echo "Compilation finished with errors!"
        exit 1
    fi
    echo "Generating glueVars..."
    $BUILD_DIR/glue_generator
    echo "Compiling main program..."
    g++ -std=gnu++11 *.cpp *.o -o $BUILD_DIR/openplc -I $C_LIBS_DIR -lrt -lwiringPi -lpthread -fpermissive `pkg-config --cflags --libs libmodbus` -lasiodnp3 -lasiopal -lopendnp3 -lopenpal -w
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
