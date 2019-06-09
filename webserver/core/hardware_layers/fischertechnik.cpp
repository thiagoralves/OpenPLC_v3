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
#include <wiringPi.h>
#include <wiringSerial.h>
#include <pthread.h>

#include "ladder.h"
#include "custom_layer.h"

#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

int serialFd; //serial file descriptor

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
	wiringPiSetup();
	//piHiPri(99);

	serialFd = serialOpen("/dev/ttyUSB0", 9600);
	if (serialFd < 0)
	{
		printf("Error trying to open serial port\n");
		exit(1);
	}
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is finalizing.
// Resource clearing procedures should be here.
//-----------------------------------------------------------------------------
void finalizeHardware()
{
}

void sendOutput(unsigned char *sendBytes, unsigned char *recvBytes)
{
	long serialTimer;

	/*
	//==================================================================
	//READING ANALOG VALUE EX
	//==================================================================
	serialPutchar(serialFd, 0xC5);
	serialPutchar(serialFd, sendBytes[1]);

	serialTimer = millis();
	while ((serialDataAvail(serialFd) < 3) && (millis() - serialTimer < 100)) {} //wait until timeout

	if (serialDataAvail(serialFd) == 3) //received 3 bytes correctly
	{
		recvBytes[0] = serialGetchar(serialFd); //this one will be discarded
		recvBytes[2] = serialGetchar(serialFd); //EX high
		recvBytes[3] = serialGetchar(serialFd); //EX low
	}
	else
	{
		//corrupted data - discard
		printf("corrupted1\n");
		while (serialDataAvail(serialFd) > 0)
		{
			unsigned char discard = serialGetchar(serialFd);
		}
	}

	//==================================================================
	//READING ANALOG VALUE EY
	//==================================================================
	serialPutchar(serialFd, 0xC9);
	serialPutchar(serialFd, sendBytes[1]);

	serialTimer = millis();
	while ((serialDataAvail(serialFd) < 3) && (millis() - serialTimer < 100)) {} //wait until timeout

	if (serialDataAvail(serialFd) == 3) //received 3 bytes correctly
	{
		recvBytes[0] = serialGetchar(serialFd); //this one will be discarded
		recvBytes[4] = serialGetchar(serialFd); //EY high
		recvBytes[5] = serialGetchar(serialFd); //EY low
	}
	else
	{
		//corrupted data - discard
		printf("corrupted2\n");
		while (serialDataAvail(serialFd) > 0)
		{
			unsigned char discard = serialGetchar(serialFd);
		}
	}
	*/

	//==================================================================
	//READING AND WRITING DIGITAL IO
	//==================================================================
	serialPutchar(serialFd, sendBytes[0]);
	serialPutchar(serialFd, sendBytes[1]);
	serialPutchar(serialFd, sendBytes[2]);

	serialTimer = millis();
	while ((serialDataAvail(serialFd) < 2) && (millis() - serialTimer < 100)) {} //wait until timeout

	if (serialDataAvail(serialFd) == 2) //received 2 bytes correctly
	{
		recvBytes[0] = serialGetchar(serialFd);
		recvBytes[1] = serialGetchar(serialFd);
	}

	else
	{
		//corrupted data - discard
		printf("corrupted3\n");
		while (serialDataAvail(serialFd) > 0)
		{
			unsigned char discard = serialGetchar(serialFd);
		}
	}
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Input state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersIn()
{
	unsigned char sendBytes[3], recvBytes[6], i;
	sendBytes[0] = 0xC2; //read and write IO for both modules
	sendBytes[1] = 0; //make sure output is off
	sendBytes[2] = 0; //make sure output is off

	pthread_mutex_lock(&bufferLock);
	for (i=0; i<8; i++)
	{
	    if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
		    if (bool_output[0][i] != NULL) sendBytes[1] = sendBytes[1] | (*bool_output[0][i] << i); //write each bit
	}
	for (i=8; i<16; i++)
	{
	    if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
		    if (bool_output[1][i%8] != NULL) sendBytes[2] = sendBytes[2] | (*bool_output[1][i%8] << (i-8)); //write each bit
	}
	pthread_mutex_unlock(&bufferLock);

	sendOutput(sendBytes, recvBytes);

	pthread_mutex_lock(&bufferLock);
	//if (int_input[0] != NULL) *int_input[0] = (int)(recvBytes[2] << 8) | (int)recvBytes[3]; //EX
	//if (int_input[1] != NULL) *int_input[1] = (int)(recvBytes[4] << 8) | (int)recvBytes[5]; //EY

	for (i=0; i<8; i++)
	{
	    if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
		    if (bool_input[0][i] != NULL) *bool_input[0][i] = (recvBytes[0] >> i) & 0x01;
		//printf("%d\t", DiscreteInputBuffer0[i]);
	}
	for (i=8; i<16; i++)
	{
	    if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
		    if (bool_input[1][i%8] != NULL) *bool_input[1][i%8] = (recvBytes[1] >> (i-8)) & 0x01;
		//printf("%d\t", DiscreteInputBuffer0[i]);
	}
	//printf("\n");
	pthread_mutex_unlock(&bufferLock);
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Output state. The mutex buffer_lock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
	unsigned char sendBytes[3], recvBytes[6], i;
	sendBytes[0] = 0xC2; //read and write IO for both modules
	sendBytes[1] = 0; //make sure output is off
	sendBytes[2] = 0; //make sure output is off

	pthread_mutex_lock(&bufferLock);
	for (i=0; i<8; i++)
	{
	    if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
		    if (bool_output[0][i] != NULL) sendBytes[1] = sendBytes[1] | (*bool_output[0][i] << i); //write each bit
	}
	for (i=8; i<16; i++)
	{
	    if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
		    if (bool_output[1][i%8] != NULL) sendBytes[2] = sendBytes[2] | (*bool_output[1][i%8] << (i-8)); //write each bit
	}
	pthread_mutex_unlock(&bufferLock);

	sendOutput(sendBytes, recvBytes);

	pthread_mutex_lock(&bufferLock);
	//if (int_input[0] != NULL) *int_input[0] = (int)(recvBytes[2] << 8) | (int)recvBytes[3]; //EX
	//if (int_input[1] != NULL) *int_input[1] = (int)(recvBytes[4] << 8) | (int)recvBytes[5]; //EY

	for (i=0; i<8; i++)
	{
	    if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
		    if (bool_input[0][i] != NULL) *bool_input[0][i] = (recvBytes[0] >> i) & 0x01;
		//printf("%d\t", DiscreteInputBuffer0[i]);
	}
	for (i=8; i<16; i++)
	{
	    if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
		    if (bool_input[1][i%8] != NULL) *bool_input[1][i%8] = (recvBytes[1] >> (i-8)) & 0x01;
		//printf("%d\t", DiscreteInputBuffer0[i]);
	}
	//printf("\n");
	pthread_mutex_unlock(&bufferLock);
}

