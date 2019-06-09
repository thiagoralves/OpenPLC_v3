//-----------------------------------------------------------------------------
// Copyright 2017 Thiago Alves
//
// Based on original PiXtend V2 -S- library created by 
// Robin Turner from Qube Solutions UG, 2017
// For more information about PiXtend(R) and this program,
// see <http://www.pixtend.de> or <http://www.pixtend.com>
//
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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <inttypes.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <wiringPiSPI.h>
#include <wiringSerial.h>
#include <pthread.h>
#include <string.h>

#include "ladder.h"
#include "custom_layer.h"

#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#define MAX_DIG_IN          8
#define MAX_DIG_OUT         4
#define MAX_REL_OUT         4
#define MAX_GPIO_OUT        4
#define MAX_GPIO_IN         4
#define MAX_GPIO_CTRL       1
#define MAX_UC_CTRL         2
#define MAX_ANALOG_IN       2
#define MAX_ANALOG_OUT      4
#define MAX_TEMP_IN         4
#define MAX_HUMID_IN        4

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

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

//STRUCTURES AND METHODS DECLARATIONS FROM PIXTEND
struct pixtOutV2S {
    uint8_t byModelOut;
    uint8_t byUCMode;
    uint8_t byUCCtrl0;
    uint8_t byUCCtrl1;
    uint8_t byDigitalInDebounce01;
    uint8_t byDigitalInDebounce23;
    uint8_t byDigitalInDebounce45;
    uint8_t byDigitalInDebounce67;
    uint8_t byDigitalOut;
    uint8_t byRelayOut; 
    uint8_t byGPIOCtrl;
    uint8_t byGPIOOut;
    uint8_t byGPIODebounce01;
    uint8_t byGPIODebounce23;
    uint8_t byPWM0Ctrl0; 
    uint16_t wPWM0Ctrl1;   
    uint16_t wPWM0A;
    uint16_t wPWM0B;
    uint8_t byPWM1Ctrl0;
    uint8_t byPWM1Ctrl1;
    uint8_t byPWM1A; 
    uint8_t byPWM1B;
    uint8_t byJumper10V;
    uint8_t byGPIO0Dht11;
    uint8_t byGPIO1Dht11;
    uint8_t byGPIO2Dht11;
    uint8_t byGPIO3Dht11;
    uint8_t abyRetainDataOut[32];
};

struct pixtOutDAC {
    uint16_t wAOut0;
    uint16_t wAOut1;
};

struct pixtInV2S {
    uint8_t byFirmware; 
    uint8_t byHardware;
    uint8_t byModelIn;
    uint8_t byUCState;
    uint8_t byUCWarnings;
    uint8_t byDigitalIn;
    uint16_t wAnalogIn0;
    uint16_t wAnalogIn1;
    uint8_t byGPIOIn;
    uint16_t wTemp0;
    uint8_t byTemp0Error;
    uint16_t wTemp1;
    uint8_t byTemp1Error;
    uint16_t wTemp2;
    uint8_t byTemp2Error;
    uint16_t wTemp3;
    uint8_t byTemp3Error;
    uint16_t wHumid0;
    uint16_t wHumid1;
    uint16_t wHumid2;
    uint16_t wHumid3;
    float rAnalogIn0;
    float rAnalogIn1;
    float rTemp0;
    float rTemp1;
    float rTemp2;
    float rTemp3;
    float rHumid0;
    float rHumid1;
    float rHumid2;
    float rHumid3;
    uint8_t abyRetainDataIn[32];
};

uint16_t crc16_calc(uint16_t crc, uint8_t data);
int Spi_AutoModeV2S(struct pixtOutV2S *OutputData, struct pixtInV2S *InputData);
int Spi_AutoModeDAC(struct pixtOutDAC *OutputDataDAC);
int Spi_SetupV2(int spi_device);
int Spi_Set_Aout(int channel, uint16_t value);

