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
// This file is responsible for parse and discovery of slave devices by parsing
// the mbconfig.cfg file. This code also updates OpenPLC internal buffers with
// the data queried from the slave devices.
// Thiago Alves, Jul 2018
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <modbus.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include <iostream>
#include <fstream>
#include <string>

#include "ladder.h"

#define MB_TCP                1
#define MB_RTU                2
#define MAX_MB_IO            400

using namespace std;

uint8_t bool_input_buf[MAX_MB_IO];
uint8_t bool_output_buf[MAX_MB_IO];
uint16_t int_input_buf[MAX_MB_IO];
uint16_t int_output_buf[MAX_MB_IO];
uint8_t rpi_modbus_rts_pin;     // If <> 0, expect hardware RTS to be used with this pin

pthread_mutex_t ioLock;

struct MB_address
{
    uint16_t start_address;
    uint16_t num_regs;
};

struct MB_device
{
    modbus_t *mb_ctx;
    char dev_name[100];
    uint8_t protocol;
    char dev_address[100];
    uint16_t ip_port;
    int rtu_baud;
    char rtu_parity;
    int rtu_data_bit;
    int rtu_stop_bit;
    int rtu_tx_pause;
    uint8_t dev_id;
    bool isConnected;

    struct MB_address discrete_inputs;
    struct MB_address coils;
    struct MB_address input_registers;
    struct MB_address holding_read_registers;
    struct MB_address holding_registers;
};

struct MB_device *mb_devices;
uint8_t num_devices;
uint16_t polling_period = 100;
uint16_t timeout = 1000;

//-----------------------------------------------------------------------------
// Finds the data between the separators on the line provided
//-----------------------------------------------------------------------------
void getData(char *line, char *buf, char separator1, char separator2)
{
    int i=0, j=0;
    buf[j] = '\0';

    while (line[i] != separator1 && line[i] != '\0')
    {
        i++;
    }
    i++;

    while (line[i] != separator2 && line[i] != '\0')
    {
        buf[j] = line[i];
        i++;
        j++;
        buf[j] = '\0';
    }
}

//-----------------------------------------------------------------------------
// Get the number of the Modbus device
//-----------------------------------------------------------------------------
int getDeviceNumber(char *line)
{
    char temp[5];
    int i = 0, j = 6;

    while (line[j] != '.')
    {
        temp[i] = line[j];
        i++;
        j++;
        temp[i] = '\0';
    }

    return(atoi(temp));
}

//-----------------------------------------------------------------------------
// get the type of function or parameter for the Modbus device
//-----------------------------------------------------------------------------
void getFunction(char *line, char *parameter)
{
    int i = 0, j = 0;

    while (line[j] != '.')
    {
        j++;
    }
    j++;

    while (line[j] != ' ' && line[j] != '=' && line[j] != '(')
    {
        parameter[i] = line[j];
        i++;
        j++;
        parameter[i] = '\0';
    }
}

