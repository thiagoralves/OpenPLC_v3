/*
MIT License

Copyright (c) 2024 FACTS Engineering, LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>
#include <ctime>
#include <cstring>
#include <fcntl.h>              								// File control options
#include <sys/ioctl.h>          								// Manipulates underlying device parameters of special files
#include <linux/spi/spidev.h>
#include <linux/gpio.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>             								// Standard symbolic constants and types
#include <pthread.h>

#include "ladder.h"
//#include "custom_layer.h"

#define NUMBER_OF_MODULES	15 
#define slaveSelectPin 		27
#define slaveAckPin 		25
#define baseEnable			17

#define SPI_MODE			SPI_MODE2

#define MOD_HDR 			0x02
#define VERSION_HDR 		0x03
#define ACTIVE_HDR			0x04
#define DROPOUT_HDR			0x05
#define CFG_HDR				0x10
#define READ_CFG_HDR 		0x11
#define PETWD_HDR			0x30
#define STARTWD_HDR			0x31
#define STOPWD_HDR			0x32
#define CONFIGWD_HDR		0x33
#define READ_STATUS_HDR 	0x40
#define READ_DISCRETE_HDR 	0x50
#define READ_ANALOG_HDR		0x51
#define READ_BLOCK_HDR		0x52
#define WRITE_DISCRETE_HDR 	0x60
#define WRITE_ANALOG_HDR	0x61
#define WRITE_BLOCK_HDR		0x62
#define FW_UPDATE_HDR		0xAA

#define DUMMY				0xFF
#define EMPTY_SLOT_ID 		0xFFFFFFFE
#define MAX_TIMEOUT	  		0xFFFFFFFF

#define HIGH				1
#define LOW					0

#define SPI_DEVPATH     	"/dev/spidev6.0"
#define GPIO_DEVPATH    	"/dev/gpiochip0"

#define DEBUG_PRINT_ON
#define GEN_DEBUG 			1
#define LOG_ERROR			log_error(__LINE__)

#define OK					0
#define ERROR				-1

using namespace std;

/************************************************************************
 *                             MODULES                                  *
************************************************************************/
const struct moduleProps
{
	unsigned int moduleID;
	char diBytes;												// Number of bytes used by all Discrete Input channels
	char doBytes;												// Number of bytes used by all Discrete Output channels
	char aiBytes;												// Number of bytes used by all Analog Input channels
	char aoBytes;												// Number of bytes used by all Analog Output channels
	char statusBytes;											// Number of status bytes. Things like overrange errors or missing 24V.
	char configBytes;											// Number of bytes used to configure module. Things like ranges and enabled channels.
	char dataSize;												// Resolution or Specialty info
	const char* moduleName;										// Text name of module
}mdb[] = {
	// {0x000000ID,di,do,ai,ao,st,cf,ds}
    {0x00000000, 0, 0, 0, 0, 0, 0, 0, "Empty"}, //Empty first entry for defaults
	{0x04A010E1, 1, 0, 0, 0, 0, 0, 1, "SLM-DCI-8NP"},
	{0x04A000E5, 1, 0, 0, 0, 0, 0, 1, "SLM-ACI-8"},
	{0x04A000E7, 1, 0, 0, 0, 0, 0, 1, "SLM-SIM-8"},
	{0x04A000E8, 1, 0, 0, 0, 0, 0, 1, "SLM-ACDCI-8NP"},
	{0x052000E2, 2, 0, 0, 0, 0, 0, 1, "SLM-DCI-16NP"},
	{0x052000E9, 2, 0, 0, 0, 0, 0, 1, "SLM-ACDCI-16NP"},
	{0x140300E0, 0, 1, 0, 0, 0, 0, 1, "SLM-RLY-4-ISO"},
	{0x1403F4E1, 0, 0, 0, 32, 4, 4, 0xA0, "SLM-PWM-4"},
	{0x140400ED, 0, 1, 0, 0, 0, 0, 1, "SLM-ACO-8"},
	{0x140400EF, 0, 1, 0, 0, 0, 0, 1, "SLM-RLY-8-ISO"},
	{0x140400F1, 0, 2, 0, 0, 0, 0, 1, "SLM-RLY-16"},
	{0x04A000E2, 1, 0, 0, 0, 0, 0, 1, "SLM-TTL-8ND"},
	{0x140500E1, 0, 1, 0, 0, 0, 0, 1, "SLM-DCO-8N"},
	{0x140500E2, 0, 1, 0, 0, 0, 0, 1, "SLM-DCO-8P"},
	{0x140500E6, 0, 1, 0, 0, 0, 0, 1, "SLM-TTL-08TD"},
	{0x140800E5, 0, 2, 0, 0, 0, 0, 1, "SLM-DCO-15N"},
	{0x140800E6, 0, 2, 0, 0, 0, 0, 1, "SLM-DCO-15P"},
	{0x24A500E1, 1, 1, 0, 0, 0, 0, 1, "SLM-ACDCI-8NP-RLY8"},
	{0x24A500E2, 1, 1, 0, 0, 0, 0, 1, "SLM-DCI-8NP-DCO-7N"},
	{0x24A500E3, 1, 1, 0, 0, 0, 0, 1, "SLM-DCI-8NP-DCO-7P"},
	{0x346055E1, 0, 0, 16, 0, 12, 18, 16, "SLM-04AD-Bipolar"},
	{0x346055E8, 0, 0, 16, 0, 12, 8, 16, "SLM-RTD-4"},
    {0x346055E2, 0, 0, 16, 0, 12, 2, 16, "SLM-AI-4AD-mA"},
	{0x346055E3, 0, 0, 16, 0, 12, 2, 16, "SLM-AI-4AD-V"},
	{0x346055EF, 0, 0, 16, 0, 12, 2, 12, "SLM-AI-4ADL-mA"},
	{0x346055E0, 0, 0, 16, 0, 12, 2, 12, "SLM-AI-4ADL-V"},
	{0x34608CE1, 0, 0, 16, 0, 12, 20, 32, "SLM-THM-4"},
	{0x34608CEE, 0, 0, 16, 0, 12, 8, 32, "SLM-NTC-4"},
	{0x34A055EA, 0, 0, 32, 0, 12, 2, 12, "SLM-AI-8-mA"},
	{0x34A055EB, 0, 0, 32, 0, 12, 2, 12, "SLM-AI-8-V"},
	{0x440355E3, 0, 0, 0, 16, 4, 0, 12, "SLM-AO-4DAL-mA"},
	{0x440355E4, 0, 0, 0, 16, 4, 0, 12, "SLM-AO-4DAL-V"},
	{0x440555E8, 0, 0, 0, 32, 4, 0, 12, "SLM-AO-8-mA"},
	{0x440555E9, 0, 0, 0, 32, 4, 0, 12, "SLM-AO-8-V"},
	{0x5461A7E3, 0, 0, 16, 8, 12, 0, 12, "SLM-AI4-AO2-mA"},
	{0x5461A7E4, 0, 0, 16, 8, 12, 0, 12, "SLM-AI4-AO2-V"},
	{0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0, "BAD SLOT"}, //empty in case no modules are defined.
	{0x00000000, 0, 0, 0, 0, 0, 0, 0, "BAD SLOT"} //empty in case no modules are defined.
};