//IMPLEMENTATION OF PIXTEND LIBRARY
static uint8_t byJumper10V;
static uint8_t byInitFlag = 0;

uint16_t crc16_calc(uint16_t crc, uint8_t data)
{
    int i;
    crc ^= data;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
        {
            crc = (crc >> 1) ^ 0xA001;
        }
        else
        {
            crc = (crc >> 1);
        }
    }
    return crc;
}

int Spi_AutoModeDAC(struct pixtOutDAC *OutputDataDAC) {
    
    Spi_Set_Aout(0, OutputDataDAC->wAOut0);
    Spi_Set_Aout(1, OutputDataDAC->wAOut1);
        
    return 0;
}

int Spi_Set_Aout(int channel, uint16_t value)
{
	unsigned char spi_output[2];
	int spi_device = 1;
	int len = 2;
	uint16_t tmp;

	spi_output[0] = 0b00010000;

	if(channel)
	{
		spi_output[0] = spi_output[0] | 0b10000000;
	}
	if(value > 1023)
	{
		value=1023;
	}

	tmp = value & 0b1111000000;
	tmp = tmp >> 6;
	spi_output[0]=spi_output[0] | tmp;

	tmp = value & 0b0000111111;
	tmp = tmp << 2;
	spi_output[1]=tmp;

	wiringPiSPIDataRW(spi_device, spi_output, len);

	return 0;
}

