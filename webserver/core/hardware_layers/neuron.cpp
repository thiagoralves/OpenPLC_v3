//-----------------------------------------------------------------------------
// Copyright 2018 Thiago Alves
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
// Thiago Alves, Oct 2018
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>

#include "ladder.h"
#include "custom_layer.h"

#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

char digital_inputs[1000][200];
char digital_outputs[1000][200];
char analog_inputs[1000][200];
char analog_outputs[1000][200];


//-----------------------------------------------------------------------------
// This function is responsible for making I/O requests using SYSFS
//-----------------------------------------------------------------------------
int requestSYSFS(char *path, char *command)
{
    int fd, len;
    char buf[100];
    
    if (!strncmp(command, "read", 4))
    {
        fd = open(path, O_RDONLY);
        
        if (fd < 0)
        {
            close(fd);
            return -1;
        }
        
        read(fd, buf, 100);
        close(fd);
        return atoi(buf);
    }
    
    else if (!strncmp(command, "write", 5))
    {
        fd = open(path, O_WRONLY);
        
        if (fd < 0)
        {
            return -1;
        }
        
        int a = 6, b = 0;
        while (command[a] != '\0')
        {
            buf[b] = command[a];
            a++;
            b++;
            buf[b] = '\0';
        }
        
        write(fd, buf, b);
        close(fd);
    }
}

