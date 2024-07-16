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
// This file is the hardware layer for the OpenPLC. If you change the platform
// where it is running, you may only need to change this file. All the I/O
// related stuff is here. Basically it provides functions to read and write
// to the OpenPLC internal buffers in order to update I/O state.
// Thiago Alves, Dec 2015
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/select.h>
#include <errno.h>

#define MB_PORT		2605

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define ERROR_LIMIT    10
#define BUFFER_LIMIT   1024 

#include "ladder.h"
#include "custom_layer.h"

int psm = 0;
int error_count = 0;


//-----------------------------------------------------------------------------
// Verify if error count is at the limit and, if true, disable PSM
//-----------------------------------------------------------------------------
void check_error_count()
{
    if (error_count >= ERROR_LIMIT)
    {
        char log_msg[1000];
        sprintf(log_msg, "PSM: Too many errors!\nPSM: PSM is disabled\n");
        log(log_msg);
    }
}


//-----------------------------------------------------------------------------
// PSM Thread - Start python3 interpreter running PSM script
//-----------------------------------------------------------------------------
void *start_psm()
{
    char log_msg[BUFFER_LIMIT];
    sprintf(log_msg, "PSM: Starting PSM...\n");
    log(log_msg);
    char *cmd = "../.venv/bin/python3 -u ./core/psm/main.py 2>&1";
    
    FILE *psm_proc;
    if ((psm_proc = popen(cmd, "r")) == NULL)
    {
        sprintf(log_msg, "PSM: Error opening pipe!\n");
        log(log_msg);
        return;
    }
    
    while (fgets(log_msg, BUFFER_LIMIT, psm_proc) != NULL)
    {
        log(log_msg);
    }
    
    if (pclose(psm_proc))
    {
        sprintf(log_msg, "PSM: Error while starting Python interpreter\n");
        log(log_msg);
        return;
    }
}

//-----------------------------------------------------------------------------
// Initializes TCP socket and connects to python PSM module
//-----------------------------------------------------------------------------
int connect_to_psm(int debug)
{
    char log_msg[1000];
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        if (debug)
        {
            sprintf(log_msg, "PSM: Socket creation error \n");
            log(log_msg);
        }
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(MB_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        if (debug)
        {
            sprintf(log_msg, "PSM: Invalid address or address not supported\n");
            log(log_msg);
        }
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        if (debug)
        {
            sprintf(log_msg, "PSM: Connection to psm failed\n");
            log(log_msg);
        }
        return -1;
    }

    return sock;
}

//-----------------------------------------------------------------------------
// Read analog inputs from PSM through Modbus Read Input Register function
//-----------------------------------------------------------------------------
void read_ana_inp(int psm)
{
    char log_msg[1000];
    //                          tid     pid     len   uid  fc  start   count
    unsigned char request[] = {00, 01, 00, 00, 00, 06, 00, 04, 00, 00, 00, 25};
    unsigned char buffer[1024];
    send(psm, request, 12, 0);
    read(psm, buffer, 1024);

    //check if response is complete
    if (buffer[8] < 50)
        return;

    pthread_mutex_lock(&bufferLock); //lock mutex
    int k = 9; //start at the beginning of data
    for (int i=0; i<25; i++)
    {
        if (int_input[i] != NULL) *int_input[i] = buffer[k+1] | (uint16_t)buffer[k] << 8;
        k=k+2;
    }
    pthread_mutex_unlock(&bufferLock); //unlock mutex

    request[9]=25; //request the next 25 items
    buffer[8]=0; //zeroed previous response size
    send(psm, request, 12, 0);
    read(psm, buffer, 1024);

    //check if response is complete
    if (buffer[8] < 50)
    {
        sprintf(log_msg, "PSM: Error reading from analog inputs!\n");
        log(log_msg);
        error_count++;
        check_error_count();
    }

    pthread_mutex_lock(&bufferLock); //lock mutex
    k = 9; //start at the beginning of data
    for (int i=25; i<50; i++)
    {
        if (int_input[i] != NULL) *int_input[i] = buffer[k+1] | (uint16_t)buffer[k] << 8;
        k=k+2;
    }
    pthread_mutex_unlock(&bufferLock); //unlock mutex
}

