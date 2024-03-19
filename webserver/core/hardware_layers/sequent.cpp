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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <poll.h>

#include "ladder.h"
#include "custom_layer.h"

#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#define I2C_SLAVE	0x0703
#define I2C_SMBUS	0x0720	/* SMBus-level access */

#define I2C_SMBUS_READ	1
#define I2C_SMBUS_WRITE	0

// SMBus transaction types

#define I2C_SMBUS_QUICK		    0
#define I2C_SMBUS_BYTE		    1
#define I2C_SMBUS_BYTE_DATA	    2
#define I2C_SMBUS_WORD_DATA	    3
#define I2C_SMBUS_PROC_CALL	    4
#define I2C_SMBUS_BLOCK_DATA	    5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7		/* SMBus 2.0 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

// SMBus messages

#define I2C_SMBUS_BLOCK_MAX	32	/* As specified in SMBus standard */
#define I2C_SMBUS_I2C_BLOCK_MAX	32	/* Not specified but we use same structure */

//#define I2C_DEBUG 1 //todo: remove for production
#define GEN_DEBUG 1

#define OK 0
#define ERROR -1

#define STACK_LEVELS 8

//#define GEN_DEBUG 1
//#define I2C_DEBUG 1

#define LOG_ERROR	log_error(__LINE__)

int log_error(int line)
{
	char log_msg[1000];

#ifdef GEN_DEBUG
	sprintf(log_msg, "!line %d\n", line);
	log(log_msg);
#endif
	return ERROR;
}

int i2cSetup(int addr)
{
	static int file = -1;
	char filename[32];
	char log_msg[1000];

	if (file < 0)
	{
		sprintf(filename, "/dev/i2c-1");

		if ( (file = open(filename, O_RDWR)) < 0)
		{
			sprintf(log_msg, "Failed to open the bus.\n");
			log(log_msg);

			return ERROR;
		}
	}
	if (ioctl(file, I2C_SLAVE, addr) < 0)
	{
		sprintf(log_msg, "Failed to acquire bus access and/or talk to slave.\n");
		log(log_msg);
		return ERROR;
	}
	return file;
}

int i2cMemRead(int dev, int add, uint8_t *buff, int size)
{
	uint8_t intBuff[I2C_SMBUS_BLOCK_MAX];
	unsigned char log_msg[1000];

	if (NULL == buff)
	{
		return ERROR;
	}

	if (size > I2C_SMBUS_BLOCK_MAX)
	{
		return ERROR;
	}

	intBuff[0] = 0xff & add;

	if (write(dev, intBuff, 1) != 1)
	{
#ifdef I2C_DEBUG
		sprintf(log_msg, "Fail to select 0x%02hhx mem add!\n", add);
		log(log_msg);
#endif
		return ERROR;
	}
	if (read(dev, buff, size) != size)
	{
#ifdef I2C_DEBUG
		sprintf(log_msg, "Fail to read memory!\n");
		log(log_msg);
#endif
		return ERROR;
	}
	return OK; //OK
}

int i2cMemWrite(int dev, int add, uint8_t *buff, int size)
{
	uint8_t intBuff[I2C_SMBUS_BLOCK_MAX];
	char log_msg[1000];

	if (NULL == buff)
	{
		return ERROR;
	}

	if (size > I2C_SMBUS_BLOCK_MAX - 1)
	{
		return ERROR;
	}

	intBuff[0] = 0xff & add;
	memcpy(&intBuff[1], buff, size);

	if (write(dev, intBuff, size + 1) != size + 1)
	{
		sprintf(log_msg, "Fail to write memory at 0x%02hhx address!\n", add);
		log(log_msg);
		return ERROR;
	}
	return OK;
}

//------------------------------------------------------------------------------
// Eight Relays 8-Layer Stackable HAT for Raspberry Pi
//------------------------------------------------------------------------------

#define RELAY8_HW_I2C_BASE_ADD 0x20
#define RELAY8_HW_I2C_BASE_ALT_ADD 0x38
#define RELAY8_INPORT_REG_ADD	0x00
#define RELAY8_OUTPORT_REG_ADD	0x01
#define RELAY8_POLINV_REG_ADD	0x02
#define RELAY8_CFG_REG_ADD		0x03
#define RELAY8_REG_COUNT 0x04

#define RELAY8_SL_START 0
#define RELAY8_SL_COUNT 8

const uint8_t relay8MaskRemap[8] = {0x01, 0x04, 0x40, 0x10, 0x20, 0x80, 0x08,
	0x02};

uint8_t relay8ToIO(uint8_t relay)
{
	uint8_t i;
	uint8_t val = 0;
	for (i = 0; i < 8; i++)
	{
		if ( (relay & (1 << i)) != 0)
			val += relay8MaskRemap[i];
	}
	return val;
}

uint8_t IOToRelay8(uint8_t io)
{
	uint8_t i;
	uint8_t val = 0;
	for (i = 0; i < 8; i++)
	{
		if ( (io & relay8MaskRemap[i]) != 0)
		{
			val += 1 << i;
		}
	}
	return val;
}

int relay8CardCheck(int stack)
{
	uint8_t add = 0;

	if ( (stack < 0) || (stack > 7))
	{
		//printf("Invalid stack level [0..7]!");
		return LOG_ERROR;
	}
	add = (stack + RELAY8_HW_I2C_BASE_ADD) ^ 0x07;
	return i2cSetup(add);
}