struct moduleInfo{
	uint8_t dbLoc;												// MDB location
}baseSlot[NUMBER_OF_MODULES];

const char SLM_AIH_4_mA_DEFAULT_CONFIG[] = {0x40,0x03};
const char SLM_AIH_4_V_DEFAULT_CONFIG[] = {0x40,0x03};
const char SLM_AI_4_mA_DEFAULT_CONFIG[] = {0x40,0x03};
const char SLM_AI_4_V_DEFAULT_CONFIG[] = {0x40,0x03};
const char SLM_AI_8_mA_DEFAULT_CONFIG[] = {0x40,0x07};
const char SLM_AI_8_V_DEFAULT_CONFIG[] = {0x40,0x07};
const char SLM_PWM_4_DEFAULT_CONFIG[] = {0x02,0x02,0x02,0x02};
// const char SLM_AI4_AO2_mA_DEFAULT_CONFIG[] = {0x40,0x03};
// const char SLM_AI4_AO2_V_DEFAULT_CONFIG[] = {0x40,0x03};
const char SLM_NTC_4_DEFAULT_CONFIG[] = {0x40,0x03,0x60,0x05,
										0x20,0x00,0x80,0x02};
const char SLM_THM_4_DEFAULT_CONFIG[] = {0x40,0x03,0x60,0x05,
										0x21,0x00,0x22,0x00,
										0x23,0x00,0x24,0x00,
				       						0x00,0x00,0x00,0x00,
				       						0x00,0x00,0x00,0x00};