//------------------------------------------------------------------------------------------------------------
// Look for all available I/Os connected to Neuron. Scan from 1_01 to 10_20. With specific scan for user leds 
//------------------------------------------------------------------------------------------------------------
void searchForIO()
{
    char path[200];
    char path_fmt[200];
    char log_msg[1000];
    
    sprintf(log_msg, "Neuron: Searching for I/O...\n");
    log(log_msg);
    
    /* look for digital inputs */
    strcpy(path_fmt, "/sys/devices/platform/unipi_plc/io_group%d/di_%d_%02d/di_value");
    int index = 0;
    for (int group = 1; group < 10; group++)
    {
        for (int major = 1; major < 10; major++)
        {
            for (int minor = 1; minor < 20; minor++)
            {
                sprintf(path, path_fmt, group, major, minor);
                char *command = "read";
                if (requestSYSFS(path, command) >= 0)
                {
                    /* valid I/O. Add to the list */
                    strcpy(digital_inputs[index], path);                
                    index++;
                    digital_inputs[index][0] = '\0';
                }
            }
        }
    }

    /* look for digital outputs */
    strcpy(path_fmt, "/sys/devices/platform/unipi_plc/io_group%d/do_%d_%02d/do_value");
    index = 0;
    for (int group = 1; group < 10; group++)
    {
        for (int major = 1; major < 10; major++)
        {
            for (int minor = 1; minor < 20; minor++)
            {
                sprintf(path, path_fmt, group, major, minor);
                char *command = "read";
                if (requestSYSFS(path, command) >= 0)
                {
                    /* valid I/O. Add to the list */
                    strcpy(digital_outputs[index], path);                
                    index++;
                    digital_outputs[index][0] = '\0';
                }
            }
        }
    }

    /* look for digital outputs (relay) */
    strcpy(path_fmt, "/sys/devices/platform/unipi_plc/io_group%d/ro_%d_%02d/ro_value");
    for (int group = 1; group < 10; group++)
    {
        for (int major = 1; major < 10; major++)
        {
            for (int minor = 1; minor < 20; minor++)
            {
                sprintf(path, path_fmt, group, major, minor);
                char *command = "read";
                if (requestSYSFS(path, command) >= 0)
                {
                    /* valid I/O. Add to the list */
                    strcpy(digital_outputs[index], path);                
                    index++;
                    digital_outputs[index][0] = '\0';
                }
            }
        }

    }

    /* look for digital outputs (user leds) */
    strcpy(path_fmt, "/sys/devices/platform/unipi_plc/io_group%d/leds/unipi:green:uled-x%d/brightness");
    for (int group = 1; group < 10; group++)
    {
        for (int major = 0; major < 10; major++)
        {
            sprintf(path, path_fmt, group, major);
            char *command = "read";
            if (requestSYSFS(path, command) >= 0)
            {
                /* valid I/O. Add to the list */
                strcpy(digital_outputs[index], path);                
                index++;
                digital_outputs[index][0] = '\0';
            }
        }
    }

    /* look for analog inputs */
    strcpy(path_fmt, "/sys/devices/platform/unipi_plc/io_group%d/ai_%d_%d/in_voltage0_raw");
    index = 0;
    for (int group = 1; group < 10; group++)
    {
        for (int major = 1; major < 10; major++)
        {
            for (int minor = 1; minor < 20; minor++)
            {
                sprintf(path, path_fmt, group, major, minor);
                char *command = "read";
                if (requestSYSFS(path, command) >= 0)
                {
                    /* valid I/O. Add to the list */
                    strcpy(analog_inputs[index], path);                
                    index++;
                    analog_inputs[index][0] = '\0';
                }
            }
        }
    }
    
    /* look for analog outputs */
    strcpy(path_fmt, "/sys/devices/platform/unipi_plc/io_group%d/ao_%d_%d/out_voltage0_raw");
    index = 0;
    for (int group = 1; group < 10; group++)
    {
        for (int major = 1; major < 10; major++)
        {
            for (int minor = 1; minor < 20; minor++)
            {
                sprintf(path, path_fmt, group, major, minor);
                char *command = "read";
                if (requestSYSFS(path, command) >= 0)
                {
                    /* valid I/O. Add to the list */
                    strcpy(analog_outputs[index], path);                
                    index++;
                    analog_outputs[index][0] = '\0';
                }
            }
        }
    }
    
    /* print found I/Os on console log */
    sprintf(log_msg, "Neuron: Done!\n\nNeuron Digital Inputs\n");
    log(log_msg);
    index = 0;
    while (digital_inputs[index][0] != '\0')
    {
        sprintf(log_msg, "%s\t=>\t%%IX%d.%d\n", digital_inputs[index], (index/8), (index%8));
        log(log_msg);
        index++;
    }
    
    sprintf(log_msg, "\nNeuron Digital Outputs\n");
    log(log_msg);
    index = 0;
    while (digital_outputs[index][0] != '\0')
    {
        sprintf(log_msg, "%s\t=>\t%%QX%d.%d\n", digital_outputs[index], (index/8), (index%8));
        log(log_msg);
        index++;
    }
    
    sprintf(log_msg, "\nNeuron Analog Inputs\n");
    log(log_msg);
    index = 0;
    while (analog_inputs[index][0] != '\0')
    {
        sprintf(log_msg, "%s\t=>\t%%IW%d\n", analog_inputs[index], index);
        log(log_msg);
        index++;
    }
    
    sprintf(log_msg, "\nNeuron Analog Outputs\n");
    log(log_msg);
    index = 0;
    while (analog_outputs[index][0] != '\0')
    {
        //check if this is the last message
        if (analog_outputs[index+1][0] == '\0')
            sprintf(log_msg, "%s\t=>\t%%QW%d\n\n", analog_outputs[index], index);
            
        else
            sprintf(log_msg, "%s\t=>\t%%QW%d\n", analog_outputs[index], index);
        
        log(log_msg);
        index++;
    }
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
    searchForIO();
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is finalizing.
// Resource clearing procedures should be here.
//-----------------------------------------------------------------------------
void finalizeHardware()
{
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Input state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersIn()
{
	pthread_mutex_lock(&bufferLock); //lock mutex
    
    /* read digital inputs */
    int i = 0;
    while (digital_inputs[i][0] != '\0')
    {   
        if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
            if (bool_input[i/8][i%8] != NULL) *bool_input[i/8][i%8] = requestSYSFS(digital_inputs[i], "read");
        
        i++;
    }
    
    /* read analog inputs */
    i = 0;
    while (analog_inputs[i][0] != '\0')
    {
        if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
            if (int_input[i] != NULL)
            {
                uint32_t value = (uint32_t)((float)requestSYSFS(analog_inputs[i], "read") * 6.5535);
                if (value > 65535) value = 65535;
                *int_input[i] = (uint16_t)value;
            }
        
        i++;
    }

	pthread_mutex_unlock(&bufferLock); //unlock mutex
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Output state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
	pthread_mutex_lock(&bufferLock); //lock mutex
    
    /* write digital outputs */
    int i = 0;
    while (digital_outputs[i][0] != '\0')
    {
        if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
        {
            if (bool_output[i/8][i%8] != NULL) 
            {
                if (*bool_output[i/8][i%8])
                    requestSYSFS(digital_outputs[i], "write=1");
                else
                    requestSYSFS(digital_outputs[i], "write=0");
            }
        }
        i++;
    }

    /* write analog outputs */
    i = 0;
    while (analog_outputs[i][0] != '\0')
    {
        if (pinNotPresent(ignored_int_outputs, ARRAY_SIZE(ignored_int_outputs), i))
        {
            if (int_output[i] != NULL) 
            {
                char value_fmt[100];
                char value[100];
                strcpy(value_fmt, "write=%f");
                sprintf(value, value_fmt, ((float)*int_output[i]/6.5535));
                requestSYSFS(analog_outputs[i], value);
            }
        }
        i++;
    }
    
	pthread_mutex_unlock(&bufferLock); //unlock mutex
    
}