int relay8Init(int stack)
{
	int dev = -1;
	uint8_t add = 0;
	uint8_t buff[2];

	dev = relay8CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (ERROR == i2cMemRead(dev, RELAY8_CFG_REG_ADD, buff, 1))
	{
		return ERROR;
	}
	if (OK == i2cMemRead(dev, RELAY8_REG_COUNT, buff, 1)) //16 bits I/O expander found
	{
		return ERROR;
	}
	if (buff[0] != 0) //non initialized I/O Expander
	{
		// make all I/O pins output
		buff[0] = 0;
		if (OK > i2cMemWrite(dev, RELAY8_CFG_REG_ADD, buff, 1))
		{
			return LOG_ERROR;
		}
		// put all pins in 0-logic state
		buff[0] = 0;
		if (OK > i2cMemWrite(dev, RELAY8_OUTPORT_REG_ADD, buff, 1))
		{
			return LOG_ERROR;
		}
	}
	//relay8_presence |= 1 << stack;
	return OK;
}

int relays8Set(uint8_t stack, uint8_t val)
{
	uint8_t buff[2];
	int dev = -1;
	static uint8_t relaysOldVal[STACK_LEVELS] = {0, 0, 0, 0, 0, 0, 0, 0};

	if (stack >= STACK_LEVELS)
	{
		return LOG_ERROR;
	}

	if (relaysOldVal[stack] == val)
	{
		return OK;
	}
	dev = relay8CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	buff[0] = relay8ToIO(val);

	if (OK != i2cMemWrite(dev, RELAY8_OUTPORT_REG_ADD, buff, 1))
	{
		return LOG_ERROR;
	}
	relaysOldVal[stack] = val;
	return OK;
}

//------------------------------------------------------------------------------
// Sixteen Relays 8-Layer Stackable HAT for Raspberry Pi
//-----------------------------------------------------------------------------

#define RELAY16_CHANNELS 16
#define RELAY16_HW_I2C_BASE_ADD	0x20
#define RELAY16_INPORT_REG_ADD	0x00
#define RELAY16_OUTPORT_REG_ADD	0x02
#define RELAY16_POLINV_REG_ADD	0x04
#define RELAY16_CFG_REG_ADD		0x06

#define RELAY16_X_PLC_OFFSET 64
#define RELAY16_STACK_MIN 0
#define RELAY16_STACK_LEVELS 4

const uint16_t relayMaskRemap16[RELAY16_CHANNELS] = {0x8000, 0x4000, 0x2000,
	0x1000, 0x800, 0x400, 0x200, 0x100, 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2,
	0x1};

uint16_t relayToIO16(uint16_t relay)
{
	uint8_t i;
	uint16_t val = 0;
	for (i = 0; i < 16; i++)
	{
		if ( (relay & (1 << i)) != 0)
			val += relayMaskRemap16[i];
	}
	return val;
}

uint16_t IOToRelay16(uint16_t io)
{
	uint8_t i;
	uint16_t val = 0;
	for (i = 0; i < 16; i++)
	{
		if ( (io & relayMaskRemap16[i]) != 0)
		{
			val += 1 << i;
		}
	}
	return val;
}

int relay16CardCheck(uint8_t stack)
{
	uint8_t add = 0;

	if ( (stack < 0) || (stack > 7))
	{
		//printf("Invalid stack level [0..7]!");
		return LOG_ERROR;
	}
	add = (stack + RELAY16_HW_I2C_BASE_ADD) ^ 0x07;
	return i2cSetup(add);
}

int relay16Init(int stack)
{
	int dev = -1;
	uint8_t add = 0;
	uint8_t buff[2];
	uint16_t val = 0;

	dev = relay16CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (ERROR == i2cMemRead(dev, RELAY16_CFG_REG_ADD, buff, 2)) // 16 bits IO expander found
	{
		return ERROR;
	}
	memcpy(&val, buff, 2);
	if (val != 0) //non initialized I/O Expander
	{
		// make all I/O pins output
		val = 0;
		memcpy(buff, &val, 2);
		if (OK > i2cMemWrite(dev, RELAY16_CFG_REG_ADD, buff, 2))
		{
			return LOG_ERROR;
		}
		// put all pins in 0-logic state
		if (OK > i2cMemWrite(dev, RELAY16_OUTPORT_REG_ADD, buff, 2))
		{
			return LOG_ERROR;
		}
	}
	return OK;
}

int relay16Set(uint8_t stack, uint16_t val)
{
	static uint16_t relaysOldVal[STACK_LEVELS] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t buff[2];
	uint16_t rVal = 0;
	int dev = -1;

	if (stack >= STACK_LEVELS)
	{
		return LOG_ERROR;
	}
	if (relaysOldVal[stack] == val)
	{
		return OK;
	}
	dev = relay16CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	rVal = relayToIO16(val);
	memcpy(buff, &rVal, 2);

	if (OK == i2cMemWrite(dev, RELAY16_OUTPORT_REG_ADD, buff, 2))
	{
		relaysOldVal[stack] = val;
		return OK;
	}
	return LOG_ERROR;
}


//-----------------------------------------------------------------------------
// Eight High Voltage Digital Inputs 8-Layer Stackable HAT for Raspberry Pi
//-----------------------------------------------------------------------------
#define DIG_IN8_CHANNELS 8
#define DIG_IN8_HW_I2C_BASE_ADD	0x20
#define DIG_IN8_INPORT_REG_ADD	0x00
#define DIG_IN8_OUTPORT_REG_ADD	0x01
#define DIG_IN8_POLINV_REG_ADD	0x02
#define DIG_IN8_CFG_REG_ADD		0x03