//-----------------------------------------------------------------------------
// Write analog outputs to PSM through Modbus Write Holding Register function
//-----------------------------------------------------------------------------
void write_ana_out(int psm)
{
    char log_msg[1000];
    //                           tid     pid     len  uid  fc  start   count  len
    unsigned char request[] = {00, 01, 00, 00, 00, 57, 00, 16, 00, 00, 00, 25, 50, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00};
    unsigned char buffer[1024];
    
    pthread_mutex_lock(&bufferLock); //lock mutex
    int k = 13;
    for (int i = 0; i < 25; i ++)
    {
        if (int_output[i] != NULL)
        {
            request[k] = (unsigned char)(*int_output[i] >> 8);
            k++;
            request[k] = (unsigned char)(*int_output[i]);
            k++;
        }
    }
    pthread_mutex_unlock(&bufferLock); //unlock mutex
    
    send(psm, request, 63, 0);
    read(psm, buffer, 1024);
    
    //check if response is complete
    if (buffer[11] < 25)
    {
        sprintf(log_msg, "PSM: Error writing to analog outputs!\n");
        log(log_msg);
        error_count++;
        check_error_count();
    }
    
    pthread_mutex_lock(&bufferLock); //lock mutex
    //write next 25 items
    request[9] = 25;
    k = 13;
    for (int i = 25; i < 50; i ++)
    {
        if (int_output[i] != NULL)
        {
            request[k] = (unsigned char)(*int_output[i] >> 8);
            k++;
            request[k] = (unsigned char)(*int_output[i]);
            k++;
        }
    }
    pthread_mutex_unlock(&bufferLock); //unlock mutex
    
    send(psm, request, 63, 0);
    read(psm, buffer, 1024);
    
    //check if response is complete
    if (buffer[11] < 25)
    {
        sprintf(log_msg, "PSM: Error writing to analog outputs!\n");
        log(log_msg);
        error_count++;
        check_error_count();
    }
}

//-----------------------------------------------------------------------------
// Read digital inputs from PSM through Modbus Read Input Status function
//-----------------------------------------------------------------------------
void read_dig_inp(int psm)
{
    char log_msg[1000];
    //                          tid     pid     len   uid  fc  start   count
    unsigned char request[] = {00, 01, 00, 00, 00, 06, 00, 02, 00, 00, 0x01, 0x90}; //reading 400 coils count = 0x0190
    unsigned char buffer[1024];
    send(psm, request, 12, 0);
    read(psm, buffer, 1024);
    
    //check if response is complete
    if (buffer[8] < 50)
    {
        sprintf(log_msg, "PSM: Error reading from digital inputs!\n");
        log(log_msg);
        error_count++;
        check_error_count();
    }
        
    
    //format of bits on response
    //byte 1 => coils 7 to 0
    //byte 2 => coils 15 to 8
    //byte 3 => coils 23 to 16
    
    //read inputs into buffer
    pthread_mutex_lock(&bufferLock); //lock mutex
    int a = 0;
    for (int i = 9; i < 59; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (bool_input[a/8][a%8] != NULL) *bool_input[a/8][a%8] = bitRead(buffer[i], j);
            a++;
        }
    }
    pthread_mutex_unlock(&bufferLock); //unlock mutex
}

//-----------------------------------------------------------------------------
// Write digital outputs from PSM through Modbus Write Coils function
//-----------------------------------------------------------------------------
void write_dig_out(int psm)
{
    char log_msg[1000];
    //                          tid     pid     len   uid  fc  start   count (400 coils - 50 bytes)
    unsigned char request[] = {00, 01, 00, 00, 00, 57, 00, 15, 00, 00, 0x01, 0x90, 50, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00};
    unsigned char buffer[1024];
    
    //write all coils from buffer to request
    pthread_mutex_lock(&bufferLock); //lock mutex
    for (int i = 0; i < 50; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (bool_output[i][j] != NULL)
            {
                bitWrite(request[(i+13)], j, *bool_output[i][j]);
            }
        }
    }
    pthread_mutex_unlock(&bufferLock); //unlock mutex
    
    send(psm, request, 63, 0);
    read(psm, buffer, 1024);
    
    //check if response is complete
    if (buffer[10] != 0x01 && buffer[11] != 0x90)
    {
        sprintf(log_msg, "PSM: Error writing to digital outputs!\n");
        log(log_msg);
        error_count++;
        check_error_count();
    }
}