const char SLM_RTD_4_DEFAULT_CONFIG[] = {0x40,0x03,0x60,0x05,
										0x20,0x01,0x80,0x00};
const char SLM_AIH_4_VmA_DEFAULT_CONFIG[] = 	{0x40,0x03,0x00,0x00,
										 0x20,0x03,0x00,0x00,
										 0x21,0x03,0x00,0x00,
										 0x22,0x03,0x00,0x00,
										 0x23,0x03};
const char SLM_HSC_DEFAULT_CONFIG[] = {0x00,0x00,0x00,0x00,
										0x00,0x00,0x00,0x01,
										0x00,0x00,0x00,0x01};


int log_error(int line)
{
	char log_msg[1000];

#ifdef GEN_DEBUG
	sprintf(log_msg, "!line %d\n", line);
	log(log_msg);
#endif
	return ERROR;
}

/************************************************************************
 *                  RASPBERRY Pi Functions                              *
************************************************************************/
int digitalWrite(int offset, uint8_t value){
    struct gpiohandle_request rq;
    struct gpiohandle_data data;
    int chip_fd, line_fd;

    // Open GPIO Device
    chip_fd = open(GPIO_DEVPATH, O_RDONLY);
    if (chip_fd < 0) {
		#ifdef DEBUG_PRINT_ON
        	cerr << "Failed to open GPIO device." << endl;
        #endif
		return -1;
    }

    // Set GPIO line as output
    rq.lineoffsets[0] = offset;
    rq.flags = GPIOHANDLE_REQUEST_OUTPUT;
    rq.lines = 1;

    if(ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &rq) < 0 ){
		#ifdef DEBUG_PRINT_ON
        	cerr << "Failed to get line handle." << endl;
        #endif
		close(chip_fd);
        return -1;
    }

    line_fd = rq.fd;

    // Set GPIO state
    data.values[0] = value;
    if (ioctl(line_fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) < 0) {
		#ifdef DEBUG_PRINT_ON
        	cerr << "Failed to set GPIO value." << endl;
		#endif
        close(line_fd);
        close(chip_fd);
        return -1;
    }
    close(line_fd);
    close(chip_fd);
    return 0;
}

int digitalRead(int offset){
    struct gpiohandle_request rq;
    struct gpiohandle_data data;
    int chip_fd, line_fd, reading;

     // Open GPIO Device
    chip_fd = open(GPIO_DEVPATH, O_RDONLY);
    if (chip_fd < 0) {
		#ifdef DEBUG_PRINT_ON
        	cerr << "Failed to open GPIO device." << endl;
		#endif
        return -1;
    }

    // Set GPIO line as input
    rq.lineoffsets[0] = offset;
    rq.flags = GPIOHANDLE_REQUEST_INPUT;
    rq.lines = 1;

    if(ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &rq) < 0 ){
		#ifdef DEBUG_PRINT_ON
        	cerr << "Failed to get line handle." << endl;
		#endif
        close(chip_fd);
        return -1;
    }

    line_fd = rq.fd;

    // Read GPIO state
    if (ioctl(line_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) < 0) {
		#ifdef DEBUG_PRINT_ON
        	cerr << "Failed to set GPIO value." << endl;
        #endif
		close(line_fd);
        close(chip_fd);
        return -1;
    }
    
    reading = +data.values[0];
    close(line_fd);
    close(chip_fd);
    return reading;
}

uint8_t rpiSPITransfer(int fd, uint8_t txByte){
    uint8_t rxByte;

    struct spi_ioc_transfer spi;
    memset(&spi, 0, sizeof(spi));
    
    spi.tx_buf = (unsigned long)&txByte;
    spi.rx_buf = (unsigned long)&rxByte;
    spi.len = 1;
    spi.delay_usecs = 0;
    spi.speed_hz = 1000000;

    if (ioctl(fd, SPI_IOC_MESSAGE(1), &spi) < 0) {
		#ifdef DEBUG_PRINT_ON
        	cerr << "SPI transfer failed." << endl;
		#endif
        return -1;
    }
    return rxByte;
}