const uint8_t inputsMaskRemap8[DIG_IN8_CHANNELS] ={0x08, 0x04, 0x02, 0x01, 0x10, 0x20, 0x40, 0x80};

int digIn8CardCheck(uint8_t stack)
{
	uint8_t add = 0;

	if ( (stack < 0) || (stack > 7))
	{
		//printf("Invalid stack level [0..7]!");
		return LOG_ERROR;
	}
	add = (stack + DIG_IN8_HW_I2C_BASE_ADD) ^ 0x07;
	return i2cSetup(add);
}

int digIn8Init(int stack)
{
	int dev = -1;
	uint8_t add = 0;
	uint8_t buff[2];
	uint8_t val = 0;

	dev = digIn8CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (ERROR == i2cMemRead(dev, DIG_IN8_CFG_REG_ADD, buff, 1))
	{
		return ERROR;
	}
	
	if (buff[0] != 0xff) //non initialized I/O Expander
	{
		// make all I/O pins inputs
		buff[0] = 0xff;
		if (OK != i2cMemWrite(dev, RELAY8_CFG_REG_ADD, buff, 1))
		{
			return LOG_ERROR;
		}
	}
	return OK;
}

int digIn8Get(uint8_t stack, uint8_t *val)
{
	int dev = -1;
	uint8_t buff[2];
	uint8_t raw = 0;
	uint8_t i = 0;

	if (stack >= STACK_LEVELS || val == NULL)
	{
		return LOG_ERROR;
	}
	dev = digIn8CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (OK != i2cMemRead(dev, DIG_IN8_INPORT_REG_ADD, buff, 1))
	{
		return LOG_ERROR;
	}
	raw = 0xff & (~buff[0]);
	*val = 0;
	for (i = 0; i < 8; i++)
	{
		if (raw & inputsMaskRemap8[i])
		{
			*val += 1 << i;
		}
	}
	return OK;
}


//------------------------------------------------------------------------------
// Sixteen Digital Inputs 8-Layer Stackable HAT for Raspberry Pi
//------------------------------------------------------------------------------
#define DIG_IN16_CHANNELS 16
#define DIG_IN16_HW_I2C_BASE_ADD	0x20
#define DIG_IN16_INPORT_REG_ADD	0x00
#define DIG_IN16_OUTPORT_REG_ADD	0x02
#define DIG_IN16_POLINV_REG_ADD	0x04
#define DIG_IN16_CFG_REG_ADD		0x06

#define DIG_IN16_X_PLC_OFFSET 0

#define DIG_IN16_STACK_MIN 4
#define DIG_IN16_STACK_LEVELS 4

const uint16_t inputsMaskRemap16[DIG_IN16_CHANNELS] = {0x8000, 0x4000, 0x2000,
	0x1000, 0x800, 0x400, 0x200, 0x100, 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2,
	0x1};

int digIn16CardCheck(uint8_t stack)
{
	uint8_t add = 0;

	if ( (stack < 0) || (stack > 7))
	{
		//printf("Invalid stack level [0..7]!");
		return LOG_ERROR;
	}
	add = (stack + DIG_IN16_HW_I2C_BASE_ADD) ^ 0x07;
	return i2cSetup(add);
}

int digIn16Init(int stack)
{
	int dev = -1;
	uint8_t add = 0;
	uint8_t buff[2];
	uint16_t val = 0;

	dev = digIn16CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (ERROR == i2cMemRead(dev, DIG_IN16_CFG_REG_ADD, buff, 2))
	{
		return ERROR;
	}
	memcpy(&val, buff, 2);
	if (val != 0xffff) //non initialized I/O Expander
	{
		// make all I/O pins inputs
		val = 0xffff;
		memcpy(buff, &val, 2);
		if (OK != i2cMemWrite(dev, RELAY16_CFG_REG_ADD, buff, 2))
		{
			return LOG_ERROR;
		}
	}
	return OK;
}

int digIn16Get(uint8_t stack, uint16_t *val)
{
	int dev = -1;
	uint8_t buff[2];
	uint16_t raw = 0;
	uint8_t i = 0;

	if (stack >= STACK_LEVELS || val == NULL)
	{
		return LOG_ERROR;
	}
	dev = digIn16CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (OK != i2cMemRead(dev, DIG_IN16_INPORT_REG_ADD, buff, 2))
	{
		return LOG_ERROR;
	}
	raw = (0xff & (~buff[0])) + ( (0xff & (~buff[1])) << 8);
	*val = 0;
	for (i = 0; i < 16; i++)
	{
		if (raw & inputsMaskRemap16[i])
		{
			*val += 1 << i;
		}
	}
	return OK;
}

//------------------------------------------------------------------------------
// Four Relays four HV Inputs 8-Layer Stackable HAT for Raspberry Pi
//------------------------------------------------------------------------------

#define I2C_REL4_IN4_ADDRESS_BASE 0x0e

#define R4I4_I2C_MEM_RELAY_VAL  0//reserved 4 bits for open-drain and 4 bits for leds
#define R4I4_I2C_MEM_DIG_IN 3
#define R4I4_I2C_MEM_AC_IN 4
#define R4I4_I2C_MEM_PWM_IN_FILL 45
#define R4I4_I2C_MEM_IN_FREQENCY  53
#define R4I4_I2C_MEM_BUTTON 71
#define R4I4_I2C_MEM_REVISION_MAJOR_ADD 120

#define R4I4_CHANNEL_NR_MIN		1