int Spi_AutoModeV2S(struct pixtOutV2S *OutputData, struct pixtInV2S *InputData)
{
    uint16_t crcSumHeader;
    uint16_t crcSumData;
    uint16_t crcSumHeaderRx;
    uint16_t crcSumHeaderRxCalc;
    uint16_t crcSumDataRx;
    uint16_t crcSumDataRxCalc;
    uint16_t wTempValue;
    int i;
    unsigned char spi_output[67];
    int spi_device = 0;
    int len = 67;
    
    spi_output[0] = OutputData->byModelOut;                                
    spi_output[1] = OutputData->byUCMode;
    spi_output[2] = OutputData->byUCCtrl0;
    spi_output[3] = OutputData->byUCCtrl1;
    spi_output[4] = 0;     //Reserved
    spi_output[5] = 0;     //Reserved
    spi_output[6] = 0;     //Reserved
    spi_output[7] = 0; // Reserved for Header CRC value
    spi_output[8] = 0; // Reserver for Header CRC value
    spi_output[9] = OutputData->byDigitalInDebounce01;
    spi_output[10] = OutputData->byDigitalInDebounce23;
    spi_output[11] = OutputData->byDigitalInDebounce45;
    spi_output[12] = OutputData->byDigitalInDebounce67;
    spi_output[13] = OutputData->byDigitalOut;
    spi_output[14] = OutputData->byRelayOut;
    spi_output[15] = OutputData->byGPIOCtrl;
    spi_output[16] = OutputData->byGPIOOut;
    spi_output[17] = OutputData->byGPIODebounce01;
    spi_output[18] = OutputData->byGPIODebounce23;
    spi_output[19] = OutputData->byPWM0Ctrl0;
    spi_output[20] = (uint8_t)(OutputData->wPWM0Ctrl1 & 0xFF);
    spi_output[21] = (uint8_t)((OutputData->wPWM0Ctrl1>>8) & 0xFF);
    spi_output[22] = (uint8_t)(OutputData->wPWM0A & 0xFF);
    spi_output[23] = (uint8_t)((OutputData->wPWM0A>>8) & 0xFF);
    spi_output[24] = (uint8_t)(OutputData->wPWM0B & 0xFF);
    spi_output[25] = (uint8_t)((OutputData->wPWM0B>>8) & 0xFF);
    spi_output[26] = OutputData->byPWM1Ctrl0;
    spi_output[27] = OutputData->byPWM1Ctrl1;
    spi_output[28] = 0;     //Reserved
    spi_output[29] = OutputData->byPWM1A;
    spi_output[30] = 0;     //Reserved
    spi_output[31] = OutputData->byPWM1B;
    spi_output[32] = 0;     //Reserved

    //Add Retain data to SPI output          
    for (i=0; i <= 31; i++) 
    {
        spi_output[33+i] = OutputData->abyRetainDataOut[i];
    }

    spi_output[65] = 0; //Reserved for data CRC
    spi_output[66] = 0; //Reserved for data CRC            
    //Save physical jumper setting given by user for this call          
    byJumper10V = OutputData->byJumper10V;

    //Calculate CRC16 Header Transmit Checksum
    crcSumHeader = 0xFFFF;
    for (i=0; i < 7; i++) 
    {
        crcSumHeader = crc16_calc(crcSumHeader, spi_output[i]);
    }
    spi_output[7]=crcSumHeader & 0xFF;    //CRC Low Byte
    spi_output[8]=crcSumHeader >> 8;    //CRC High Byte

    //Calculate CRC16 Data Transmit Checksum
    crcSumData = 0xFFFF;
    for (i=9; i < 65; i++) 
    {
        crcSumData = crc16_calc(crcSumData, spi_output[i]);
    }
    spi_output[65]=crcSumData & 0xFF; //CRC Low Byte
    spi_output[66]=crcSumData >> 8;      //CRC High Byte
    
    //-------------------------------------------------------------------------
    //Initialise SPI Data Transfer with OutputData
    wiringPiSPIDataRW(spi_device, spi_output, len);
    //-------------------------------------------------------------------------

    //Calculate Header CRC16 Receive Checksum
    crcSumHeaderRxCalc = 0xFFFF;
    for (i=0; i <= 6; i++) 
    {
        crcSumHeaderRxCalc = crc16_calc(crcSumHeaderRxCalc, spi_output[i]);
    }
    
    crcSumHeaderRx = (spi_output[8]<<8) + spi_output[7];
    
    //Check that CRC sums match
    if (crcSumHeaderRx != crcSumHeaderRxCalc)
        return -1;

    if (spi_output[2] != 83)
        return -2;

    //-------------------------------------------------------------------------
    // Data received is OK, CRC and model matched
    //-------------------------------------------------------------------------
    //spi_output now contains all returned data, assign values to InputData

    InputData->byFirmware = spi_output[0];
    InputData->byHardware = spi_output[1];
    InputData->byModelIn = spi_output[2];
    InputData->byUCState = spi_output[3];
    InputData->byUCWarnings = spi_output[4];
    //spi_output[5]; //Reserved
    //spi_output[6]; //Reserved
    //spi_output[7]; //CRC Reserved
    //spi_output[8]; //CRC Reserved
    
    //Calculate Data CRC16 Receive Checksum
    crcSumDataRxCalc = 0xFFFF;
    for (i=9; i <= 64; i++) 
    {
        crcSumDataRxCalc = crc16_calc(crcSumDataRxCalc, spi_output[i]);
    }
    
    crcSumDataRx = (spi_output[66]<<8) + spi_output[65];
    
    if (crcSumDataRxCalc != crcSumDataRx)
        return -3;    
    
    InputData->byDigitalIn =spi_output[9];
    InputData->wAnalogIn0 = (uint16_t)(spi_output[11]<<8)|(spi_output[10]);
    InputData->wAnalogIn1 = (uint16_t)(spi_output[13]<<8)|(spi_output[12]);
    InputData->byGPIOIn = spi_output[14];
    //----------------------------------------------------------------------------------------------------
    //Check Temp0 and Humid0 for value 255, meaning read error
    if (spi_output[16] == 255 && spi_output[15] == 255 && spi_output[18] == 255 && spi_output[17] == 255){
        InputData->byTemp0Error = 1;
    }
    else{
        InputData->wTemp0 = (uint16_t)(spi_output[16]<<8)|(spi_output[15]);
        InputData->wHumid0 = (uint16_t)(spi_output[18]<<8)|(spi_output[17]);
        InputData->byTemp0Error = 0;
    }
    //----------------------------------------------------------------------------------------------------
    //Check Temp1 and Humid1 for value 255, meaning read error
     if (spi_output[20] == 255 && spi_output[19] == 255 && spi_output[22] == 255 && spi_output[21] == 255){
        InputData->byTemp1Error = 1;
    }
    else{
        InputData->wTemp1 = (uint16_t)(spi_output[20]<<8)|(spi_output[19]);
        InputData->wHumid1 = (uint16_t)(spi_output[22]<<8)|(spi_output[21]);
        InputData->byTemp1Error = 0;
    }
    //----------------------------------------------------------------------------------------------------
    //Check Temp2 and Humid2 for value 255, meaning read error
     if (spi_output[24] == 255 && spi_output[23] == 255 && spi_output[26] == 255 && spi_output[25] == 255){
        InputData->byTemp2Error = 1;
    }
    else{
        InputData->wTemp2 = (uint16_t)(spi_output[24]<<8)|(spi_output[23]);
        InputData->wHumid2 = (uint16_t)(spi_output[26]<<8)|(spi_output[25]);
        InputData->byTemp2Error = 0;
    }
    //----------------------------------------------------------------------------------------------------
    //Check Temp3 and Humid3 for value 255, meaning read error
     if (spi_output[28] == 255 && spi_output[27] == 255 && spi_output[30] == 255 && spi_output[29] == 255){
        InputData->byTemp3Error = 1;
    }
    else{
        InputData->wTemp3 = (uint16_t)(spi_output[28]<<8)|(spi_output[27]);
        InputData->wHumid3 = (uint16_t)(spi_output[30]<<8)|(spi_output[29]);
        InputData->byTemp3Error = 0;
    }
    //----------------------------------------------------------------------------------------------------
    //spi_output[31]; //Reserved
    //spi_output[32]; //Reserved

    if (byJumper10V & (0b00000001)) {
        InputData->rAnalogIn0 = (float)(InputData->wAnalogIn0) * (10.0 / 1024);
    } 
    else {
        InputData->rAnalogIn0 = (float)(InputData->wAnalogIn0) * (5.0 / 1024);
    }
    if (byJumper10V & (0b00000010)) {
        InputData->rAnalogIn1 = (float)(InputData->wAnalogIn1) * (10.0 / 1024);
    } 
    else {
        InputData->rAnalogIn1 = (float)(InputData->wAnalogIn1) * (5.0 / 1024);
    }
    
    //Check if user chose DHT11 or DHT22 sensor at GPIO0, 1 = DHT11 and 0 = DHT22
    if (OutputData->byGPIO0Dht11 == 1){
        InputData->rTemp0 = (float)(InputData->wTemp0 / 256);
        InputData->rHumid0 = (float)(InputData->wHumid0 / 256);
    }
    else{
        //For DHT22 sensors check bit 15, if set temperature value is negative
        wTempValue = InputData->wTemp0;
        if ((wTempValue >> 15) & 1) {
            wTempValue &= ~(1 << 15);
            InputData->rTemp0 = ((float)(wTempValue) / 10.0) * -1.0;
        }
        else {
            InputData->rTemp0 = (float)(InputData->wTemp0) / 10.0;
        }
        InputData->rHumid0 = (float)(InputData->wHumid0) / 10.0;
    }
    //Check if user chose DHT11 or DHT22 sensor at GPIO1, 1 = DHT11 and 0 = DHT22
     if (OutputData->byGPIO1Dht11 == 1){
        InputData->rTemp1 = (float)(InputData->wTemp1 / 256);
        InputData->rHumid1 = (float)(InputData->wHumid1 / 256);
    }
    else{
        //For DHT22 sensors check bit 15, if set temperature value is negative
        wTempValue = InputData->wTemp1;
        if ((wTempValue >> 15) & 1) {
            wTempValue &= ~(1 << 15);
            InputData->rTemp1 = ((float)(wTempValue) / 10.0) * -1.0;
        }
        else {
            InputData->rTemp1 = (float)(InputData->wTemp1) / 10.0;
        }
        InputData->rHumid1 = (float)(InputData->wHumid1) / 10.0;
    }
    //Check if user chose DHT11 or DHT22 sensor at GPIO2, 1 = DHT11 and 0 = DHT22
     if (OutputData->byGPIO2Dht11 == 1){
        InputData->rTemp2 = (float)(InputData->wTemp2 / 256);
        InputData->rHumid2 = (float)(InputData->wHumid2 / 256);
    }
    else{
        //For DHT22 sensors check bit 15, if set temperature value is negative
        wTempValue = InputData->wTemp2;
        if ((wTempValue >> 15) & 1) {
            wTempValue &= ~(1 << 15);
            InputData->rTemp2 = ((float)(wTempValue) / 10.0) * -1.0;
        }
        else {
            InputData->rTemp2 = (float)(InputData->wTemp2) / 10.0;
        }
        InputData->rHumid2 = (float)(InputData->wHumid2) / 10.0;
    }
    //Check if user chose DHT11 or DHT22 sensor at GPIO3, 1 = DHT11 and 0 = DHT22
     if (OutputData->byGPIO3Dht11 == 1){
        InputData->rTemp3 = (float)(InputData->wTemp3 / 256);
        InputData->rHumid3 = (float)(InputData->wHumid3 / 256);
    }
    else{
        //For DHT22 sensors check bit 15, if set temperature value is negative
        wTempValue = InputData->wTemp3;
        if ((wTempValue >> 15) & 1) {
            wTempValue &= ~(1 << 15);
            InputData->rTemp3 = ((float)(wTempValue) / 10.0) * -1.0;
        }
        else {
            InputData->rTemp3 = (float)(InputData->wTemp3) / 10.0;
        }
        InputData->rHumid3 = (float)(InputData->wHumid3) / 10.0;
    }
    //Get Retain data from SPI input          
    for (i=0; i <= 31; i++) 
    {
        InputData->abyRetainDataIn[i] = spi_output[33+i];
    }

    return 0;
}