int rpiSPIBegin(int mode){
    int spispeed = 1000000;


    int fd = open(SPI_DEVPATH, O_RDONLY);
    if (fd < 0) {
		#ifdef DEBUG_PRINT_ON
        	cerr << "Failed to open SPI device." << endl;
		#endif
    }

     if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
		#ifdef DEBUG_PRINT_ON
        	cerr << "Failed to set SPI mode." << endl;
		#endif
        return -1;
    }

    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spispeed) < 0) {
        #ifdef DEBUG_PRINT_ON
			cerr << "Failed to set SPI speed." << endl;
		#endif
        return -1;
    }
    return fd;
}

void rpiSPIEnd(int fd){
    close(fd);
}

void delay(int value){
    usleep(1000 * value);
}

void delayMicroseconds(int value){
    usleep(value);
}

uint32_t millis(){
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint32_t>(ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL);
}

/************************************************************************
 *                      SPI Functions		                            *
************************************************************************/
uint8_t spiSendRecvByte(uint8_t data){

	int fd = rpiSPIBegin(SPI_MODE_2);
	
	digitalWrite(slaveSelectPin, LOW);
	data = rpiSPITransfer(fd, data);
	digitalWrite(slaveSelectPin, HIGH);
	
	rpiSPIEnd(fd);

	return data;
}

uint32_t spiSendRecvInt(uint32_t data){ 						// will be used to read analog data
	uint8_t rData[4];
	uint8_t tData[4];
	uint32_t returnInt = 0;

	tData[0] = (data>>0)  & 0xFF;									// Data to write to the Base Controller - 1 byte long
	tData[1] = (data>>8)  & 0xFF;									// Data to write to the Base Controller - 1 byte long
	tData[2] = (data>>16) & 0xFF;									// Data to write to the Base Controller - 1 byte long
	tData[3] = (data>>24) & 0xFF;									// Data to write to the Base Controller - 1 byte long

	int fd = rpiSPIBegin(SPI_MODE_2);
	digitalWrite(slaveSelectPin, LOW);

	rData[0] = rpiSPITransfer(fd, tData[0]);
	rData[1] = rpiSPITransfer(fd, tData[1]);
	rData[2] = rpiSPITransfer(fd, tData[2]);
	rData[3] = rpiSPITransfer(fd, tData[3]);
    digitalWrite(slaveSelectPin, HIGH);
	rpiSPIEnd(fd);

	returnInt  = (rData[3]<<24);
	returnInt += (rData[2]<<16);
	returnInt += (rData[1]<<8);
	returnInt += (rData[0]<<0);

	return returnInt;
}

void spiSendRecvBuf(uint8_t *buf, int len, bool returnData = 0){

	int fd = rpiSPIBegin(SPI_MODE_2);
	digitalWrite(slaveSelectPin, LOW);

	if(returnData){
		for(int i = 0; i < len; ++i){
			buf[i] = rpiSPITransfer(fd, buf[i]); 					// Return what we get into our buffer
		}
	}
	else{
		for(int i = 0; i < len; ++i){
			rpiSPITransfer(fd, buf[i]); 							// Or don't return what we get
		}
	}

	digitalWrite(slaveSelectPin, HIGH);
	rpiSPIEnd(fd);
	return;
}

bool spiTimeout(uint32_t uS,uint8_t resendMsg = 0,uint16_t retryPeriod = 0){
	uint16_t retryTime = 0;

	while((!digitalRead(slaveAckPin)) && (uS != 0)){
		delayMicroseconds(1);
		uS--;

		retryTime++;
		if((retryPeriod) && (retryTime == retryPeriod)){			// If we specified a retry period and it equals the time we've waited

			spiSendRecvByte(resendMsg);
			retryTime = 0;
		}
	}
	delayMicroseconds(50);											// Small delay to let Base Controller load next msg in buf

	if(uS > 0){
		return 1;
	}
	else{
		#ifdef DEBUG_PRINT_ON
			// debugPrintln("Timeout");
		#endif
		return 0;
	}
}