#define R4I4_OPTO_CH_NR_MAX		4
#define R4I4_REL_CH_NR_MAX			4

#define R4I4_REL_PLC_OFFSET_BITS 0
#define R4I4_OPTO_IN_PLC_OFFSET_BITS 0
#define R4I4_AC_IN_PLC_OFFSET_BITS 4
#define R4I4_PWM_IN_FILL_PLC_OFFSET 0
#define R4I4_PWM_IN_FREQ_PLC_OFFSET 4
#define R4I4_BUTTON_PLC_OFFSET 9

int r4i4CardCheck(uint8_t stack)
{
	uint8_t add = 0;

	if ( (stack < 0) || (stack > 7))
	{
		return LOG_ERROR;
	}
	add = stack + I2C_REL4_IN4_ADDRESS_BASE;
	return i2cSetup(add);
}

int r4i4Init(int stack)
{
	int dev = -1;
	uint8_t add = 0;
	uint8_t buff[2];
	uint16_t val = 0;

	dev = r4i4CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (ERROR == i2cMemRead(dev, R4I4_I2C_MEM_REVISION_MAJOR_ADD, buff, 2))
	{
		return ERROR;
	}
	return OK;
}

int r4i4SetRelays(uint8_t stack, uint8_t value)
{
	static uint8_t prevRelays[STACK_LEVELS] = {0, 0, 0, 0, 0, 0, 0, 0};
	int dev = -1;

	if (stack >= STACK_LEVELS)
	{
		return LOG_ERROR;
	}
	if (prevRelays[stack] == value)
	{
		return OK; // prevent usless transactions on I2C bus
	}
	dev = r4i4CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (ERROR == i2cMemWrite(dev, R4I4_I2C_MEM_RELAY_VAL, &value, 1))
	{
		return LOG_ERROR;
	}
	prevRelays[stack] = value;
	return OK;
}

int r4i4GetOptoInputs(uint8_t stack, uint8_t *val)
{
	int dev = -1;

	if (val == NULL)
	{
		return LOG_ERROR;
	}
	dev = r4i4CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	return i2cMemRead(dev, R4I4_I2C_MEM_DIG_IN, val, 1);
}

int r4i4GetACInputs(uint8_t stack, uint8_t *val)
{
	int dev = -1;

	if (val == NULL)
	{
		return LOG_ERROR;
	}
	dev = r4i4CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	return i2cMemRead(dev, R4I4_I2C_MEM_AC_IN, val, 1);
}

int r4i4GetButton(uint8_t stack, uint8_t *button)
{
	int dev = -1;
	uint8_t val;

	if (button == NULL)
	{
		return LOG_ERROR;
	}
	dev = r4i4CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	if (OK != i2cMemRead(dev, R4I4_I2C_MEM_BUTTON, &val, 1))
	{
		return LOG_ERROR;
	}
	*button = 0x01 & val;
	return OK;
}

int r4i4GetPWMInFill(uint8_t stack, uint8_t channel, uint16_t *value)
{
	int dev = -1;
	uint8_t buff[2];

	if (channel >= R4I4_OPTO_CH_NR_MAX || value == NULL)
	{
		return LOG_ERROR;
	}

	dev = r4i4CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	if (OK != i2cMemRead(dev, R4I4_I2C_MEM_PWM_IN_FILL + 2 * channel, buff, 2))
	{
		return LOG_ERROR;
	}
	memcpy(value, buff, 2);
	return OK;
}

int r4i4GetPWMInFreq(uint8_t stack, uint8_t channel, uint16_t *value)
{
	int dev = -1;
	uint8_t buff[2];

	if (channel >= R4I4_OPTO_CH_NR_MAX || value == NULL)
	{
		return LOG_ERROR;
	}

	dev = r4i4CardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	if (OK != i2cMemRead(dev, R4I4_I2C_MEM_IN_FREQENCY + 2 * channel, buff, 2))
	{
		return LOG_ERROR;
	}
	memcpy(value, buff, 2);
	return OK;
}


//------------------------------------------------------------------------------
// RTD Data Acquisition 8-Layer Stackable HAT for Raspberry Pi
//------------------------------------------------------------------------------
#define I2C_RTD_ADDRESS_BASE 0x40

#define RTD_I2C_VAL1_ADD  0//reserved 4 bits for open-drain and 4 bits for leds
#define RTD_I2C_MEM_REVISION_MAJOR_ADD 57

#define RTD_CHANNEL_NR_MIN		1
#define RTD_CH_NR_MAX		8

#define RTD_TEMP_SIZE 4

#define RTD_TEMP_PLC_SCALE_FACTOR ((float)10)
#define RTD_TEMP_PLC_OFFSET ((float)200)

#define RTD_TEMP_IW_ADDR_OFFSET  0
#define RTD_IW_PER_CARD 12

int rtdCardCheck(uint8_t stack)
{
	uint8_t add = 0;

	if ( (stack < 0) || (stack > 7))
	{
		return LOG_ERROR;
	}
	add = stack + I2C_RTD_ADDRESS_BASE;
	return i2cSetup(add);
}

int rtdInit(int stack)
{
	int dev = -1;
	uint8_t add = 0;
	uint8_t buff[2];
	uint16_t val = 0;

	dev = rtdCardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (ERROR == i2cMemRead(dev, RTD_I2C_MEM_REVISION_MAJOR_ADD, buff, 2))
	{
		return ERROR;
	}

	return OK;
}

