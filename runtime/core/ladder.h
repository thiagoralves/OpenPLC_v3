// Copyright 2015 Thiago Alves
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.


// This file is to be used in conjuction with OPLC_Compiler generated code.
// It is the "glue" that holds all parts of the OpenPLC together.
// Thiago Alves, Oct 2015
//-----------------------------------------------------------------------------

#include <pthread.h>
#include <cstdint>
#include <mutex>

#define MODBUS_PROTOCOL     0
#define DNP3_PROTOCOL       1
#define ENIP_PROTOCOL       2

//Internal buffers for I/O and memory. These buffers are defined in the
//auto-generated glueVars.cpp file
#define BUFFER_SIZE		1024
/*********************/
/*  IEC Types defs   */
/*********************/

typedef uint8_t  IEC_BOOL;

typedef int8_t    IEC_SINT;
typedef int16_t   IEC_INT;
typedef int32_t   IEC_DINT;
typedef int64_t   IEC_LINT;

typedef uint8_t    IEC_USINT;
typedef uint16_t   IEC_UINT;
typedef uint32_t   IEC_UDINT;
typedef uint64_t   IEC_ULINT;

typedef uint8_t    IEC_BYTE;
typedef uint16_t   IEC_WORD;
typedef uint32_t   IEC_DWORD;
typedef uint64_t   IEC_LWORD;

typedef float    IEC_REAL;
typedef double   IEC_LREAL;

//Booleans
extern IEC_BOOL *bool_input[BUFFER_SIZE][8];
extern IEC_BOOL *bool_output[BUFFER_SIZE][8];

//Bytes
extern IEC_BYTE *byte_input[BUFFER_SIZE];
extern IEC_BYTE *byte_output[BUFFER_SIZE];

//Analog I/O
extern IEC_UINT *int_input[BUFFER_SIZE];
extern IEC_UINT *int_output[BUFFER_SIZE];

//Memory
extern IEC_UINT *int_memory[BUFFER_SIZE];
extern IEC_DINT *dint_memory[BUFFER_SIZE];
extern IEC_LINT *lint_memory[BUFFER_SIZE];

//Special Functions
extern IEC_LINT *special_functions[BUFFER_SIZE];

//lock for the buffer
extern std::mutex bufferLock;

//Common task timer
extern unsigned long long common_ticktime__;

struct GlueVariable;

extern const std::uint16_t OPLCGLUE_INPUT_SIZE;
extern GlueVariable oplc_input_vars[];
extern const std::uint16_t OPLCGLUE_OUTPUT_SIZE;
extern GlueVariable oplc_output_vars[];

//----------------------------------------------------------------------
//FUNCTION PROTOTYPES
//----------------------------------------------------------------------

//MatIEC Compiler
extern "C" {
    void config_run__(unsigned long tick);  
    void config_init__(void);
}
//glueVars.cpp
void glueVars();
void updateTime();

//hardware_layer.cpp
void initializeHardware();
void finalizeHardware();
void updateBuffersIn();
void updateBuffersOut();

//custom_layer.h
void initCustomLayer();
void updateCustomIn();
void updateCustomOut();
extern int ignored_bool_inputs[];
extern int ignored_bool_outputs[];
extern int ignored_int_inputs[];
extern int ignored_int_outputs[];

//main.cpp
void sleep_until(struct timespec *ts, int delay);
void sleepms(int milliseconds);
bool pinNotPresent(int *ignored_vector, int vector_size, int pinNumber);
extern uint8_t run_openplc;
void handleSpecialFunctions();

//server.cpp
void startServer(uint16_t port, int protocol_type);
int getSO_ERROR(int fd);
void closeSocket(int fd);
bool SetSocketBlockingEnabled(int fd, bool blocking);

//interactive_server.cpp
void startInteractiveServer(int port);
void initializeLogging(int argc,char **argv);
extern bool run_modbus;
extern bool run_dnp3;
extern bool run_enip;
extern bool run_pstorage;
extern uint16_t pstorage_polling;
extern time_t start_time;
extern time_t end_time;

//modbus.cpp
int processModbusMessage(unsigned char *buffer, int bufferSize);
void mapUnusedIO();

//enip.cpp
int processEnipMessage(unsigned char *buffer, int buffer_size);

//modbus_master.cpp
void initializeMB();
void *querySlaveDevices(void *arg);
void updateBuffersIn_MB();
void updateBuffersOut_MB();

#ifdef OPLC_DNP3_OUTSTATION
//dnp3.cpp
void dnp3StartServer(int port);
#endif

//persistent_storage.cpp
void startPstorage();
int readPersistentStorage();