//-----------------------------------------------------------------------------
// Write a special stop signal to PSM register signaling it to quit
//-----------------------------------------------------------------------------
void stop_psm(int psm)
{
    char log_msg[1000];
    //                          tid     pid     len   uid  fc  address value
    unsigned char request[] = {00, 01, 00, 00, 00, 06, 00, 06, 00, 50, 00, 127};
    unsigned char buffer[1024];

    sprintf(log_msg, "PSM: Stopping PSM...\n");
    log(log_msg);
    
    send(psm, request, 12, 0);
    /*
    size_t rsp_size = read(psm, buffer, 1024);
    
    //check response
    if (buffer[11] != 127)
    {
        sprintf(log_msg, "PSM: Error while trying to stop PSM!\n");
        log(log_msg);
    }
    */
}

//-----------------------------------------------------------------------------
// Kill all PSM modules running on the background
//-----------------------------------------------------------------------------
void kill_psm()
{
    char log_msg[BUFFER_LIMIT];
    sprintf(log_msg, "PSM: Killing previous PSM modules...\n");
    log(log_msg);
    char *cmd = "ps aux | grep ./core/psm/main.py | awk '{print $2}'";
    
    FILE *psm_proc;
    if ((psm_proc = popen(cmd, "r")) == NULL)
    {
        sprintf(log_msg, "PSM: Error while trying to kill background PSM.\n Could not open pipe!\n");
        log(log_msg);
        return;
    }
    
    while (fgets(log_msg, BUFFER_LIMIT, psm_proc) != NULL)
    {
        //grab each PSM PID and kill it
        char proc_pid[100];
        char kill_command[1024];
        int i = 0;
        int j = 0;
        for (i = 0; log_msg[i] != '\0'; i++)
        {
            if (log_msg[i] != '\n')
            {
                proc_pid[j] = log_msg[i];
                j++;
                proc_pid[j] = '\0';
            }
            else
            {
                sprintf(kill_command, "kill -9 %d", atoi(proc_pid));
                system(kill_command);
                j = 0;
            }
        }
    }
    
    if (pclose(psm_proc))
    {
        sprintf(log_msg, "PSM: Error while trying to kill background PSM\n");
        log(log_msg);
        return;
    }
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
    //Verify if there is any old PSM running on the background and kill it
    if ((psm = connect_to_psm(0)) >= 0)
    {
        printf("PSM found..killing it!\n");
        close(psm);
        kill_psm();
        sleepms(500);
    }
    
    //Start PSM thread
    pthread_t psm_thread;
    int ret = -1;
    ret = pthread_create(&psm_thread, NULL, start_psm, NULL);
    if (ret == 0)
    {
        pthread_detach(psm_thread);
    }
    sleepms(2000);
    
    char log_msg[1000];
    psm = connect_to_psm(1);
    if (psm < 0)
    {
        sprintf(log_msg, "PSM: Error connecting to psm!\nPSM: PSM is disabled\n");
        log(log_msg);
    }
    else
    {
        sprintf(log_msg, "PSM: Connected to PSM\n");
        log(log_msg);
    }
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is finalizing.
// Resource clearing procedures should be here.
//-----------------------------------------------------------------------------
void finalizeHardware()
{
    stop_psm(psm);
    close(psm);
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Input state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersIn()
{
    if (psm >= 0 && error_count < ERROR_LIMIT)
    {
        read_dig_inp(psm);
        read_ana_inp(psm);
    }
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Output state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
    if (psm >= 0 && error_count < ERROR_LIMIT)
    {
        write_dig_out(psm);
        write_ana_out(psm);
    }
}