int rtdGetTemp(uint8_t stack, uint8_t channel, float *value)
{
	int dev = -1;
	uint8_t buff[4];
	
	if (channel >= RTD_CH_NR_MAX || value == NULL)
	{
		return LOG_ERROR;
	}

	dev = rtdCardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	if (OK != i2cMemRead(dev, RTD_I2C_VAL1_ADD + 4 * channel, buff, 4))
	{
		return LOG_ERROR;
	}
	memcpy(value, buff, 4);
	return OK;
}


//------------------------------------------------------------------------------
// Industrial Automation 8-Layer Stackable HAT for Raspberry Pi
//------------------------------------------------------------------------------

#define I2C_IND_ADDRESS_BASE 0x50

#define IND_I2C_MEM_RELAY_VAL 0//reserved 4 bits for open-drain and 4 bits for leds
#define IND_I2C_MEM_OPTO_IN_VAL 3
#define IND_I2C_MEM_U0_10_OUT_VAL 4
#define IND_I2C_MEM_I4_20_OUT_VAL 12
#define IND_I2C_MEM_OD_PWM 20
#define IND_I2C_MEM_U0_10_IN_VAL 28
#define IND_I2C_MEM_U_PM_10_IN_VAL 36
#define IND_I2C_MEM_I4_20_IN_VAL 44
#define IND_I2C_MEM_REVISION_MAJOR  0x78
#define IND_I2C_MEM_1WB_START_SEARCH 173
#define IND_I2C_MEM_1WB_T1 174

#define CHANNEL_NR_MIN		1

#define IND_OPTO_CH_NR_MAX		4
#define IND_OD_CH_NR_MAX			4
#define IND_I_OUT_CH_NR_MAX		4
#define IND_U_OUT_CH_NR_MAX		4
#define IND_U_IN_CH_NR_MAX		4
#define IND_I_IN_CH_NR_MAX		4
#define IND_LED_CH_NR_MAX 4
#define IND_OWB_CH_MAX 4 // (4/16) limit PLC variable alocation

#define IND_0_10V_RAW_MAX 10000
#define IND_4_20MA_RAW_MIN 4000
#define IND_4_20MA_RAW_MAX 20000
#define IND_OD_PWM_VAL_MAX	10000

#define IND_LED_PLC_OFFSET_BITS 0
#define IND_OPTO_IN_PLC_OFFSET_BITS 0
#define IND_U_IN_PLC_OFFSET 0
#define IND_I_IN_PLC_OFFSET 4
#define IND_T_PLC_OFFSET 8
#define IND_U_OUT_PLC_OFFSET 0
#define IND_I_OUT_PLC_OFFSET 4
#define IND_PWM_OUT_PLC_OFFSET 8

#define VOLT_TO_MILIVOLT ((float)1000)
#define MILI_TO_MICRO ((float)1000)
#define OWB_TEMP_SCALE ((float)100)
#define PWM_SCALE ((float)100)

int indCardCheck(uint8_t stack)
{
	uint8_t add = 0;

	if ( (stack < 0) || (stack > 7))
	{
		return LOG_ERROR;
	}
	add = stack + I2C_IND_ADDRESS_BASE;
	return i2cSetup(add);
}

int indInit(int stack)
{
	int dev = -1;
	uint8_t add = 0;
	uint8_t buff[2];
	uint16_t val = 0;

	dev = indCardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (ERROR == i2cMemRead(dev, IND_I2C_MEM_REVISION_MAJOR, buff, 2))
	{
		return ERROR;
	}
	return OK;
}

int indSetLeds(uint8_t stack, uint8_t value)
{
	static uint8_t prevLeds[STACK_LEVELS] = {0, 0, 0, 0, 0, 0, 0, 0};
	int dev = -1;
	uint8_t buff[2];
	if (stack >= STACK_LEVELS)
	{
		return LOG_ERROR;
	}
	if (prevLeds[stack] == value)
	{
		return OK; // prevent usless transactions on I2C bus
	}
	dev = indCardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	buff[0] = 0xf0 & (value << 4);
	if (ERROR == i2cMemWrite(dev, IND_I2C_MEM_RELAY_VAL, buff, 1))
	{
		return LOG_ERROR;
	}
	prevLeds[stack] = value;
	return OK;
}

int indGetOptoInputs(uint8_t stack, uint8_t *val)
{
	int dev = -1;

	if (val == NULL)
	{
		return LOG_ERROR;
	}
	dev = indCardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	return i2cMemRead(dev, IND_I2C_MEM_OPTO_IN_VAL, val, 1);
}

int indGet0_10Vin(uint8_t stack, uint8_t channel, float *val)
{
	uint8_t buff[2];
	int dev = -1;
	uint16_t aux16 = 0;

	if (channel > IND_U_IN_CH_NR_MAX || NULL == val)
	{
		return LOG_ERROR;
	}
	dev = indCardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (OK != i2cMemRead(dev, IND_I2C_MEM_U0_10_IN_VAL + 2 * channel, buff, 2))
	{
		return LOG_ERROR;
	}
	memcpy(&aux16, buff, 2);
	*val = (float)aux16 / VOLT_TO_MILIVOLT;
	return OK;
}

int indGet4_20mAin(uint8_t stack, uint8_t channel, float *val)
{
	uint8_t buff[2];
	int dev = -1;
    uint16_t aux16 = 0;

	if (channel > IND_I_IN_CH_NR_MAX || NULL == val)
	{
		return LOG_ERROR;
	}
	dev = indCardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}

	if (OK != i2cMemRead(dev, IND_I2C_MEM_I4_20_IN_VAL + 2 * channel, buff, 2))
	{
		return LOG_ERROR;
	}
	memcpy(&aux16, buff, 2);
	*val = (float)aux16 / MILI_TO_MICRO;
	return OK;
}