void parseConfig()
{
    string line;
    char line_str[1024];
    ifstream cfgfile("mbconfig.cfg");

    if (cfgfile.is_open())
    {
        while (getline(cfgfile, line))
        {
            strncpy(line_str, line.c_str(), 1024);
            if (line_str[0] != '#' && strlen(line_str) > 1)
            {
                if (!strncmp(line_str, "Num_Devices", 11))
                {
                    char temp_buffer[5];
                    getData(line_str, temp_buffer, '"', '"');
                    num_devices = atoi(temp_buffer);
                    //initializes the allocated memory to zero
                    mb_devices = calloc(num_devices, sizeof(struct MB_device));
                }
                else if (!strncmp(line_str, "Polling_Period", 14))
                {
                    char temp_buffer[10];
                    getData(line_str, temp_buffer, '"', '"');
                    polling_period = atoi(temp_buffer);
                }
                else if (!strncmp(line_str, "Timeout", 7))
                {
                    char temp_buffer[10];
                    getData(line_str, temp_buffer, '"', '"');
                    timeout = atoi(temp_buffer);
                }

                else if (!strncmp(line_str, "device", 6))
                {
                    int deviceNumber = getDeviceNumber(line_str);
                    char functionType[100];
                    getFunction(line_str, functionType);

                    if (!strncmp(functionType, "name", 4))
                    {
                        getData(line_str, mb_devices[deviceNumber].dev_name, '"', '"');
                    }
                    else if (!strncmp(functionType, "protocol", 8))
                    {
                        char temp_buffer[5];
                        getData(line_str, temp_buffer, '"', '"');

                        if (!strncmp(temp_buffer, "TCP", 3))
                            mb_devices[deviceNumber].protocol = MB_TCP;
                        else if (!strncmp(temp_buffer, "RTU", 3))
                            mb_devices[deviceNumber].protocol = MB_RTU;
                    }
                    else if (!strncmp(functionType, "slave_id", 8))
                    {
                        char temp_buffer[5];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].dev_id = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "address", 7))
                    {
                        getData(line_str, mb_devices[deviceNumber].dev_address, '"', '"');
                    }
                    else if (!strncmp(functionType, "IP_Port", 7))
                    {
                        char temp_buffer[6];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].ip_port = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "RTU_Baud_Rate", 13))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].rtu_baud = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "RTU_Parity", 10))
                    {
                        char temp_buffer[3];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].rtu_parity = temp_buffer[0];
                    }
                    else if (!strncmp(functionType, "RTU_Data_Bits", 13))
                    {
                        char temp_buffer[6];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].rtu_data_bit = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "RTU_Stop_Bits", 13))
                    {
                        char temp_buffer[20];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].rtu_stop_bit = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "RTU_TX_Pause", 12))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].rtu_tx_pause = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "Discrete_Inputs_Start", 21))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].discrete_inputs.start_address = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "Discrete_Inputs_Size", 20))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].discrete_inputs.num_regs = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "Coils_Start", 11))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].coils.start_address = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "Coils_Size", 10))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].coils.num_regs = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "Input_Registers_Start", 21))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].input_registers.start_address = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "Input_Registers_Size", 20))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].input_registers.num_regs = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "Holding_Registers_Read_Start", 28))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].holding_read_registers.start_address = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "Holding_Registers_Read_Size", 27))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].holding_read_registers.num_regs = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "Holding_Registers_Start", 23))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].holding_registers.start_address = atoi(temp_buffer);
                    }
                    else if (!strncmp(functionType, "Holding_Registers_Size", 22))
                    {
                        char temp_buffer[10];
                        getData(line_str, temp_buffer, '"', '"');
                        mb_devices[deviceNumber].holding_registers.num_regs = atoi(temp_buffer);
                    }
                }
            }
        }
    }
    else
    {
        char log_msg[1000];
        sprintf(log_msg, "Skipping configuration of Slave Devices (mbconfig.cfg file not found)\n");
        log(log_msg);
    }

    //Parser Debug
    ///*
    for (int i = 0; i < num_devices; i++)
    {
        printf("Device %d\n", i);
        printf("Name: %s\n", mb_devices[i].dev_name);
        printf("Protocol: %d\n", mb_devices[i].protocol);
        printf("Address: %s\n", mb_devices[i].dev_address);
        printf("IP Port: %d\n", mb_devices[i].ip_port);
        printf("Baud rate: %d\n", mb_devices[i].rtu_baud);
        printf("Parity: %c\n", mb_devices[i].rtu_parity);
        printf("Data Bits: %d\n", mb_devices[i].rtu_data_bit);
        printf("Stop Bits: %d\n", mb_devices[i].rtu_stop_bit);
        printf("DI Start: %d\n", mb_devices[i].discrete_inputs.start_address);
        printf("DI Size: %d\n", mb_devices[i].discrete_inputs.num_regs);
        printf("Coils Start: %d\n", mb_devices[i].coils.start_address);
        printf("Coils Size: %d\n", mb_devices[i].coils.num_regs);
        printf("IR Start: %d\n", mb_devices[i].input_registers.start_address);
        printf("IR Size: %d\n", mb_devices[i].input_registers.num_regs);
        printf("HR Start: %d\n", mb_devices[i].holding_registers.start_address);
        printf("HR Size: %d\n", mb_devices[i].holding_registers.num_regs);
        printf("\n\n");
    }
    //*/
}