void dataSync(){
	uint32_t currentMillis = 0;
	uint32_t startMillis = 0;
	
	startMillis = millis();
	while(!digitalRead(slaveAckPin)){
		currentMillis = millis();
		if(currentMillis - startMillis >= 200){
			#ifdef DEBUG_PRINT_ON
				// debugPrintln("Base Sync Timeout");
			#endif
			break;
		}
	}
	delayMicroseconds(1);
	
	startMillis = millis();
	while(digitalRead(slaveAckPin)){
		currentMillis = millis();
		if(currentMillis - startMillis >= 200){
			#ifdef DEBUG_PRINT_ON
				// debugPrintln("Base Sync Timeout");
			#endif
			break;
		}
	}
	delayMicroseconds(1);
	
	startMillis = millis();
	while(!digitalRead(slaveAckPin)){
		currentMillis = millis();
		if(currentMillis - startMillis >= 200){
			#ifdef DEBUG_PRINT_ON
				// debugPrintln("Base Sync Timeout");
			#endif
			break;
		}
	}
	delayMicroseconds(1);
	
	return;
}

/************************************************************************
*                    DISCRETE MODULE FUNCTIONS                          *
************************************************************************/
uint32_t readDiscrete(uint8_t slot, uint8_t channel = 0){
	uint32_t data = 0;
	uint8_t len = 0;
	uint8_t mdbLoc = 0;
	char rData[4] = {0,0,0,0};

	mdbLoc = baseSlot[slot-1].dbLoc;
	len = mdb[mdbLoc].diBytes;

	if((slot < 1) || (slot > NUMBER_OF_MODULES)){
		#ifdef DEBUG_PRINT_ON
			// debugPrint("Slots must be between 1 and ");
			// debugPrintln(NUMBER_OF_MODULES);
		#endif
		return 0;
	}

	if((len <= 0)){
		#ifdef DEBUG_PRINT_ON
			// debugPrint("Slot ");
			// debugPrint(slot);
			// debugPrintln(": This module has no Discrete Input bytes");
		#endif
		return 0;
	}

	if(channel > (len * 8)){		//8 channels per byte
		#ifdef DEBUG_PRINT_ON
			// debugPrintln("This channel is not valid");
		#endif
		return 0;
	}

	rData[0] = READ_DISCRETE_HDR;
	rData[1] = slot;
	spiSendRecvBuf((uint8_t *)rData,2);
	memset(rData,0,4);		//clear buffer
	if(spiTimeout(1000*200) == true){
		spiSendRecvBuf((uint8_t *)rData,len,true);
		data  = (rData[3]<<24);
		data += (rData[2]<<16);
		data += (rData[1]<<8);
		data += (rData[0]<<0);

		if(channel != 0){
			data = (data>>(channel-1)) & 1;	// shift and mask
		}
		dataSync();
		return data;
	}
	else{
		#ifdef DEBUG_PRINT_ON
			// debugPrintln("Slow read");
		#endif
		delay(100);
		return 0;
	}
}

void writeDiscrete(uint32_t data,uint8_t slot, uint8_t channel = 0){
	uint8_t tData[7];
	uint8_t mdbLoc = 0;
	uint8_t len = 0;

	mdbLoc = baseSlot[slot-1].dbLoc;
	len = mdb[mdbLoc].doBytes;

	if((slot < 1) || (slot > NUMBER_OF_MODULES)){
		#ifdef DEBUG_PRINT_ON
			// debugPrint("Slots must be between 1 and ");
			// debugPrintln(NUMBER_OF_MODULES);
		#endif
		return;
	}

	if((len <= 0)){
		#ifdef DEBUG_PRINT_ON
			// debugPrint("Slot ");
			// debugPrint(slot);
			// debugPrintln(": This module has no Discrete Output bytes");
		#endif
		return;
	}

	if(channel > (len * 8)){		//8 channels per byte
		#ifdef DEBUG_PRINT_ON
			// debugPrintln("This channel is not valid");
		#endif
		return;
	}

	tData[0] = WRITE_DISCRETE_HDR;
	tData[1] = slot;
	tData[2] = channel;

	if(channel == 0){
		for(int i=0;i<len;i++){
			tData[i+3] = data>>(8*i)  & 0xFF;	//Shift and mask
		}
	}
	else{
		tData[3] = data & 0b1;			//mask bit
		len = 1;						//only send 1 byte
	}


	spiSendRecvBuf(tData,len+3);	//3 Header bytes plus data length

	dataSync();
	return;
}

