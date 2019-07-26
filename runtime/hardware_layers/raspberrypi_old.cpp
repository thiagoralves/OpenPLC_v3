// Copyright 2015 Thiago Alves
//
// Based on the LDmicro software by Jonathan Westhues
// This file is part of the OpenPLC Software Stack.
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

// This file is the hardware layer for the OpenPLC. If you change the platform
// where it is running, you may only need to change this file. All the I/O
// related stuff is here. Basically it provides functions to read and write
// to the OpenPLC internal buffers in order to update I/O state.
// Thiago Alves, Dec 2015

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <mutex>

#include "ladder.h"
#include "custom_layer.h"

#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#define MAX_INPUT 		7
#define MAX_OUTPUT 		5
#define MAX_ANALOG_OUT	1

/********************I/O PINS CONFIGURATION*********************
 * A good source for RaspberryPi I/O pins information is:
 * http://pinout.xyz
 *
 * The buffers below works as an internal mask, so that the
 * OpenPLC can access each pin sequentially
****************************************************************/
//inBufferPinMask: pin mask for each input, which
//means what pin is mapped to that OpenPLC input
int inBufferPinMask[MAX_INPUT] = { 7, 0, 2, 3, 12, 13, 14 };

//outBufferPinMask: pin mask for each output, which
//means what pin is mapped to that OpenPLC output
int outBufferPinMask[MAX_OUTPUT] = { 4, 5, 6, 10, 11 };

//analogOutBufferPinMask: pin mask for the analog PWM
//output of the RaspberryPi
int analogOutBufferPinMask[MAX_ANALOG_OUT] = { 1 };

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
	wiringPiSetup();
	//piHiPri(99);

	//set pins as input
	for (int i = 0; i < MAX_INPUT; i++)
	{
	    if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
	    {
		    pinMode(inBufferPinMask[i], INPUT);
		    if (i != 0 && i != 1) //pull down can't be enabled on the first two pins
		    {
			    pullUpDnControl(inBufferPinMask[i], PUD_DOWN); //pull down enabled
		    }
	    }
	}

	//set pins as output
	for (int i = 0; i < MAX_OUTPUT; i++)
	{
	    if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
	    	pinMode(outBufferPinMask[i], OUTPUT);
	}

	//set PWM pins as output
	for (int i = 0; i < MAX_ANALOG_OUT; i++)
	{
	    if (pinNotPresent(ignored_int_outputs, ARRAY_SIZE(ignored_int_outputs), i))
    		pinMode(analogOutBufferPinMask[i], PWM_OUTPUT);
	}
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
// must be updated to reflect the actual state of the input pins. The mutex buffer_lock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersIn()
{
	std::lock_guard<std::mutex> lock(bufferLock); //lock mutex

	//INPUT
	for (int i = 0; i < MAX_INPUT; i++)
	{
	    if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
    		if (bool_input[i/8][i%8] != NULL) *bool_input[i/8][i%8] = digitalRead(inBufferPinMask[i]);
	}
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual state of the output pins. The mutex buffer_lock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
	std::lock_guard<std::mutex> lock(bufferLock); //lock mutex

	//OUTPUT
	for (int i = 0; i < MAX_OUTPUT; i++)
	{
	    if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
    		if (bool_output[i/8][i%8] != NULL) digitalWrite(outBufferPinMask[i], *bool_output[i/8][i%8]);
	}

	//ANALOG OUT (PWM)
	for (int i = 0; i < MAX_ANALOG_OUT; i++)
	{
	    if (pinNotPresent(ignored_int_outputs, ARRAY_SIZE(ignored_int_outputs), i))
    		if (int_output[i] != NULL) pwmWrite(analogOutBufferPinMask[i], (*int_output[i] / 64));
	}
}
