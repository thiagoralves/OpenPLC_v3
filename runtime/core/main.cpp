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
#include "service/service_definition.h"
#include "service/service_registry.h"

/** \addtogroup openplc_runtime
 *  @{
 */

#define OPLC_CYCLE          50000000

extern int opterr;
IEC_BOOL __DEBUG;

IEC_LINT cycle_counter = 0;

unsigned long __tick = 0;
std::mutex bufferLock;  // Mutex for the internal buffers
uint8_t run_openplc = 1;  // Variable to control OpenPLC Runtime execution

////////////////////////////////////////////////////////////////////////////////
/// \brief Helper function - Makes the running thread sleep for the amount of
/// time in milliseconds
/// \param ts
/// \param delay in milliseconds
////////////////////////////////////////////////////////////////////////////////
void sleep_until(struct timespec *ts, int delay) {
    ts->tv_nsec += delay;
    if (ts->tv_nsec >= 1000*1000*1000)
    {
        ts->tv_nsec -= 1000*1000*1000;
        ts->tv_sec++;
    }
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, ts,  NULL);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Verify if pin is present in one of the ignored vectors
/// \param
/// \param
/// \param
/// \return
////////////////////////////////////////////////////////////////////////////////
bool pinNotPresent(int *ignored_vector, int vector_size, int pinNumber)
{
    for (int i = 0; i < vector_size; i++)
    {
        if (ignored_vector[i] == pinNumber)
        {
            return false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Disables all outputs
////////////////////////////////////////////////////////////////////////////////
void disableOutputs()
{
    // Disable digital outputs
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (bool_output[i][j] != NULL) *bool_output[i][j] = 0;
        }
    }

    // Disable byte outputs
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (byte_output[i] != NULL) *byte_output[i] = 0;
    }

    // Disable analog outputs
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (int_output[i] != NULL) *int_output[i] = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Special Functions
////////////////////////////////////////////////////////////////////////////////
void handleSpecialFunctions()
{
    // Current time [%ML1024]
    struct tm *current_time;
    time_t rawtime;

    tzset();
    time(&rawtime);
    current_time = localtime(&rawtime);

    rawtime = rawtime - timezone;
    if (current_time->tm_isdst > 0)
    {
        rawtime = rawtime + 3600;
    }

    if (special_functions[0] != NULL)
    {
        *special_functions[0] = rawtime;
    }

    // Number of cycles [%ML1025]
    cycle_counter++;
    if (special_functions[1] != NULL)
    {
        *special_functions[1] = cycle_counter;
    }

    // Insert other special functions below
}

int main(int argc, char **argv)
{
    initialize_logging(argc, argv);
    spdlog::info("OpenPLC Runtime starting...");

    time(&start_time);

    // Start the thread for the any services we have, including the
    // interactive server and anything out that is configured to
    // automatically start
    bootstrap();

    // Gets the starting point for the clock
    spdlog::debug("Getting current time");
    struct timespec timer_start;
    clock_gettime(CLOCK_MONOTONIC, &timer_start);

    //======================================================
    //                    MAIN LOOP
    //======================================================
    while (run_openplc)
    {

        // Read input image - this method tries to get the lock
        // so don't put it in the lock context.
        updateBuffersIn();
        {
            std::lock_guard<std::mutex> guard(bufferLock);
            updateCustomIn();
            // Update input image table with data from slave devices
            services_before_cycle();
            handleSpecialFunctions();
            // Execute plc program logic
            config_run__(__tick++);
            updateCustomOut();
            // Update slave devices with data from the output image table

            services_after_cycle();
        }

        // Write output image - this method tries to get the lock
        // so don't put it in the lock context.
        updateBuffersOut();

        updateTime();

        sleep_until(&timer_start, common_ticktime__);
    }

    //======================================================
    //             SHUTTING DOWN OPENPLC RUNTIME
    //======================================================
    services_stop();
    services_finalize();

    spdlog::debug("Disabling outputs...");
    disableOutputs();
    updateCustomOut();
    updateBuffersOut();
    spdlog::debug("Shutting down OpenPLC Runtime...");
    finalizeHardware();
    exit(0);
}

/** @}*/