/************************************************************************
*                    ANALOG MODULE FUNCTIONS                            *
************************************************************************/
int readAnalog(uint8_t slot, uint8_t channel){
	int data = 0;
	uint8_t len = 0;
	uint8_t mdbLoc = 0;
	char rData[4] = {0,0,0,0};

	mdbLoc = baseSlot[slot-1].dbLoc;
	len = mdb[mdbLoc].aiBytes;

	if((slot < 1) || (slot > NUMBER_OF_MODULES)){
		#ifdef DEBUG_PRINT_ON
			// debugPrint("Slots must be between 1 and ");
			// debugPrintln(NUMBER_OF_MODULES);
		#endif
		return 0;
	}

	if((len <= 0)){
		#ifdef DEBUG_PRINT_ON
			// debugPrint("Slot ");
			// debugPrint(slot);
			// debugPrintln(": This module has no Analog Input bytes");
		#endif
		return 0;
	}

	if((channel <= 0) || (channel > (len / 4))){		//4 bytes per channel
		#ifdef DEBUG_PRINT_ON
			// debugPrintln("This channel is not valid");
		#endif
		return 0;
	}

	rData[0] = READ_ANALOG_HDR;
	rData[1] = slot;
	rData[2] = channel;
	spiSendRecvBuf((uint8_t *)rData,3);

	if(spiTimeout(1000*200) == true){
		data = spiSendRecvInt(DUMMY);
		dataSync();
		return data;
	}
	else{
		#ifdef DEBUG_PRINT_ON
			// debugPrintln("Slow read");
		#endif
		delay(100);
		return 0;
	}
}

float readTemperature(uint8_t slot, uint8_t channel){
	char log_msg[1000];
	union int2float{
		int data;			//We get an int back no matter what analog module. Unions set variables as the same/
		float temperature;	//Floats and Ints are both 32 bits, so this lets quickly convert our int to a float.
	}ourValue;

	ourValue.data = readAnalog(slot,channel);	//Use the same analog read function to get int
	#ifdef DEBUG_PRINT_ON
		// sprintf(log_msg, "Slot %d: Channel %d: %.2f\n", slot, channel, ourValue.temperature);
		// log(log_msg);
	#endif

	return ourValue.temperature;		//return the float
}

void writeAnalog(uint32_t data,uint8_t slot, uint8_t channel){
	uint8_t tData[7];
	uint8_t mdbLoc = 0;
	uint8_t len = 0;

	mdbLoc = baseSlot[slot-1].dbLoc;
	len = mdb[mdbLoc].aoBytes;

	if((slot < 1) || (slot > NUMBER_OF_MODULES)){
		#ifdef DEBUG_PRINT_ON
			// debugPrint("Slots must be between 1 and ");
			// debugPrintln(NUMBER_OF_MODULES);
		#endif
		return;
	}

	if((len <= 0)){
		#ifdef DEBUG_PRINT_ON
			// debugPrint("Slot ");
			// debugPrint(slot);
			// debugPrintln(": This module has no Analog Output bytes");
		#endif
		return;
	}

	if((channel <= 0) || (channel > (len / 4))){		//4 bytes per channel
		#ifdef DEBUG_PRINT_ON
			// debugPrintln("This channel is not valid");
		#endif
		return;
	}

	tData[0] = WRITE_ANALOG_HDR;
	tData[1] = slot;
	tData[2] = channel;
	tData[3] = (data>>0)  & 0xFF;
	tData[4] = (data>>8)  & 0xFF;
	tData[5] = (data>>16) & 0xFF;
	tData[6] = (data>>24) & 0xFF;
	spiSendRecvBuf(tData,7);

	dataSync();
	return;
}

/************************************************************************
 *                    BASE CONTROLLER FUNCTIONS                         *
************************************************************************/

bool handleHDR(uint8_t HDR){

	while(!digitalRead(slaveAckPin));								// Wait for Base Controller to be out of base scanning
	spiSendRecvByte(HDR);										// Send intital Header to ping DMA

	return spiTimeout(MAX_TIMEOUT,HDR,2000);					// 1 if we got Base Controller ack, 0 if we took too long
}