int Spi_SetupV2(int spi_device)
{    
    int pin_Spi_enable = 5;
    int Spi_frequency = 700000;
    if(byInitFlag < 1)
    {
        wiringPiSetup();
        byInitFlag = 1;
    }
    
    pinMode(pin_Spi_enable, OUTPUT);
    digitalWrite(pin_Spi_enable,1); 
    
    wiringPiSPISetup(spi_device, Spi_frequency);

    return 0;
}

pthread_mutex_t localBufferLock; //mutex for the internal ADC buffer
struct pixtInV2S InputData;
struct pixtOutV2S OutputData;
struct pixtOutDAC OutputDataDAC;
static const uint8_t byModel = 83;

void *updateLocalBuffers(void *args)
{
    struct pixtInV2S InputData_thread;
    struct pixtOutV2S OutputData_thread;
    struct pixtOutDAC OutputDataDAC_thread;

    while(1)
    {
        pthread_mutex_lock(&localBufferLock);
        memcpy(&OutputData_thread, &OutputData, sizeof(pixtOutV2S));
        memcpy(&OutputDataDAC_thread, &OutputDataDAC, sizeof(pixtOutDAC));
        pthread_mutex_unlock(&localBufferLock);

        //Exchange PiXtend Data
        OutputData_thread.byModelOut = byModel;
        Spi_AutoModeV2S(&OutputData_thread, &InputData_thread);
        Spi_AutoModeDAC(&OutputDataDAC_thread);

        pthread_mutex_lock(&localBufferLock);
        memcpy(&InputData, &InputData_thread, sizeof(pixtInV2S));
        pthread_mutex_unlock(&localBufferLock);

        sleep_ms(30); //For temperature measurement MUST be 30 ms
    }
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{       
    Spi_SetupV2(0);
    Spi_SetupV2(1);

    pthread_t piXtend_thread;
    pthread_create(&piXtend_thread, NULL, updateLocalBuffers, NULL);
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
	//lock mutexes
	pthread_mutex_lock(&bufferLock);
	pthread_mutex_lock(&localBufferLock);
    
    //DIGITAL INPUT
    for (int i = 0; i < MAX_DIG_IN; i++)
    {
        if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
            if (bool_input[i/8][i%8] != NULL) *bool_input[i/8][i%8] = bitRead(InputData.byDigitalIn, i);
    }
    
    //GPIO INPUT
    for (int i = MAX_DIG_IN; i < MAX_DIG_IN+MAX_GPIO_IN; i++)
    {
        if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
            if (bool_input[i/8][i%8] != NULL) *bool_input[i/8][i%8] = bitRead(InputData.byGPIOIn, i-MAX_DIG_IN);
    }
    
    // uint8_t byFirmware;
    if (byte_input[0] != NULL) *byte_input[0] = InputData.byFirmware;
    // uint8_t byHardware;
    if (byte_input[1] != NULL) *byte_input[1] = InputData.byHardware;
    // uint8_t byModelIn;
    if (byte_input[2] != NULL) *byte_input[2] = InputData.byModelIn;
    // uint8_t byUCState;
    if (byte_input[3] != NULL) *byte_input[3] = InputData.byUCState;
    // uint8_t byUCWarnings
    if (byte_input[4] != NULL) *byte_input[4] = InputData.byUCWarnings; 

    //ANALOG IN - TEMP INPUT - HUMID INPUT
    uint16_t *analogInputs;
    analogInputs = &InputData.wAnalogIn0;
    for (int i = 0; i < MAX_ANALOG_IN+MAX_TEMP_IN+MAX_HUMID_IN; i++)
    {
        if (i < MAX_ANALOG_IN)
        {
            if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
                if (int_input[i] != NULL) *int_input[i] = analogInputs[i];
        }
        if ((i >= MAX_ANALOG_IN) && ( i < MAX_ANALOG_IN+MAX_TEMP_IN)) 
        {
            if (i == MAX_ANALOG_IN){
                if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
                    if (int_input[i] != NULL) *int_input[i] = InputData.wTemp0;
            }
            if (i == (MAX_ANALOG_IN+1)){
                if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
                    if (int_input[i] != NULL) *int_input[i] = InputData.wTemp1;
            }
            if (i == (MAX_ANALOG_IN+2)){
                if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
                    if (int_input[i] != NULL) *int_input[i] = InputData.wTemp2;
            }
            if (i == (MAX_ANALOG_IN+3)){
                if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
                    if (int_input[i] != NULL) *int_input[i] = InputData.wTemp3;
            }
        }
        if ((i >= (MAX_ANALOG_IN+MAX_TEMP_IN)) && ( i < (MAX_ANALOG_IN+MAX_TEMP_IN+MAX_HUMID_IN))) 
        {
            if (i == (MAX_ANALOG_IN+MAX_TEMP_IN))
            {
                if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
                    if (int_input[i] != NULL) *int_input[i] = InputData.wHumid0;
            }
            if (i == ((MAX_ANALOG_IN+MAX_TEMP_IN)+1))
            {
                if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
                    if (int_input[i] != NULL) *int_input[i] = InputData.wHumid1;
            }
            if (i == ((MAX_ANALOG_IN+MAX_TEMP_IN)+2))
            {
                if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
                    if (int_input[i] != NULL) *int_input[i] = InputData.wHumid2;
            }
            if (i == ((MAX_ANALOG_IN+MAX_TEMP_IN)+3))
            {
                if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
                    if (int_input[i] != NULL) *int_input[i] = InputData.wHumid3;
            }
        }
            
    }
   
	//unlock mutexes
	pthread_mutex_unlock(&localBufferLock);
	pthread_mutex_unlock(&bufferLock);
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Output state. The mutex buffer_lock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
	//lock mutexes
	pthread_mutex_lock(&bufferLock);
	pthread_mutex_lock(&localBufferLock);   
    
    //DIGITAL OUTPUT
    for (int i = 0; i < MAX_DIG_OUT; i++)
    {
        if (i < MAX_DIG_OUT)
        {
            if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
                if (bool_output[i/8][i%8] != NULL) bitWrite(OutputData.byDigitalOut, i, *bool_output[i/8][i%8]);
        }  
    }
    
    //RELAY OUTPUT
    for (int i = 0; i < MAX_DIG_OUT+MAX_REL_OUT; i++)
    {
        if ((i >= MAX_DIG_OUT) && (i < (MAX_DIG_OUT+MAX_REL_OUT)))
        {
            if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
                if (bool_output[i/8][i%8] != NULL) bitWrite(OutputData.byRelayOut, i-MAX_DIG_OUT, *bool_output[i/8][i%8]);
        }
    }
    
    //GPIO OUTPUT    
    for (int i = 0; i < MAX_DIG_OUT+MAX_REL_OUT+MAX_GPIO_OUT; i++)
    {
        if ((i >= MAX_DIG_OUT+MAX_REL_OUT) && (i < (MAX_DIG_OUT+MAX_REL_OUT+MAX_GPIO_OUT)))
        {
            if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
                if (bool_output[i/8][i%8] != NULL) bitWrite(OutputData.byGPIOOut, i-(MAX_DIG_OUT+MAX_REL_OUT), *bool_output[i/8][i%8]);
        }
    }

    //ANALOG OUT
    uint16_t *analogOutputs;
    uint16_t *pwmOutputs;
    analogOutputs = &OutputDataDAC.wAOut0;
    pwmOutputs = &OutputData.wPWM0A;
    for (int i = 0; i < MAX_ANALOG_OUT; i++)
    {
        if (i < 2)
        {
            if (pinNotPresent(ignored_int_outputs, ARRAY_SIZE(ignored_int_outputs), i))
                if (int_output[i] != NULL) analogOutputs[i] = (*int_output[i] / 64);
        }
        else
        {
            if (pinNotPresent(ignored_int_outputs, ARRAY_SIZE(ignored_int_outputs), i))
                if (int_output[i] != NULL) pwmOutputs[i-2] = *int_output[i];
        }
    }
    // PWM0Ctrl1L - PWM0Ctrl1H
    if (pinNotPresent(ignored_int_outputs, ARRAY_SIZE(ignored_int_outputs), 4))
        if (int_output[4] != NULL) OutputData.wPWM0Ctrl1 = *int_output[4];
    
    // UCCtrl0
    if (byte_output[0] != NULL) OutputData.byUCCtrl0 = *byte_output[0];
    // UCCtrl1
    if (byte_output[1] != NULL) OutputData.byUCCtrl1 = *byte_output[1];
    // GPIOCtrl    
    if (byte_output[2] != NULL) OutputData.byGPIOCtrl = *byte_output[2];
    // PWM0 - PWM0Ctrl0
    if (byte_output[3] != NULL) OutputData.byPWM0Ctrl0 = *byte_output[3];
    // PWM1 - PWM1Ctrl0
    if (byte_output[4] != NULL) OutputData.byPWM1Ctrl0 = *byte_output[4];
    // PWM1Ctrl1L - PWM1Ctrl1H
    if (byte_output[5] != NULL) OutputData.byPWM1Ctrl1 = *byte_output[5];
    // PWM1AL - PWM1AH
    if (byte_output[6] != NULL) OutputData.byPWM1A = *byte_output[6];
    // PWM1BL - PWM1BH     
    if (byte_output[7] != NULL) OutputData.byPWM1B = *byte_output[7];
    
	//unlock mutexes
	pthread_mutex_unlock(&localBufferLock);
	pthread_mutex_unlock(&bufferLock);
}