int indGet1WbTemp(uint8_t stack, uint8_t channel, float *val)
{
	uint8_t buff[2];
	int dev = -1;
	int16_t aux16 = 0;

	if (channel > IND_OWB_CH_MAX || NULL == val)
	{
		return LOG_ERROR;
	}
	dev = indCardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	if (OK != i2cMemRead(dev, IND_I2C_MEM_1WB_T1 + 2 * channel, buff, 2))
	{
		return LOG_ERROR;
	}
	memcpy(&aux16, buff, 2);
	*val = (float)aux16 / OWB_TEMP_SCALE;
	return OK;
}

int indSet0_10Vout(uint8_t stack, uint8_t channel, float val)
{
	uint8_t buff[2];
	int dev = -1;
	uint16_t aux16 = 0;

	val *= VOLT_TO_MILIVOLT;
	if (channel > IND_U_OUT_CH_NR_MAX || val > IND_0_10V_RAW_MAX)
	{
		return LOG_ERROR;
	}
	aux16 = (uint16_t)val;
	dev = indCardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	memcpy(buff, &aux16, 2);
	if (OK
		!= i2cMemWrite(dev, IND_I2C_MEM_U0_10_OUT_VAL + 2 * channel, buff, 2))
	{
		return LOG_ERROR;
	}

	return OK;
}

int indSet4_20mAout(uint8_t stack, uint8_t channel, float val)
{
	uint8_t buff[2];
	int dev = -1;
	uint16_t aux16 = 0;

	val *= MILI_TO_MICRO;
	if (channel > IND_I_OUT_CH_NR_MAX || val > IND_4_20MA_RAW_MAX
		|| val < IND_4_20MA_RAW_MIN)
	{
		return ERROR;
		//return LOG_ERROR;
	}
	aux16 = (uint16_t)val;
	dev = indCardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	memcpy(buff, &aux16, 2);
	if (OK
		!= i2cMemWrite(dev, IND_I2C_MEM_I4_20_OUT_VAL + 2 * channel, buff, 2))
	{
		return LOG_ERROR;
	}

	return OK;
}

int indSetPWMout(uint8_t stack, uint8_t channel, float val)
{
	uint8_t buff[2];
	int dev = -1;
	uint16_t aux16 = 0;

	if (channel > IND_OD_CH_NR_MAX || val > 100 || val < 0)
	{
		return LOG_ERROR;
	}
	aux16 = (uint16_t)(val * PWM_SCALE);
	dev = indCardCheck(stack);
	if (dev < 0)
	{
		return LOG_ERROR;
	}
	memcpy(buff, &aux16, 2);
	if (OK != i2cMemWrite(dev, IND_I2C_MEM_OD_PWM + 2 * channel, buff, 2))
	{
		return LOG_ERROR;
	}

	return OK;
}

//------------------------------------------------------------------------------
// Building  Automation 8-Layer Stackable HAT for Raspberry Pi
//------------------------------------------------------------------------------
#define I2C_BAS_ADDRESS_BASE 0x48

#define BAS_I2C_TRIACS_VAL_ADD 0
#define BAS_I2C_DRY_CONTACT_VAL_ADD 3
#define BAS_I2C_U0_10_OUT_VAL1_ADD 4
#define BAS_I2C_U0_10_IN_VAL1_ADD 12
#define BAS_I2C_R_1K_CH1 28
#define BAS_I2C_R_10K_CH1 44
#define BAS_I2C_MEM_1WB_T1 174
#define BAS_I2C_REVISION_MAJOR_MEM_ADD 122

#define BAS_U_IN_CH_NR_MAX 8
#define BAS_U_OUT_CH_NR_MAX 4
#define BAS_VOLT_MAX 10



int basCardCheck(uint8_t stack)
{
	uint8_t add = 0;

	if ( (stack < 0) || (stack > 7))
	{
		return ERROR;
	}
	add = stack + I2C_BAS_ADDRESS_BASE;
	return i2cSetup(add);
}

int basInit(int stack)
{
	int dev = -1;
	uint8_t add = 0;
	uint8_t buff[2];
	uint16_t val = 0;

	dev = basCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}

	if (ERROR == i2cMemRead(dev, BAS_I2C_REVISION_MAJOR_MEM_ADD, buff, 2))
	{
		return ERROR;
	}

	return OK;
}

int basSetTriacs(uint8_t stack, uint8_t value)
{
	static uint8_t prevTriacs[STACK_LEVELS] = {0, 0, 0, 0, 0, 0, 0, 0};
	int dev = -1;
	uint8_t buff[2];
	if (stack >= STACK_LEVELS)
	{
		return ERROR;
	}
	if (prevTriacs[stack] == value)
	{
		return OK; // prevent usless transactions on I2C bus
	}
	dev = basCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}

	buff[0] = 0x0f & value ;
	if (ERROR == i2cMemWrite(dev, BAS_I2C_TRIACS_VAL_ADD, buff, 1))
	{
		return ERROR;
	}
	prevTriacs[stack] = value;
	return OK;
}

int basGetDryContacts(uint8_t stack, uint8_t *val)
{
	int dev = -1;

	if (val == NULL)
	{
		return ERROR;
	}
	dev = basCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}
	return i2cMemRead(dev, BAS_I2C_DRY_CONTACT_VAL_ADD, val, 1);
}