bool configureModule(char cfgData[], uint8_t slot){
	uint8_t len = 0;
	uint8_t mdbLoc = 0;
	uint8_t cfgForSpi[66];

	mdbLoc = baseSlot[slot-1].dbLoc;
	len = mdb[mdbLoc].configBytes + 2;
	cfgForSpi[0] = CFG_HDR;
	cfgForSpi[1] = slot;

	memcpy(cfgForSpi+2,cfgData,len);
	delay(1);
	if((slot < 1) || (slot > NUMBER_OF_MODULES)){
		#ifdef DEBUG_PRINT_ON
			// debugPrint("Slots must be between 1 and ");
			// debugPrintln(NUMBER_OF_MODULES);
		#endif
		return 0;
	}

	if(len <= 0){
		#ifdef DEBUG_PRINT_ON
			// debugPrint("Slot ");
			// debugPrint(slot);
			// debugPrintln(": This module has no config Bytes");
		#endif
		return 0;
	}

	spiSendRecvBuf(cfgForSpi,len);
	delay(100);														// Additional time for Config to written
	dataSync();
	dataSync();
	return 1;
}

/*******************************************************************************
Overload of above function for const arrays
*******************************************************************************/
bool configureModule(const char cfgData[], uint8_t slot){
	
	return configureModule((char*)cfgData, slot);
	
}

char *loadConfigBuf(int moduleID){

	switch(moduleID){
		case 0x34608CE1:
			return (char*)SLM_THM_4_DEFAULT_CONFIG;
		case 0x34A055EA:
			return (char*)SLM_AI_8_mA_DEFAULT_CONFIG;
		case 0x34A055EB:
			return (char*)SLM_AI_8_V_DEFAULT_CONFIG;
		case 0x1403F4E1:
			return (char*)SLM_PWM_4_DEFAULT_CONFIG;
		default:
			break;
	}
	return (char*)SLM_AI_4_mA_DEFAULT_CONFIG;
}

void enableBaseController(bool state){

	digitalWrite(baseEnable, state);

}

uint8_t printModules(){
	char log_msg[1000];
	uint32_t slot = 0;
	uint8_t dbLoc = 0;
	uint8_t goodSlots = 0;

	while(baseSlot[slot].dbLoc != 0 && slot < NUMBER_OF_MODULES){
		dbLoc = baseSlot[slot].dbLoc;
		if(mdb[dbLoc].moduleID == 0 || mdb[dbLoc].moduleID == 0xFFFFFFFF){
			#ifdef DEBUG_PRINT_ON
				sprintf(log_msg, "slot %d: Sign On Error\n", slot + 1);
				log(log_msg);
			#endif
		}
		else{
			#ifdef DEBUG_PRINT_ON
				sprintf(log_msg, "slot %d: %s\n", slot + 1, mdb[dbLoc].moduleName);
				log(log_msg);
			#endif
			goodSlots++;
		}
		slot++;
	}
	dataSync();
	return goodSlots;
}