//-----------------------------------------------------------------------------
// Thread to poll each slave device
//-----------------------------------------------------------------------------
void *querySlaveDevices(void *arg)
{
    while (run_openplc)
    {
        char log_msg[1000];
        
        uint16_t bool_input_index = 0;
        uint16_t bool_output_index = 0;
        uint16_t int_input_index = 0;
        uint16_t int_output_index = 0;

        for (int i = 0; i < num_devices; i++)
        {
            //Check if there is a connected RTU device using the same port
            bool found_sharing = false;
            bool rtu_port_connected = false;
            if (mb_devices[i].protocol == MB_RTU)
            {
                for (int a = 0; a < num_devices; a++)
                {
                    if (a != i && !strcmp(mb_devices[i].dev_address, mb_devices[a].dev_address))
                    {
                        found_sharing = true;
                        if (mb_devices[a].isConnected)
                        {
                            rtu_port_connected = true;
                        }
                    }
                }
                if (found_sharing)
                {
                    //Must reset mb context to current device's slave id
                    modbus_set_slave(mb_devices[i].mb_ctx, mb_devices[i].dev_id);
                }
            }

            //Verify if device is connected
            if (!mb_devices[i].isConnected && !rtu_port_connected)
            {
                sprintf(log_msg, "Device %s is disconnected. Attempting to reconnect...\n", mb_devices[i].dev_name);
                log(log_msg);
                if (modbus_connect(mb_devices[i].mb_ctx) == -1)
                {

                    sprintf(log_msg, "Connection failed on MB device %s: %s\n", mb_devices[i].dev_name, modbus_strerror(errno));
                    log(log_msg);
                    
                    if (special_functions[2] != NULL) *special_functions[2]++;
                    
                    // Because this device is not connected, we skip those input registers
                    bool_input_index += (mb_devices[i].discrete_inputs.num_regs);
                    int_input_index += (mb_devices[i].input_registers.num_regs);
                    int_input_index += (mb_devices[i].holding_read_registers.num_regs);
                    bool_output_index += (mb_devices[i].coils.num_regs);
                    int_output_index += (mb_devices[i].holding_registers.num_regs);
                }
                else
                {
                    sprintf(log_msg, "Connected to MB device %s\n", mb_devices[i].dev_name);
                    log(log_msg);
                    mb_devices[i].isConnected = true;
                }
            }
            if (mb_devices[i].isConnected || rtu_port_connected)
            {

                struct timespec ts;
                ts.tv_sec = 0;
                if (mb_devices[i].protocol == MB_RTU)
                {
                    ts.tv_nsec = (1000*1000*1000*28)/mb_devices[i].rtu_baud;
                }
                else
                {
                    ts.tv_nsec = 0;                    
                }

                //Read discrete inputs
                if (mb_devices[i].discrete_inputs.num_regs != 0)
                {
                    sleepms(mb_devices[i].rtu_tx_pause);
                    uint8_t *tempBuff;
                    tempBuff = (uint8_t *)malloc(mb_devices[i].discrete_inputs.num_regs);
                    nanosleep(&ts, NULL); 
                    int return_val = modbus_read_input_bits(mb_devices[i].mb_ctx, mb_devices[i].discrete_inputs.start_address,
                                                            mb_devices[i].discrete_inputs.num_regs, tempBuff);
                    if (return_val == -1)
                    {
                        if (mb_devices[i].protocol != MB_RTU)
                        {
                            modbus_close(mb_devices[i].mb_ctx);
                            mb_devices[i].isConnected = false;
                        }
                        
                        sprintf(log_msg, "Modbus Read Discrete Input Registers failed on MB device %s: %s\n", mb_devices[i].dev_name, modbus_strerror(errno));
                        log(log_msg);
                        bool_input_index += (mb_devices[i].discrete_inputs.num_regs);
                        if (special_functions[2] != NULL) *special_functions[2]++;
                    }
                    else
                    {
                        pthread_mutex_lock(&ioLock);
                        for (int j = 0; j < return_val; j++)
                        {
                            bool_input_buf[bool_input_index] = tempBuff[j];
                            bool_input_index++;
                        }
                        pthread_mutex_unlock(&ioLock);
                    }

                    free(tempBuff);
                }

                //Write coils
                if (mb_devices[i].coils.num_regs != 0)
                {
                    sleepms(mb_devices[i].rtu_tx_pause);
                    uint8_t *tempBuff;
                    tempBuff = (uint8_t *)malloc(mb_devices[i].coils.num_regs);

                    pthread_mutex_lock(&ioLock);
                    for (int j = 0; j < mb_devices[i].coils.num_regs; j++)
                    {
                        tempBuff[j] = bool_output_buf[bool_output_index];
                        bool_output_index++;
                    }
                    pthread_mutex_unlock(&ioLock);

                    nanosleep(&ts, NULL); 
                    int return_val = modbus_write_bits(mb_devices[i].mb_ctx, mb_devices[i].coils.start_address, mb_devices[i].coils.num_regs, tempBuff);
                    if (return_val == -1)
                    {
                        if (mb_devices[i].protocol != MB_RTU)
                        {
                            modbus_close(mb_devices[i].mb_ctx);
                            mb_devices[i].isConnected = false;
                        }

                        sprintf(log_msg, "Modbus Write Coils failed on MB device %s: %s\n", mb_devices[i].dev_name, modbus_strerror(errno));
                        log(log_msg);
                        if (special_functions[2] != NULL) *special_functions[2]++;
                    }
                    
                    free(tempBuff);
                }

                //Read input registers
                if (mb_devices[i].input_registers.num_regs != 0)
                {
                    sleepms(mb_devices[i].rtu_tx_pause);
                    uint16_t *tempBuff;
                    tempBuff = (uint16_t *)malloc(2*mb_devices[i].input_registers.num_regs);
                    nanosleep(&ts, NULL); 
                    int return_val = modbus_read_input_registers(    mb_devices[i].mb_ctx, mb_devices[i].input_registers.start_address,
                                                                    mb_devices[i].input_registers.num_regs, tempBuff);
                    if (return_val == -1)
                    {
                        if (mb_devices[i].protocol != MB_RTU)
                        {
                            modbus_close(mb_devices[i].mb_ctx);
                            mb_devices[i].isConnected = false;
                        }
                        
                        sprintf(log_msg, "Modbus Read Input Registers failed on MB device %s: %s\n", mb_devices[i].dev_name, modbus_strerror(errno));
                        log(log_msg);
                        int_input_index += (mb_devices[i].input_registers.num_regs);
                        if (special_functions[2] != NULL) *special_functions[2]++;
                    }
                    else
                    {
                        pthread_mutex_lock(&ioLock);
                        for (int j = 0; j < return_val; j++)
                        {
                            int_input_buf[int_input_index] = tempBuff[j];
                            int_input_index++;
                        }
                        pthread_mutex_unlock(&ioLock);
                    }

                    free(tempBuff);
                }

                //Read holding registers
                if (mb_devices[i].holding_read_registers.num_regs != 0)
                {
                    sleepms(mb_devices[i].rtu_tx_pause);
                    uint16_t *tempBuff;
                    tempBuff = (uint16_t *)malloc(2*mb_devices[i].holding_read_registers.num_regs);
                    nanosleep(&ts, NULL); 
                    int return_val = modbus_read_registers(mb_devices[i].mb_ctx, mb_devices[i].holding_read_registers.start_address,
                                                           mb_devices[i].holding_read_registers.num_regs, tempBuff);
                    if (return_val == -1)
                    {
                        if (mb_devices[i].protocol != MB_RTU)
                        {
                            modbus_close(mb_devices[i].mb_ctx);
                            mb_devices[i].isConnected = false;
                        }
                        sprintf(log_msg, "Modbus Read Holding Registers failed on MB device %s: %s\n", mb_devices[i].dev_name, modbus_strerror(errno));
                        log(log_msg);
                        int_input_index += (mb_devices[i].holding_read_registers.num_regs);
                        if (special_functions[2] != NULL) *special_functions[2]++;
                    }
                    else
                    {
                        pthread_mutex_lock(&ioLock);
                        for (int j = 0; j < return_val; j++)
                        {
                            int_input_buf[int_input_index] = tempBuff[j];
                            int_input_index++;
                        }
                        pthread_mutex_unlock(&ioLock);
                    }

                    free(tempBuff);
                }

                //Write holding registers
                if (mb_devices[i].holding_registers.num_regs != 0)
                {
                    sleepms(mb_devices[i].rtu_tx_pause);
                    uint16_t *tempBuff;
                    tempBuff = (uint16_t *)malloc(2*mb_devices[i].holding_registers.num_regs);

                    pthread_mutex_lock(&ioLock);
                    for (int j = 0; j < mb_devices[i].holding_registers.num_regs; j++)
                    {
                        tempBuff[j] = int_output_buf[int_output_index];
                        int_output_index++;
                    }
                    pthread_mutex_unlock(&ioLock);

                    nanosleep(&ts, NULL); 
                    int return_val = modbus_write_registers(mb_devices[i].mb_ctx, mb_devices[i].holding_registers.start_address,
                                                            mb_devices[i].holding_registers.num_regs, tempBuff);
                    if (return_val == -1)
                    {
                        if (mb_devices[i].protocol != MB_RTU)
                        {
                            modbus_close(mb_devices[i].mb_ctx);
                            mb_devices[i].isConnected = false;
                        }
                        
                        sprintf(log_msg, "Modbus Write Holding Registers failed on MB device %s: %s\n", mb_devices[i].dev_name, modbus_strerror(errno));
                        log(log_msg);
                        if (special_functions[2] != NULL) *special_functions[2]++;
                    }
                    
                    free(tempBuff);
                }
            }
        }
        sleepms(polling_period);
    }
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Modbus master initialization procedures are here.
//-----------------------------------------------------------------------------
void initializeMB()
{
    parseConfig();

    for (int i = 0; i < num_devices; i++)
    {
        if (mb_devices[i].protocol == MB_TCP)
        {
            mb_devices[i].mb_ctx = modbus_new_tcp(mb_devices[i].dev_address, mb_devices[i].ip_port);
        }
        else if (mb_devices[i].protocol == MB_RTU)
        {
            //Check if there is a device using the same port
            int share_index = -1;
            for (int a = 0; a < num_devices && a < i; a++)
            {
                if (strcmp(mb_devices[i].dev_address, mb_devices[a].dev_address) == 0)
                {
                    share_index = a;
                    break;
                }
            }
            if (share_index != -1)
            {
                if (mb_devices[i].rtu_baud != mb_devices[share_index].rtu_baud || mb_devices[i].rtu_parity != mb_devices[share_index].rtu_parity || 
                    mb_devices[i].rtu_data_bit != mb_devices[share_index].rtu_data_bit || mb_devices[i].rtu_stop_bit != mb_devices[share_index].rtu_stop_bit)
                {
                    char log_msg[1000];
                    sprintf(log_msg, "Warning MB device %s port setting missmatch\n", mb_devices[i].dev_name);
                    log(log_msg);
                }
                mb_devices[i].mb_ctx = mb_devices[share_index].mb_ctx;
            }
            else
            {
                mb_devices[i].mb_ctx = modbus_new_rtu(mb_devices[i].dev_address, mb_devices[i].rtu_baud,
                                                mb_devices[i].rtu_parity, mb_devices[i].rtu_data_bit,
                                                mb_devices[i].rtu_stop_bit);

                // If hardware layer set modbus_rts_pin, enable Pi specific rts handling handling
                if (rpi_modbus_rts_pin != 0) {
                    modbus_enable_rpi(mb_devices[i].mb_ctx,TRUE);
                    modbus_configure_rpi_bcm_pin(mb_devices[i].mb_ctx,rpi_modbus_rts_pin);
                    modbus_rpi_pin_export_direction(mb_devices[i].mb_ctx);
                }
            }
        }
        
        //slave id

        modbus_set_slave(mb_devices[i].mb_ctx, mb_devices[i].dev_id);


        //timeout
        uint32_t to_sec = timeout / 1000;
        uint32_t to_usec = (timeout % 1000) * 1000;
        modbus_set_response_timeout(mb_devices[i].mb_ctx, to_sec, to_usec);
        
    }
    
    //Initialize comm error counter
    if (special_functions[2] != NULL) *special_functions[2] = 0;
    
    if (num_devices > 0)
    {
        pthread_t thread;
        int ret = pthread_create(&thread, NULL, querySlaveDevices, NULL);
        if (ret==0) 
        {
            pthread_detach(thread);
        }
    }
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Input state.
//-----------------------------------------------------------------------------
void updateBuffersIn_MB()
{
    pthread_mutex_lock(&ioLock);

    for (int i = 0; i < MAX_MB_IO; i++)
    {
        if (bool_input[100+(i/8)][i%8] != NULL) *bool_input[100+(i/8)][i%8] = bool_input_buf[i];
        if (int_input[100+i] != NULL) *int_input[100+i] = int_input_buf[i];
    }

    pthread_mutex_unlock(&ioLock);
}


//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Output state.
//-----------------------------------------------------------------------------
void updateBuffersOut_MB()
{
    pthread_mutex_lock(&ioLock);

    for (int i = 0; i < MAX_MB_IO; i++)
    {
        if (bool_output[100+(i/8)][i%8] != NULL) bool_output_buf[i] = *bool_output[100+(i/8)][i%8];
        if (int_output[100+i] != NULL) int_output_buf[i] = *int_output[100+i];
    }

    pthread_mutex_unlock(&ioLock);
}