int basGetUniversalIn(uint8_t stack, uint8_t channel, uint8_t type, float *val)
{
	uint8_t buff[2];
	int dev = -1;
	uint16_t aux16 = 0;
	uint8_t address = BAS_I2C_U0_10_IN_VAL1_ADD;
	float scaleFactor = VOLT_TO_MILIVOLT;

	if (channel >= BAS_U_IN_CH_NR_MAX || NULL == val)
	{
		return ERROR;
	}
	dev = basCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}
	switch( type) //default reads 0-10V inputs
	{
		case 1: //1k resistor
			address = BAS_I2C_R_1K_CH1;
			scaleFactor = 1;
			break;
		case 2: //10k resistor
			address = BAS_I2C_R_10K_CH1;
			scaleFactor = 1;
		break;
	}

	if (OK != i2cMemRead(dev, address + 2 * channel, buff, 2))
	{
		return ERROR;
	}
	memcpy(&aux16, buff, 2);
	*val = (float)aux16 / scaleFactor;
	return OK;
}

int basSet0_10Vout(uint8_t stack, uint8_t channel, float val)
{
	uint8_t buff[2];
	int dev = -1;
	uint16_t aux16 = 0;

	if (channel >= BAS_U_OUT_CH_NR_MAX || val > BAS_VOLT_MAX)
	{
		return ERROR;
	}
	aux16 = (uint16_t)(val * VOLT_TO_MILIVOLT);
	dev = basCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}
	memcpy(buff, &aux16, 2);
	if (OK
		!= i2cMemWrite(dev, BAS_I2C_U0_10_OUT_VAL1_ADD + 2 * channel, buff, 2))
	{
		return ERROR;
	}

	return OK;
}

int basGet1WbTemp(uint8_t stack, uint8_t channel, float *val)
{
	uint8_t buff[2];
	int dev = -1;
	int16_t aux16 = 0;

	if (channel > IND_OWB_CH_MAX || NULL == val)
	{
		return ERROR;
	}
	dev = basCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}
	if (OK != i2cMemRead(dev, BAS_I2C_MEM_1WB_T1 + 2 * channel, buff, 2))
	{
		return ERROR;
	}
	memcpy(&aux16, buff, 2);
	*val = (float)aux16 / OWB_TEMP_SCALE;
	return OK;
}

//------------------------------------------------------------------------------
// Home Automation 8-Layer Stackable HAT for Raspberry Pi
//------------------------------------------------------------------------------


#define I2C_HOME_ADDRESS_BASE 0x28

#define HOME_I2C_RELAY_ADD 0x00
#define HOME_I2C_0_10V_OUT_ADD 40
#define HOME_I2C_OD_ADD 48
#define HOME_I2C_OPTO_ADD 3
#define HOME_I2C_ADC_ADD 24
#define HOME_I2C_1W_TEMP_ADD 222
#define HOME_I2C_REVISION_MAJOR_ADD 122

#define HOME_ADC_CH_NO	8
#define HOME_DAC_CH_NO	4


int homeCardCheck(uint8_t stack)
{
	uint8_t add = 0;

	if ( (stack < 0) || (stack > 7))
	{
		return ERROR;
	}
	add = stack + I2C_HOME_ADDRESS_BASE;
	return i2cSetup(add);
}

int homeInit(int stack)
{
	int dev = -1;
	uint8_t add = 0;
	uint8_t buff[2];
	uint16_t val = 0;

	dev = homeCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}

	if (ERROR == i2cMemRead(dev, HOME_I2C_REVISION_MAJOR_ADD, buff, 2))
	{
		return ERROR;
	}

	return OK;
}

int homeSetRelays(uint8_t stack, uint8_t value)
{
	static uint8_t prevRelays[STACK_LEVELS] = {0, 0, 0, 0, 0, 0, 0, 0};
	int dev = -1;
	uint8_t buff[2];
	if (stack >= STACK_LEVELS)
	{
		return ERROR;
	}
	if (prevRelays[stack] == value)
	{
		return OK; // prevent usless transactions on I2C bus
	}
	dev = homeCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}

	buff[0] =  value ;
	if (ERROR == i2cMemWrite(dev, HOME_I2C_RELAY_ADD, buff, 1))
	{
		return ERROR;
	}
	prevRelays[stack] = value;
	return OK;
}


int homeSet0_10Vout(uint8_t stack, uint8_t channel, float val)
{
	uint8_t buff[2];
	int dev = -1;
	uint16_t aux16 = 0;

	if (channel >= HOME_DAC_CH_NO || val > BAS_VOLT_MAX)
	{
		return ERROR;
	}
	aux16 = (uint16_t)(val * VOLT_TO_MILIVOLT);
	dev = homeCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}
	memcpy(buff, &aux16, 2);
	if (OK
		!= i2cMemWrite(dev, HOME_I2C_0_10V_OUT_ADD + 2 * channel, buff, 2))
	{
		return ERROR;
	}

	return OK;
}

int homeSetOD(uint8_t stack, uint8_t channel, float val)
{
	uint8_t buff[2];
	int dev = -1;
	uint16_t aux16 = 0;

	if (channel >= HOME_DAC_CH_NO || val > 100 || val < 0)
	{
		return ERROR;
	}
	aux16 = (uint16_t)(val * 100);
	dev = homeCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}
	memcpy(buff, &aux16, 2);
	if (OK
		!= i2cMemWrite(dev, HOME_I2C_OD_ADD + 2 * channel, buff, 2))
	{
		return ERROR;
	}
	return OK;
}

