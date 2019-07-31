// Copyright 2018 Thiago Alves
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


// This is the main file for the OpenPLC. It contains the initialization
// procedures for the hardware, network and the main loop
// Thiago Alves, Jun 2018
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include <spdlog/spdlog.h>

#include "iec_types.h"
#include "ladder.h"

#define OPLC_CYCLE          50000000

extern int opterr;
//extern int common_ticktime__;
IEC_BOOL __DEBUG;

IEC_LINT cycle_counter = 0;

static int tick = 0;
std::mutex bufferLock; //mutex for the internal buffers
uint8_t run_openplc = 1; //Variable to control OpenPLC Runtime execution

//-----------------------------------------------------------------------------
// Helper function - Makes the running thread sleep for the ammount of time
// in milliseconds
//-----------------------------------------------------------------------------
void sleep_until(struct timespec *ts, int delay)
{
    ts->tv_nsec += delay;
    if(ts->tv_nsec >= 1000*1000*1000)
    {
        ts->tv_nsec -= 1000*1000*1000;
        ts->tv_sec++;
    }
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, ts,  NULL);
}

//-----------------------------------------------------------------------------
// Helper function - Makes the running thread sleep for the ammount of time
// in milliseconds
//-----------------------------------------------------------------------------
void sleepms(int milliseconds)
{
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

//-----------------------------------------------------------------------------
// Interactive Server Thread. Creates the server to listen to commands on
// localhost
//-----------------------------------------------------------------------------
void *interactiveServerThread(void *arg)
{
    startInteractiveServer(43628);
    return nullptr;
}

//-----------------------------------------------------------------------------
// Verify if pin is present in one of the ignored vectors
//-----------------------------------------------------------------------------
bool pinNotPresent(int *ignored_vector, int vector_size, int pinNumber)
{
    for (int i = 0; i < vector_size; i++)
    {
        if (ignored_vector[i] == pinNumber)
            return false;
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// Disable all outputs
//-----------------------------------------------------------------------------
void disableOutputs()
{
    //Disable digital outputs
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (bool_output[i][j] != NULL) *bool_output[i][j] = 0;
        }
    }
    
    //Disable byte outputs
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (byte_output[i] != NULL) *byte_output[i] = 0;
    }
    
    //Disable analog outputs
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (int_output[i] != NULL) *int_output[i] = 0;
    }
}

//-----------------------------------------------------------------------------
// Special Functions
//-----------------------------------------------------------------------------
void handleSpecialFunctions()
{
    //current time [%ML1024]
    struct tm *current_time;
    time_t rawtime;
    
    tzset();
    time(&rawtime);
    current_time = localtime(&rawtime);
    
    rawtime = rawtime - timezone;
    if (current_time->tm_isdst > 0) rawtime = rawtime + 3600;
        
    if (special_functions[0] != NULL) *special_functions[0] = rawtime;
    
    //number of cycles [%ML1025]
    cycle_counter++;
    if (special_functions[1] != NULL) *special_functions[1] = cycle_counter;
    
    //comm error counter [%ML1026]
    /* Implemented in modbus_master.cpp */

    //insert other special functions below
}

int main(int argc,char **argv)
{
    initializeLogging(argc, argv);
	spdlog::info("OpenPLC Runtime starting...");

    //======================================================
    //                 PLC INITIALIZATION
    //======================================================
    time(&start_time);
    pthread_t interactive_thread;
    pthread_create(&interactive_thread, NULL, interactiveServerThread, NULL);
    config_init__();
    glueVars();

    //======================================================
    //              HARDWARE INITIALIZATION
    //======================================================
    initializeHardware();
    initializeMB();
    initCustomLayer();
    updateBuffersIn();
    updateCustomIn();
    updateBuffersOut();
    updateCustomOut();

    //======================================================
    //          PERSISTENT STORAGE INITIALIZATION
    //======================================================
    glueVars();
    mapUnusedIO();
    readPersistentStorage();
    //pthread_t persistentThread;
    //pthread_create(&persistentThread, NULL, persistentStorage, NULL);

#ifdef __linux__
    //======================================================
    //              REAL-TIME INITIALIZATION
    //======================================================
    // Set our thread to real time priority
    struct sched_param sp;
    sp.sched_priority = 30;
    spdlog::info("Setting main thread priority to RT");
    if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp))
    {
        spdlog::warn("WARNING: Failed to set main thread to real-time priority");
    }

    // Lock memory to ensure no swapping is done.
    spdlog::info("Locking main thread memory");
    if(mlockall(MCL_FUTURE|MCL_CURRENT))
    {
        spdlog::warn("WARNING: Failed to lock memory");
    }
#endif

	//gets the starting point for the clock
	spdlog::debug("Getting current time");
	struct timespec timer_start;
	clock_gettime(CLOCK_MONOTONIC, &timer_start);

	//======================================================
	//                    MAIN LOOP
	//======================================================
	while(run_openplc)
	{
		//make sure the buffer pointers are correct and
		//attached to the user variables
		glueVars();
        
		updateBuffersIn(); //read input image

		
		{
			std::lock_guard<std::mutex> guard(bufferLock);
			updateCustomIn();
			updateBuffersIn_MB(); //update input image table with data from slave devices
			handleSpecialFunctions();
			config_run__(tick++); // execute plc program logic
			updateCustomOut();
			updateBuffersOut_MB(); //update slave devices with data from the output image table
		}

		updateBuffersOut(); //write output image
        
		updateTime();

		sleep_until(&timer_start, common_ticktime__);
	}
    
    //======================================================
	//             SHUTTING DOWN OPENPLC RUNTIME
	//======================================================
    pthread_join(interactive_thread, NULL);
	spdlog::debug("Disabling outputs...");
    disableOutputs();
    updateCustomOut();
    updateBuffersOut();
	spdlog::debug("Shutting down OpenPLC Runtime...");
	finalizeHardware();
    exit(0);
}
