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
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "ladder.h"
#include "custom_layer.h"

#define MAX_SIZE 1024

// Structure for message queue
static struct msg_buffer {
    long msg_type;
    char msg_text[MAX_SIZE];
} message;

// I/O queue
static int input_qid, output_qid;

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
    key_t key;

    // ftok to generate unique key and returns identifier
    key = ftok("input_q", 65);

    // msgget creates a message queue and returns identifier
    input_qid = msgget(key, 0666 | IPC_CREAT);

    // ftok to generate unique key and returns identifier
    key = ftok("output_q", 65);

    // msgget creates a message queue and returns identifier
    output_qid = msgget(key, 0666 | IPC_CREAT);
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Input state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersIn()
{
	pthread_mutex_lock(&bufferLock); //lock mutex

	/*********READING AND WRITING TO I/O**************

	*bool_input[0][0] = read_digital_input(0);
	write_digital_output(0, *bool_output[0][0]);

	*int_input[0] = read_analog_input(0);
	write_analog_output(0, *int_output[0]);

	**************************************************/
        
        // msgrcv to receive message
        msgrcv(input_qid, &message, sizeof(message), 1, 0);

        // Display the message
        printf("Data received: %s\n", message.msg_text);

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

	/*********READING AND WRITING TO I/O**************

	*bool_input[0][0] = read_digital_input(0);
	write_digital_output(0, *bool_output[0][0]);

	*int_input[0] = read_analog_input(0);
	write_analog_output(0, *int_output[0]);

	**************************************************/

	pthread_mutex_unlock(&bufferLock); //unlock mutex
}