int homeGetOpto(uint8_t stack, uint8_t* val)
{
	int dev = -1;

	if (val == NULL)
	{
		return ERROR;
	}
	dev = homeCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}
	return i2cMemRead(dev, HOME_I2C_OPTO_ADD, val, 1);
}

int homeGetADC(uint8_t stack, uint8_t channel, float* val)
{
	uint8_t buff[2];
	int dev = -1;
	uint16_t aux16 = 0;

	if (channel >= HOME_ADC_CH_NO || NULL == val)
	{
		return ERROR;
	}
	dev = homeCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}
	if (OK != i2cMemRead(dev, HOME_I2C_ADC_ADD + 2 * channel, buff, 2))
	{
		return ERROR;
	}
	memcpy(&aux16, buff, 2);

	*val = (float)aux16/ VOLT_TO_MILIVOLT;

	return OK;
}

int homeGet1WbTemp(uint8_t stack, uint8_t channel, float *val)
{
	uint8_t buff[2];
	int dev = -1;
	int16_t aux16 = 0;

	if (channel > IND_OWB_CH_MAX || NULL == val)
	{
		return ERROR;
	}
	dev = homeCardCheck(stack);
	if (dev < 0)
	{
		return ERROR;
	}
	if (OK != i2cMemRead(dev, HOME_I2C_1W_TEMP_ADD + 2 * channel, buff, 2))
	{
		return ERROR;
	}
	memcpy(&aux16, buff, 2);
	*val = (float)aux16 / OWB_TEMP_SCALE;
	return OK;
}


//---------------------------------------------------------------------------------------


const uint8_t mosfet8MaskRemap[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
	0x80};
	uint8_t mosfetBaseAdd[8] = {RELAY8_HW_I2C_BASE_ADD, RELAY8_HW_I2C_BASE_ADD, RELAY8_HW_I2C_BASE_ADD, RELAY8_HW_I2C_BASE_ADD, RELAY8_HW_I2C_BASE_ADD, RELAY8_HW_I2C_BASE_ADD, RELAY8_HW_I2C_BASE_ADD, RELAY8_HW_I2C_BASE_ADD};

uint8_t mosfet8ToIO(uint8_t relay)
{
	uint8_t i;
	uint8_t val = 0;
	for (i = 0; i < 8; i++)
	{
		if ( (relay & (1 << i)) != 0)
			val += mosfet8MaskRemap[i];
	}
	return val;
}

int mosfet8CardCheck(int stack)
{
	uint8_t add = 0;
	uint8_t buff[2];
	int dev = -1;

	if ( (stack < 0) || (stack > 7))
	{
		return ERROR;
	}
	add = (stack + mosfetBaseAdd[stack]) ^ 0x07;
	dev = i2cSetup(add);
	if(ERROR == i2cMemRead(dev, RELAY8_CFG_REG_ADD, buff, 1))
	{
		if(mosfetBaseAdd[stack] == RELAY8_HW_I2C_BASE_ADD)
		{
			mosfetBaseAdd[stack] = RELAY8_HW_I2C_BASE_ALT_ADD;
		}
		else
		{
			mosfetBaseAdd[stack] = RELAY8_HW_I2C_BASE_ADD;
		}
		add = (stack + mosfetBaseAdd[stack]) ^ 0x07;
		dev = i2cSetup(add);
	}
	
	return dev;
}

int mosfet8Init(int stack)
{
	int dev = -1;
	uint8_t add = 0;
	uint8_t buff[2];

	dev = mosfet8CardCheck(stack);
	if (dev <= 0)
	{
		return ERROR;
	}

	if (ERROR == i2cMemRead(dev, RELAY8_CFG_REG_ADD, buff, 1))
	{
		return ERROR;
	}
	if (OK == i2cMemRead(dev, RELAY8_REG_COUNT, buff, 1)) //16 bits I/O expander found
	{
		return ERROR;
	}
	if (buff[0] != 0) //non initialized I/O Expander
	{
		// make all I/O pins output
		buff[0] = 0;
		if (OK > i2cMemWrite(dev, RELAY8_CFG_REG_ADD, buff, 1))
		{
			return ERROR;
		}
		// put all pins in 0-logic state
		buff[0] = 0;
		if (OK > i2cMemWrite(dev, RELAY8_OUTPORT_REG_ADD, buff, 1))
		{
			return ERROR;
		}
	}
	return OK;
}

int mosfets8Set(uint8_t stack, uint8_t val)
{
	uint8_t buff[2];
	int dev = -1;
	static uint8_t mosfetsOldVal[STACK_LEVELS] = {0, 0, 0, 0, 0, 0, 0, 0};

	if (stack >= 8)
	{
		return ERROR;
	}

	if (mosfetsOldVal[stack] == val)
	{
		return OK;
	}
	dev = mosfet8CardCheck(stack);
	if (dev <= 0)
	{
		return ERROR;
	}

	buff[0] = mosfet8ToIO(val);

	if (OK != i2cMemWrite(dev, RELAY8_OUTPORT_REG_ADD, buff, 1))
	{
		return ERROR;
	}
	mosfetsOldVal[stack] = val;
	return OK;
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
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

	/*********READING AND WRITING TO I/O**************
	*bool_input[0][0] = read_digital_input(0);
	write_digital_output(0, *bool_output[0][0]);
	*int_input[0] = read_analog_input(0);
	write_analog_output(0, *int_output[0]);
	**************************************************/

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
