//-----------------------------------------------------------------------------
// Copyright 2024 Thiago Alves
// This file is part of the OpenPLC Runtime.
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
// This is the main file for the OpenPLC. It contains the initialization
// procedures for the hardware, network and the main loop
// Thiago Alves, Oct 2024
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "ladder.h"

pthread_mutex_t logLock; //mutex for the internal log
unsigned char log_buffer[1000000]; //A very large buffer to store all logs
int log_index = 0;
int log_counter = 0;
int64_t cycle_counter = 0;
uint8_t rpi_modbus_rts_pin;     // If <> 0, expect hardware RTS to be used with this pin

/**
 * @brief Makes the running thread sleep for the specified amount of time
 *
 * This function adjusts the given timespec structure by adding the specified delay,
 * and then uses clock_nanosleep to make the thread sleep until the adjusted time.
 *
 * @param ts Pointer to a timespec structure representing the start time
 * @param delay The amount of time to sleep in nanoseconds
 */
void sleep_until(struct timespec *ts, long long delay)
{
    ts->tv_sec  += delay / (1000*1000*1000);
    ts->tv_nsec += delay % (1000*1000*1000);
    if(ts->tv_nsec >= 1000*1000*1000)
    {
        ts->tv_nsec -= 1000*1000*1000;
        ts->tv_sec++;
    }
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, ts,  NULL);
}

/**
 * @brief Makes the running thread sleep for the specified amount of time in milliseconds
 *
 * This function creates a timespec structure based on the given milliseconds,
 * and then uses nanosleep to make the thread sleep for the specified duration.
 *
 * @param milliseconds The amount of time to sleep in milliseconds
 */
void sleepms(int milliseconds)
{
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

/**
 * @brief Compute the difference between two timespec structures
 *
 * This function calculates the difference between two timespec structures,
 * where the result is (a - b). It handles cases where the nanosecond
 * field may need to borrow from the second field.
 *
 * @param a Pointer to the minuend timespec
 * @param b Pointer to the subtrahend timespec
 * @param result Pointer to the timespec where the difference will be stored
 */
void timespec_diff(struct timespec *a, struct timespec *b, struct timespec *result) 
{
    // Calculate the difference in seconds
    result->tv_sec = a->tv_sec - b->tv_sec;
    
    // Calculate the difference in nanoseconds
    result->tv_nsec = a->tv_nsec - b->tv_nsec;
    
    // Handle borrowing if nanoseconds are negative
    if (result->tv_nsec < 0) 
    {
        // Borrow 1 second (1e9 nanoseconds)
        --result->tv_sec;
        result->tv_nsec += 1000000000L;
    }
}

/**
 * @brief Logs messages and prints them on the console
 *
 * This function logs messages to a global buffer and prints them to the console.
 * It uses a mutex to ensure thread-safe access to the shared log buffer.
 *
 * @param logmsg Pointer to a null-terminated string containing the message to be logged
 */
void log(char *logmsg)
{
    pthread_mutex_lock(&logLock); // lock mutex

    printf("%s", logmsg);

    size_t msg_len = strlen(logmsg);
    if (log_index + msg_len >= sizeof(log_buffer) - 1)
    {
        // Buffer overflow protection: clear the buffer if it's nearly full
        log_index = 0;
    }

    memcpy(&log_buffer[log_index], logmsg, msg_len);
    log_index += msg_len;
    log_buffer[log_index] = '\0';
    
    log_counter++;
    if (log_counter >= 1000)
    {
        // Clear log buffer
        log_counter = 0;
        log_index = 0;
    }

    pthread_mutex_unlock(&logLock); // unlock mutex
}

/**
 * @brief Creates the server to listen to commands on localhost
 *
 * This function starts an interactive server on a specified port to handle
 * incoming commands.
 *
 * @param arg Unused parameter (required for pthread creation)
 * @return void* Always returns NULL
 */
void *interactiveServerThread(void *arg)
{
    startInteractiveServer(43628);
    return NULL;
}

/**
 * @brief Disables all outputs
 *
 * This function sets all digital, byte, and analog outputs to zero,
 * effectively disabling them.
 */
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

/**
 * @brief Handles special functions for the OpenPLC runtime
 *
 * This function manages various special functions, including:
 * - Current time (stored in %ML1024)
 * - UTC time (stored in %ML1027)
 * - Number of cycles (stored in %ML1025)
 * - Communication error counter (implemented elsewhere)
 *
 * @note The function assumes that the `special_functions` array is properly initialized
 *       and that indices 0, 1, and 3 are valid.
 */
void handleSpecialFunctions()
{
    time_t rawtime;
    struct tm *current_time;
    
    time(&rawtime);
    
    // Store the UTC clock in [%ML1027]
    if (special_functions[3] != NULL) 
    {
        *special_functions[3] = rawtime;
    }

    current_time = localtime(&rawtime);
    
    // Adjust for local time
    time_t localtime = rawtime - timezone;
    if (current_time->tm_isdst > 0) 
    {
        localtime += 3600; // Add one hour for daylight saving time
    }
        
    // Store local time in [%ML1024]
    if (special_functions[0] != NULL) 
    {
        *special_functions[0] = localtime;
    }
    
    // Increment and store number of cycles [%ML1025]
    cycle_counter++;
    if (special_functions[1] != NULL) 
    {
        *special_functions[1] = cycle_counter;
    }
    
    // Communication error counter [%ML1026]
    /* Implemented in modbus_master.cpp */

    // Insert other special functions below
}

/**
 * @brief Records cycle time and sleep latency in special function registers
 *
 * This function stores the cycle time and sleep latency values in the special
 * function registers. These values are typically used for real-time performance
 * monitoring and diagnostics.
 *
 * @param cycle_time The execution time of the PLC cycle in microseconds
 * @param sleep_latency The latency between cycles in microseconds
 *
 * @note The function assumes that special_functions[4] and special_functions[5]
 *       are properly initialized and point to valid memory locations.
 */
void RecordCycletimeLatency(long cycle_time, long sleep_latency)
{
    if (special_functions[4] != NULL)
    {
        *special_functions[4] = static_cast<IEC_LINT>(cycle_time);
    }
    
    if (special_functions[5] != NULL)
    {
        *special_functions[5] = static_cast<IEC_LINT>(sleep_latency);
    }
}

/**
 * @brief Sets the Modbus RTS (Request to Send) pin
 *
 * This function sets the GPIO pin number to be used for hardware RTS control
 * in Modbus RTU over RS485.
 *
 * @param pin The GPIO pin number to be used for RTS control
 *
 * @note This function assumes that the external variable rpi_modbus_rts_pin
 *       is properly declared and accessible.
 */
void setModbusRtsPin(uint8_t pin)
{
    rpi_modbus_rts_pin = pin;
}
