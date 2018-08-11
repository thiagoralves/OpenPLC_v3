//-----------------------------------------------------------------------------
// Copyright 2015 Thiago Alves
//
// Based on the LDmicro software by Jonathan Westhues
// This file is part of the OpenPLC Software Stack.
//
// OpenPLC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenPLC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenPLC.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// This file is to be used in conjuction with OPLC_Compiler generated code.
// It is the "glue" that holds all parts of the OpenPLC together.
// Thiago Alves, Oct 2015
//-----------------------------------------------------------------------------

#include <pthread.h>
#include <stdint.h>

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

//lock for the buffer
extern pthread_mutex_t bufferLock;

//Common task timer
extern unsigned long long common_ticktime__;

//----------------------------------------------------------------------
//FUNCTION PROTOTYPES
//----------------------------------------------------------------------

//MatIEC Compiler
void config_run__(unsigned long tick);
void config_init__(void);

//glueVars.cpp
void glueVars();
void updateTime();

//hardware_layer.cpp
void initializeHardware();
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
void log(unsigned char *logmsg);
bool pinNotPresent(int *ignored_vector, int vector_size, int pinNumber);
extern uint8_t run_openplc;
extern unsigned char log_buffer[1000000];
extern int log_index;

//server.cpp
void startServer(int port);
int getSO_ERROR(int fd);
void closeSocket(int fd);
bool SetSocketBlockingEnabled(int fd, bool blocking);

//interactive_server.cpp
void startInteractiveServer(int port);
extern bool run_modbus;
extern bool run_dnp3;
extern time_t start_time;
extern time_t end_time;

//modbus.cpp
int processModbusMessage(unsigned char *buffer, int bufferSize);
void mapUnusedIO();

//modbus_master.cpp
void initializeMB();
void querySlaveDevices();
void updateBuffersIn_MB();
void updateBuffersOut_MB();

//dnp3.cpp
void dnp3StartServer(int port);

//persistent_storage.cpp
void *persistentStorage(void *args);
int readPersistentStorage();
