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
#include <wiringPiI2C.h>
#include <mcp23008.h>
#include <pthread.h>

#include "ladder.h"
#include "custom_layer.h"

#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#define DOUT_PINBASE		100
#define ADC_PINBASE			65
#define MAX_INPUT			15 //IO0 and IO1 are the same. The board doesn't have IO0
#define MAX_OUTPUT			8
#define ANALOG_OUT_PIN		1

const int inputPinMask[MAX_INPUT] = { 7, 7, 0, 2, 4, 3, 5, 14, 11, 10, 13, 6, 12, 21, 22 };
int adc_fd;
unsigned int adcBuffer[2];

pthread_mutex_t adcBufferLock; //mutex for the internal ADC buffer

//-----------------------------------------------------------------------------
// Helper function - Makes the running thread sleep for the ammount of time
// in milliseconds
//-----------------------------------------------------------------------------
void sleep_ms(int milliseconds)
{
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

void *readAdcThread(void *args)
{
	while(1)
	{
		unsigned char config;
		unsigned char i2c_data[4];
		unsigned int value = 0;
        double conversion;
		bool conversionInProgress;

		/*
		//DEBUG
		bool RDY, continuous;
		unsigned char channel, sample_rate, gain;

		read(adc_fd, i2c_data, 3);
		RDY = (i2c_data[2] >> 7);
		channel = (i2c_data[2] >> 5) & 0x03;
		continuous = (i2c_data[2] >> 4) & 0x01;
		sample_rate = (i2c_data[2] >> 2) & 0x03;
		gain = i2c_data[2] & 0x03;
		printf("RDY: %d\nchannel: %d\ncontinuous: %d\nsample_rate: %d\ngain: %d\n", RDY, channel, continuous, sample_rate, gain);
		*/

		config = 0x88; //read channel 0
		wiringPiI2CWrite(adc_fd, config);
		i2c_data[2] = 0;
		conversionInProgress = 1;

		while (conversionInProgress)
		{
			read(adc_fd, i2c_data, 3);
			conversionInProgress = (i2c_data[2] >> 7);
			if (conversionInProgress)
				sleep_ms(1);
		}
		value = (i2c_data [0] << 8) | i2c_data [1];
        conversion = value*2.28;
        if (conversion > 65535.0) {conversion=65535.0;}
        value = (unsigned int)conversion;
		pthread_mutex_lock(&adcBufferLock);
		adcBuffer[0] = value;
		pthread_mutex_unlock(&adcBufferLock);

		config = 0xA8; //read channel 1
		wiringPiI2CWrite(adc_fd, config);
		i2c_data[2] = 0;
		conversionInProgress = 1;

		while (conversionInProgress)
		{
			read(adc_fd, i2c_data, 3);
			conversionInProgress = (i2c_data[2] >> 7);
			if (conversionInProgress)
				sleep_ms(1);
		}
		value = (i2c_data [0] << 8) | i2c_data [1];
        conversion = value*2.28;
        if (conversion > 65535.0) {conversion=65535.0;}
        value = (unsigned int)conversion;
		pthread_mutex_lock(&adcBufferLock);
		adcBuffer[1] = value;
		pthread_mutex_unlock(&adcBufferLock);
	}
}

int mcp_adcSetup (int i2cAddress)
{
	if ((adc_fd = wiringPiI2CSetup(i2cAddress)) < 0)
	{
		printf("error on adc setup!\n");
		return adc_fd;
	}

	return 0 ;
}

int mcp_adcRead(int chan)
{
	int returnValue;
	if (chan < 2)
	{
		pthread_mutex_lock(&adcBufferLock);
		returnValue = adcBuffer[chan];
		pthread_mutex_unlock(&adcBufferLock);

		return returnValue;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
	wiringPiSetup();
	mcp_adcSetup(0x68); //ADC I2C address configuration
	mcp23008Setup(DOUT_PINBASE, 0x20); //Digital out I2C configuration
    
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(47);
    pwmSetRange(1024);

	for (int i = 0; i < MAX_OUTPUT; i++)
	{
	    if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
		    pinMode(DOUT_PINBASE + i, OUTPUT);
	}

	for (int i = 0; i < MAX_INPUT; i++)
	{
	    if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
	    {
		    pinMode(inputPinMask[i], INPUT);
		    pullUpDnControl(inputPinMask[i], PUD_UP);
	    }
	}

    if (pinNotPresent(ignored_int_outputs, ARRAY_SIZE(ignored_int_outputs), ANALOG_OUT_PIN))
	    pinMode(ANALOG_OUT_PIN, PWM_OUTPUT);

	pthread_t ADCthread;
	pthread_create(&ADCthread, NULL, readAdcThread, NULL);
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
// must be updated to reflect the actual Input state. The mutex buffer_lock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersIn()
{
	//printf("Digital Inputs:\n");
	pthread_mutex_lock(&bufferLock); //lock mutex
	for (int i = 0; i < MAX_INPUT; i++)
	{
	    if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
    		if (bool_input[i/8][i%8] != NULL) *bool_input[i/8][i%8] = !digitalRead(inputPinMask[i]); //printf("[IO%d]: %d | ", i, !digitalRead(inputPinMask[i]));
	}

	//printf("\nAnalog Inputs:");
	for (int i = 0; i < 2; i++)
	{
	    if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
    		if (int_input[i] != NULL) *int_input[i] = mcp_adcRead(i); //printf("[AI%d]: %d | ", i, mcp_adcRead(i));
	}
	//printf("\n");

	pthread_mutex_unlock(&bufferLock); //unlock mutex
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Output state. The mutex buffer_lock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
	pthread_mutex_lock(&bufferLock); //lock mutex

	//printf("\nDigital Outputs:\n");
	for (int i = 0; i < MAX_OUTPUT; i++)
	{
	    if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
    		if (bool_output[i/8][i%8] != NULL) digitalWrite(DOUT_PINBASE + i, *bool_output[i/8][i%8]); //printf("[IO%d]: %d | ", i, digitalRead(DOUT_PINBASE + i));
	}
	
	if (pinNotPresent(ignored_int_outputs, ARRAY_SIZE(ignored_int_outputs), 0))
	    if(int_output[0] != NULL) pwmWrite(ANALOG_OUT_PIN, (*int_output[0] / 64));
	
	pthread_mutex_unlock(&bufferLock); //unlock mutex
}