/************************************************************************
 *                BASE INITIALIZATION FUNCTION                          *
************************************************************************/
uint8_t initHardware() {
	char log_msg[1000];
	uint32_t slots = 0;
	int dbLoc = 0;
	char *cfgArray;
	const uint8_t max_slots = 15;
	
	union moduleIDs{												// Use a union as a quick convert between byte arrays and ints
		uint32_t IDs[max_slots];
		uint8_t byteArray[max_slots * 4];
	}modules;
	uint8_t retry = 0;

	digitalWrite(slaveSelectPin, HIGH);
	memset(baseSlot,0,sizeof(baseSlot));							// Clear base constants array
	enableBaseController(HIGH);								// Start base controller
	delay(100);

	if(spiTimeout(1000*5000) == false){
		#ifdef DEBUG_PRINT_ON
			sprintf(log_msg, "No Base Controller Activity.\nCheck External Supply Connection.\n");
			log(log_msg);
		#endif
		delay(1000);
		return 0;
	}

	while(((slots == 0) || (slots > 15)) && (retry < 5)){			// Begin sign-on until number of slots are valid. Only retry 5 times.
		if(handleHDR(MOD_HDR)){
			delay(5);
			slots = spiSendRecvByte(DUMMY);					// Get number of modules in base
			if(slots == 0 || slots > 15){
				if(retry > 2){
					enableBaseController(LOW);					// Disable base controller
					delay(10);
					enableBaseController(HIGH);				// Start base controller
					delay(10);
				}
				retry++;											// Let Base Controller retry
			}
		}
	}

	if(retry >= 5){													// Zero module in the base. Quit sign-on routine and let the user know
		#ifdef DEBUG_PRINT_ON
			sprintf(log_msg, "Zero modules in the base.\n");
			log(log_msg);
		#endif
		delay(500);
		return 0;
	}
	
	if(slots >  NUMBER_OF_MODULES){
		#ifdef DEBUG_PRINT_ON
			sprintf(log_msg, "\nToo many modules in Base\nDevice only supports %d\n", NUMBER_OF_MODULES);
			log(log_msg);
		#endif
		memset(baseSlot, 0, sizeof(baseSlot));						// Clear base constants array
		return 0;
	}

	spiTimeout(1000*200);
	spiSendRecvBuf(modules.byteArray,slots*4,1);				// Slots * 4 bytes per ID code

	uint8_t baseControllerConstants[1 * max_slots * 7];			// Seven elements in module sign on
	for(uint32_t i=0;i<slots;i++){
		dbLoc = 0;
		while (modules.IDs[i] != mdb[dbLoc].moduleID){				// Scan MDB for matching ID and grab its array location
			if(mdb[dbLoc].moduleID == 0xFFFFFFFF){
				#ifdef DEBUG_PRINT_ON
					// debugPrintln("Module is not in Module List");	// If we got here, we probably need to update the library.
				#endif
				break;
			}
			dbLoc++;
		}
		baseSlot[i].dbLoc = dbLoc;									// MDB Location

		//Grab MDB values and load them into variables for Synergy and array to send to Base Controller
		baseControllerConstants[0+i*7]  = mdb[dbLoc].diBytes;
		baseControllerConstants[1+i*7]  = mdb[dbLoc].doBytes;
		baseControllerConstants[2+i*7]  = mdb[dbLoc].aiBytes;
		baseControllerConstants[3+i*7]  = mdb[dbLoc].aoBytes;
		baseControllerConstants[4+i*7]  = mdb[dbLoc].statusBytes;
		baseControllerConstants[5+i*7]  = mdb[dbLoc].configBytes;
		baseControllerConstants[6+i*7]  = mdb[dbLoc].dataSize;
	}

	spiTimeout(1000*200);
	delay(1);
	spiSendRecvBuf(baseControllerConstants,slots*7);			//Send mdb values to Base Controller
	delay(10);

	#ifndef AUTO_CONFIG_OFF
	for (uint32_t i = 0; i < slots; i++){ 							// Default config routine
		dbLoc = baseSlot[i].dbLoc;
		if(mdb[dbLoc].configBytes > 0){								// Modules with config Bytes need to havea config loaded
			cfgArray = loadConfigBuf(mdb[dbLoc].moduleID);		// Get pointer to default config for this module
			while(!configureModule(cfgArray, i + 1)){			// configure module
				#ifdef DEBUG_PRINT_ON
					// debugPrintln("Working");
				#endif
			}
		}
	}
	#endif

	delay(50);														// Let the Base Controller complete its end of the sign-on
	#ifdef DEBUG_PRINT_ON
		printModules();										// Returns the number of good modules. Prints IDs to serial monitor
	#endif
	return slots;
}

/************************************************************************
 *                               Open PLC                               *
************************************************************************/
//-----------------------------------------------------------------------------
// Functions called by the main OpenPLC routine
//-----------------------------------------------------------------------------
void initializeHardware()
{
	// char log_msg[1000];
 	// sprintf(log_msg, "Synergy: INITIALIZE HARDWARE\n");
 	// log(log_msg);
}

void finalizeHardware()
{
}

void updateBuffersIn()
{
	pthread_mutex_lock(&bufferLock); //lock mutex
	pthread_mutex_unlock(&bufferLock); //unlock mutex
}

void updateBuffersOut()
{
	pthread_mutex_lock(&bufferLock); //lock mutex
	pthread_mutex_unlock(&bufferLock); //unlock mutex
}