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
// This file has all the MODBUS/TCP functions supported by the OpenPLC. If any
// other function is to be added to the project, it must be added here
// Thiago Alves, Dec 2015
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "ladder.h"
#include <string.h>

#define MAX_DISCRETE_INPUT              8192
#define MAX_COILS                       8192
#define MAX_HOLD_REGS                   8192
#define MAX_INP_REGS                    1024

#define MIN_16B_RANGE                   1024
#define MAX_16B_RANGE                   2047
#define MIN_32B_RANGE                   2048
#define MAX_32B_RANGE                   4095
#define MIN_64B_RANGE                   4096
#define MAX_64B_RANGE                   8191

#define MB_FC_NONE                      0
#define MB_FC_READ_COILS                1
#define MB_FC_READ_INPUTS               2
#define MB_FC_READ_HOLDING_REGISTERS    3
#define MB_FC_READ_INPUT_REGISTERS      4
#define MB_FC_WRITE_COIL                5
#define MB_FC_WRITE_REGISTER            6
#define MB_FC_WRITE_MULTIPLE_COILS      15
#define MB_FC_WRITE_MULTIPLE_REGISTERS  16
#define MB_FC_DEBUG_INFO                0x41 // Request debug variables count
#define MB_FC_DEBUG_SET                 0x42 // Debug set trace (force variable)
#define MB_FC_DEBUG_GET                 0x43 // Debug get trace (read variables)
#define MB_FC_DEBUG_GET_LIST            0x44 // Debug get trace list (read list of variables)
#define MB_FC_DEBUG_GET_MD5             0x45 // Debug get current program MD5
#define MB_FC_ERROR                     255

#define ERR_NONE                        0
#define ERR_ILLEGAL_FUNCTION            1
#define ERR_ILLEGAL_DATA_ADDRESS        2
#define ERR_ILLEGAL_DATA_VALUE          3
#define ERR_SLAVE_DEVICE_FAILURE        4
#define ERR_SLAVE_DEVICE_BUSY           6


#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define lowByte(w) ((unsigned char) ((w) & 0xff))
#define highByte(w) ((unsigned char) ((w) >> 8))

IEC_BOOL mb_discrete_input[MAX_DISCRETE_INPUT];
IEC_BOOL mb_coils[MAX_COILS];
IEC_UINT mb_input_regs[MAX_INP_REGS];
IEC_UINT mb_holding_regs[MAX_HOLD_REGS];

int MessageLength;

#include "debug.h"

// Debugger functions
// extern "C" uint16_t get_var_count(void);
// extern "C" size_t get_var_size(size_t);// {return 0;}
// extern "C" void *get_var_addr(size_t);// {return 0;}
// extern "C" void force_var(size_t, bool, void *);// {}
// extern "C" void set_trace(size_t, bool, void *);// {}
// extern "C" void trace_reset(void);// {}
// extern "C" void set_endianness(uint8_t value);
extern char md5[];
extern unsigned long __tick;

#define MB_DEBUG_SUCCESS                 0x7E
#define MB_DEBUG_ERROR_OUT_OF_BOUNDS     0x81
#define MB_DEBUG_ERROR_OUT_OF_MEMORY     0x82
#define SAME_ENDIANNESS                  0
#define REVERSE_ENDIANNESS               1
#define MAX_MB_FRAME                     260

//-----------------------------------------------------------------------------
// Concatenate two bytes into an int
//-----------------------------------------------------------------------------
int word(unsigned char byte1, unsigned char byte2)
{
	int returnValue;
	returnValue = (int)(byte1 << 8) | (int)byte2;

	return returnValue;
}

//-----------------------------------------------------------------------------
// This function sets the internal NULL OpenPLC buffers to point to valid
// positions on the Modbus buffer
//-----------------------------------------------------------------------------
void mapUnusedIO()
{
	pthread_mutex_lock(&bufferLock);

	for(int i = 0; i < MAX_DISCRETE_INPUT; i++)
	{
		if (bool_input[i/8][i%8] == NULL) bool_input[i/8][i%8] = &mb_discrete_input[i];
	}

	for(int i = 0; i < MAX_COILS; i++)
	{
		if (bool_output[i/8][i%8] == NULL) bool_output[i/8][i%8] = &mb_coils[i];
	}

	for (int i = 0; i < MAX_INP_REGS; i++)
	{
		if (int_input[i] == NULL) int_input[i] = &mb_input_regs[i];
	}

	for (int i = 0; i <= MAX_16B_RANGE; i++)
	{
		if (i < MIN_16B_RANGE)
        {
            if (int_output[i] == NULL)
            {
                int_output[i] = &mb_holding_regs[i];
            }
        }

		else if (i >= MIN_16B_RANGE && i <= MAX_16B_RANGE)
        {
			if (int_memory[i - MIN_16B_RANGE] == NULL)
            {
                int_memory[i - MIN_16B_RANGE] = &mb_holding_regs[i];
            }
        }
	}

	pthread_mutex_unlock(&bufferLock);
}

//-----------------------------------------------------------------------------
// Response to a Modbus Error
//-----------------------------------------------------------------------------
void ModbusError(unsigned char *buffer, int mb_error)
{
	buffer[4] = 0;
	buffer[5] = 3;
	buffer[7] = buffer[7] | 0x80; //set the highest bit
	buffer[8] = mb_error;
	MessageLength = 9;
}

//-----------------------------------------------------------------------------
// Implementation of Modbus/TCP Read Coils
//-----------------------------------------------------------------------------
void ReadCoils(unsigned char *buffer, int bufferSize)
{
	int Start, ByteDataLength, CoilDataLength;
	int mb_error = ERR_NONE;

	//this request must have at least 12 bytes. If it doesn't, it's a corrupted message
	if (bufferSize < 12)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_VALUE);
		return;
	}

	Start = word(buffer[8], buffer[9]);
	CoilDataLength = word(buffer[10], buffer[11]);
	ByteDataLength = CoilDataLength / 8; //calculating the size of the message in bytes
	if(ByteDataLength * 8 < CoilDataLength) ByteDataLength++;

	//asked for too many coils
	if (ByteDataLength > 255)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_ADDRESS);
		return;
	}

	//preparing response
	buffer[4] = highByte(ByteDataLength + 3);
	buffer[5] = lowByte(ByteDataLength + 3); //Number of bytes after this one
	buffer[8] = ByteDataLength;     //Number of bytes of data

	pthread_mutex_lock(&bufferLock);
	for(int i = 0; i < ByteDataLength ; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			int position = Start + i * 8 + j;
			if (position < MAX_COILS)
			{
				if (bool_output[position/8][position%8] != NULL)
				{
					bitWrite(buffer[9 + i], j, *bool_output[position/8][position%8]);
				}
				else
				{
					bitWrite(buffer[9 + i], j, 0);
				}
			}
			else //invalid address
			{
				mb_error = ERR_ILLEGAL_DATA_ADDRESS;
			}
		}
	}
	pthread_mutex_unlock(&bufferLock);

	if (mb_error != ERR_NONE)
	{
		ModbusError(buffer, mb_error);
	}
	else
	{
		MessageLength = ByteDataLength + 9;
	}
}

//-----------------------------------------------------------------------------
// Implementation of Modbus/TCP Read Discrete Inputs
//-----------------------------------------------------------------------------
void ReadDiscreteInputs(unsigned char *buffer, int bufferSize)
{
	int Start, ByteDataLength, InputDataLength;
	int mb_error = ERR_NONE;

	//this request must have at least 12 bytes. If it doesn't, it's a corrupted message
	if (bufferSize < 12)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_VALUE);
		return;
	}

	Start = word(buffer[8],buffer[9]);
	InputDataLength = word(buffer[10],buffer[11]);
	ByteDataLength = InputDataLength / 8;
	if(ByteDataLength * 8 < InputDataLength) ByteDataLength++;

	//asked for too many inputs
	if (ByteDataLength > 255)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_ADDRESS);
		return;
	}

	//Preparing response
	buffer[4] = highByte(ByteDataLength + 3);
	buffer[5] = lowByte(ByteDataLength + 3); //Number of bytes after this one
	buffer[8] = ByteDataLength;     //Number of bytes of data

	pthread_mutex_lock(&bufferLock);
	for(int i = 0; i < ByteDataLength ; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			int position = Start + i * 8 + j;
			if (position < MAX_DISCRETE_INPUT)
			{
				if (bool_input[position/8][position%8] != NULL)
				{
					bitWrite(buffer[9 + i], j, *bool_input[position/8][position%8]);
				}
				else
				{
					bitWrite(buffer[9 + i], j, 0);
				}
			}
			else //invalid address
			{
				mb_error = ERR_ILLEGAL_DATA_ADDRESS;
			}
		}
	}
	pthread_mutex_unlock(&bufferLock);

	if (mb_error != ERR_NONE)
	{
		ModbusError(buffer, mb_error);
	}
	else
	{
		MessageLength = ByteDataLength + 9;
	}
}

//-----------------------------------------------------------------------------
// Implementation of Modbus/TCP Read Holding Registers
//-----------------------------------------------------------------------------
void ReadHoldingRegisters(unsigned char *buffer, int bufferSize)
{
	int Start, WordDataLength, ByteDataLength;
	int mb_error = ERR_NONE;

	//this request must have at least 12 bytes. If it doesn't, it's a corrupted message
	if (bufferSize < 12)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_VALUE);
		return;
	}

	Start = word(buffer[8],buffer[9]);
	WordDataLength = word(buffer[10],buffer[11]);
	ByteDataLength = WordDataLength * 2;

	//asked for too many registers
	if (ByteDataLength > 255)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_ADDRESS);
		return;
	}

	//preparing response
	buffer[4] = highByte(ByteDataLength + 3);
	buffer[5] = lowByte(ByteDataLength + 3); //Number of bytes after this one
	buffer[8] = ByteDataLength;     //Number of bytes of data

	pthread_mutex_lock(&bufferLock);
	for(int i = 0; i < WordDataLength; i++)
	{
		int position = Start + i;
		if (position <= MIN_16B_RANGE)
		{
			if (int_output[position] != NULL)
			{
				buffer[ 9 + i * 2] = highByte(*int_output[position]);
				buffer[10 + i * 2] = lowByte(*int_output[position]);
			}
			else
			{
				buffer[ 9 + i * 2] = 0;
				buffer[10 + i * 2] = 0;
			}
		}
		//accessing memory
		//16-bit registers
		else if (position >= MIN_16B_RANGE && position <= MAX_16B_RANGE)
		{
			if (int_memory[position - MIN_16B_RANGE] != NULL)
			{
				buffer[ 9 + i * 2] = highByte(*int_memory[position - MIN_16B_RANGE]);
				buffer[10 + i * 2] = lowByte(*int_memory[position - MIN_16B_RANGE]);
			}
			else
			{
				buffer[ 9 + i * 2] = 0;
				buffer[10 + i * 2] = 0;
			}
		}
		//32-bit registers
		else if (position >= MIN_32B_RANGE && position <= MAX_32B_RANGE)
		{
			if (dint_memory[(position - MIN_32B_RANGE)/2] != NULL)
			{
				if ((position - MIN_32B_RANGE) % 2 == 0) //first word
				{
					uint16_t tempValue = (uint16_t)(*dint_memory[(position - MIN_32B_RANGE)/2] >> 16);
					buffer[ 9 + i * 2] = highByte(tempValue);
					buffer[10 + i * 2] = lowByte(tempValue);
				}
				else //second word
				{
					uint16_t tempValue = (uint16_t)(*dint_memory[(position - MIN_32B_RANGE)/2] & 0xffff);
					buffer[ 9 + i * 2] = highByte(tempValue);
					buffer[10 + i * 2] = lowByte(tempValue);
				}
			}
			else
			{
				buffer[ 9 + i * 2] = mb_holding_regs[position];
				buffer[10 + i * 2] = mb_holding_regs[position];
			}
		}
		//64-bit registers
		else if (position >= MIN_64B_RANGE && position <= MAX_64B_RANGE)
		{
			if (lint_memory[(position - MIN_64B_RANGE)/4] != NULL)
			{
				if ((position - MIN_64B_RANGE) % 4 == 0) //first word
				{
					uint16_t tempValue = (uint16_t)(*lint_memory[(position - MIN_64B_RANGE)/4] >> 48);
					buffer[ 9 + i * 2] = highByte(tempValue);
					buffer[10 + i * 2] = lowByte(tempValue);
				}
				else if ((position - MIN_64B_RANGE) % 4 == 1)//second word
				{
					uint16_t tempValue = (uint16_t)((*lint_memory[(position - MIN_64B_RANGE)/4] >> 32) & 0xffff);
					buffer[ 9 + i * 2] = highByte(tempValue);
					buffer[10 + i * 2] = lowByte(tempValue);
				}
				else if ((position - MIN_64B_RANGE) % 4 == 2)//third word
				{
					uint16_t tempValue = (uint16_t)((*lint_memory[(position - MIN_64B_RANGE)/4] >> 16) & 0xffff);
					buffer[ 9 + i * 2] = highByte(tempValue);
					buffer[10 + i * 2] = lowByte(tempValue);
				}
				else if ((position - MIN_64B_RANGE) % 4 == 3)//fourth word
				{
					uint16_t tempValue = (uint16_t)(*lint_memory[(position - MIN_64B_RANGE)/4] & 0xffff);
					buffer[ 9 + i * 2] = highByte(tempValue);
					buffer[10 + i * 2] = lowByte(tempValue);
				}
			}
			else
			{
				buffer[ 9 + i * 2] = mb_holding_regs[position];
				buffer[10 + i * 2] = mb_holding_regs[position];
			}
		}
		//invalid address
		else
		{
			mb_error = ERR_ILLEGAL_DATA_ADDRESS;
		}
	}
	pthread_mutex_unlock(&bufferLock);

	if (mb_error != ERR_NONE)
	{
		ModbusError(buffer, mb_error);
	}
	else
	{
		MessageLength = ByteDataLength + 9;
	}
}

//-----------------------------------------------------------------------------
// Implementation of Modbus/TCP Read Input Registers
//-----------------------------------------------------------------------------
void ReadInputRegisters(unsigned char *buffer, int bufferSize)
{
	int Start, WordDataLength, ByteDataLength;
	int mb_error = ERR_NONE;

	//this request must have at least 12 bytes. If it doesn't, it's a corrupted message
	if (bufferSize < 12)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_VALUE);
		return;
	}

	Start = word(buffer[8],buffer[9]);
	WordDataLength = word(buffer[10],buffer[11]);
	ByteDataLength = WordDataLength * 2;

	//asked for too many registers
	if (ByteDataLength > 255)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_ADDRESS);
		return;
	}

	//preparing response
	buffer[4] = highByte(ByteDataLength + 3);
	buffer[5] = lowByte(ByteDataLength + 3); //Number of bytes after this one
	buffer[8] = ByteDataLength;     //Number of bytes of data

	pthread_mutex_lock(&bufferLock);
	for(int i = 0; i < WordDataLength; i++)
	{
		int position = Start + i;
		if (position < MAX_INP_REGS)
		{
			if (int_input[position] != NULL)
			{
				buffer[ 9 + i * 2] = highByte(*int_input[position]);
				buffer[10 + i * 2] = lowByte(*int_input[position]);
			}
			else
			{
				buffer[ 9 + i * 2] = 0;
				buffer[10 + i * 2] = 0;
			}
		}
		else //invalid address
		{
			mb_error = ERR_ILLEGAL_DATA_ADDRESS;
		}
	}
	pthread_mutex_unlock(&bufferLock);

	if (mb_error != ERR_NONE)
	{
		ModbusError(buffer, mb_error);
	}
	else
	{
		MessageLength = ByteDataLength + 9;
	}
}

//-----------------------------------------------------------------------------
// Implementation of Modbus/TCP Write Coil
//-----------------------------------------------------------------------------
void WriteCoil(unsigned char *buffer, int bufferSize)
{
	int Start;
	int mb_error = ERR_NONE;

	//this request must have at least 12 bytes. If it doesn't, it's a corrupted message
	if (bufferSize < 12)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_VALUE);
		return;
	}

	Start = word(buffer[8], buffer[9]);

	if (Start < MAX_COILS)
	{
		unsigned char value;
		if (word(buffer[10], buffer[11]) > 0)
		{
			value = 1;
		}
		else
		{
			value = 0;
		}

		pthread_mutex_lock(&bufferLock);
		if (bool_output[Start/8][Start%8] != NULL)
		{
			*bool_output[Start/8][Start%8] = value;
		}
		pthread_mutex_unlock(&bufferLock);
	}

	else //invalid address
	{
		mb_error = ERR_ILLEGAL_DATA_ADDRESS;
	}

	if (mb_error != ERR_NONE)
	{
		ModbusError(buffer, mb_error);
	}
	else
	{
		buffer[4] = 0;
		buffer[5] = 6; //Number of bytes after this one.
		MessageLength = 12;
	}
}

/**
 * @brief Write a word to a register at the given position. The caller must hold the `bufferLock`.
 *
 * @param position The position of the register.
 * @param value The word to write to the register.
 *
 * @return An error code, if an error occurred.
 */
int writeToRegisterWithoutLocking(int position, uint16_t value)
{
	//analog outputs
	if (position <= MIN_16B_RANGE)
	{
		if (int_output[position] != NULL) *int_output[position] = value;
	}
	//accessing memory
	//16-bit registers
	else if (position >= MIN_16B_RANGE && position <= MAX_16B_RANGE)
	{
		if (int_memory[position - MIN_16B_RANGE] != NULL) *int_memory[position - MIN_16B_RANGE] = value;
	}
	//32-bit registers
	else if (position >= MIN_32B_RANGE && position <= MAX_32B_RANGE)
	{
		if (dint_memory[(position - MIN_32B_RANGE) / 2] == NULL)
		{
			mb_holding_regs[position] = value;
		}
		else
		{
			// Overwrite one word of the 32 bit register:
			// Calculate the bit offset of the word in the 32 bit register.
			int bit_offset = (1 - ((position - MIN_32B_RANGE) % 2)) * 16;
			// Mask the word.
			*dint_memory[(position - MIN_32B_RANGE) / 2] &= ~(((uint32_t) 0xffff) << bit_offset);
			// Overwrite the word.
			*dint_memory[(position - MIN_32B_RANGE) / 2] |= ((uint32_t) value) << bit_offset;
		}
	}
	//64-bit registers
	else if (position >= MIN_64B_RANGE && position <= MAX_64B_RANGE)
	{
		if (lint_memory[(position - MIN_64B_RANGE) / 4] == NULL)
		{
			mb_holding_regs[position] = value;
		}
		else
		{
			// Overwrite one word of the 64 bit register:
			// Calculate the bit offset of the word in the 64 bit register.
			int bit_offset = (3 - ((position - MIN_64B_RANGE) % 4)) * 16;
			// Mask the word.
			*lint_memory[(position - MIN_64B_RANGE) / 4] &= ~(((uint64_t) 0xffff) << bit_offset);
			// Overwrite the word.
			*lint_memory[(position - MIN_64B_RANGE) / 4] |= ((uint64_t) value) << bit_offset;
		}
	}
	else //invalid address
	{
		return ERR_ILLEGAL_DATA_ADDRESS;
	}
	return ERR_NONE;
}


//-----------------------------------------------------------------------------
// Implementation of Modbus/TCP Write Holding Register
//-----------------------------------------------------------------------------
void WriteRegister(unsigned char *buffer, int bufferSize)
{
	int Start;
	int mb_error = ERR_NONE;

	//this request must have at least 12 bytes. If it doesn't, it's a corrupted message
	if (bufferSize < 12)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_VALUE);
		return;
	}

	Start = word(buffer[8],buffer[9]);

	pthread_mutex_lock(&bufferLock);
	mb_error = writeToRegisterWithoutLocking(Start, word(buffer[10], buffer[11]));
	pthread_mutex_unlock(&bufferLock);

	if (mb_error != ERR_NONE)
	{
		ModbusError(buffer, mb_error);
	}
	else
	{
		buffer[4] = 0;
		buffer[5] = 6; //Number of bytes after this one.
		MessageLength = 12;
	}
}

//-----------------------------------------------------------------------------
// Implementation of Modbus/TCP Write Multiple Coils
//-----------------------------------------------------------------------------
void WriteMultipleCoils(unsigned char *buffer, int bufferSize)
{
	int Start, ByteDataLength, CoilDataLength;
	int mb_error = ERR_NONE;

	//this request must have at least 12 bytes. If it doesn't, it's a corrupted message
	if (bufferSize < 12)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_VALUE);
		return;
	}

	Start = word(buffer[8],buffer[9]);
	CoilDataLength = word(buffer[10],buffer[11]);
	ByteDataLength = CoilDataLength / 8;
	if(ByteDataLength * 8 < CoilDataLength) ByteDataLength++;

	//this request must have all the bytes it wants to write. If it doesn't, it's a corrupted message
	if ( (bufferSize < (13 + ByteDataLength)) || (buffer[12] != ByteDataLength) )
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_VALUE);
		return;
	}

	//preparing response
	buffer[4] = 0;
	buffer[5] = 6; //Number of bytes after this one.

	pthread_mutex_lock(&bufferLock);
	for(int i = 0; i < ByteDataLength ; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			int position = Start + i * 8 + j;
			if (position < MAX_COILS)
			{
				if (bool_output[position/8][position%8] != NULL) *bool_output[position/8][position%8] = bitRead(buffer[13 + i], j);
			}
			else //invalid address
			{
				mb_error = ERR_ILLEGAL_DATA_ADDRESS;
			}
		}
	}
	pthread_mutex_unlock(&bufferLock);

	if (mb_error != ERR_NONE)
	{
		ModbusError(buffer, mb_error);
	}
	else
	{
		MessageLength = 12;
	}
}

//-----------------------------------------------------------------------------
// Implementation of Modbus/TCP Write Multiple Registers
//-----------------------------------------------------------------------------
void WriteMultipleRegisters(unsigned char *buffer, int bufferSize)
{
	int Start, WordDataLength, ByteDataLength;
	int mb_error = ERR_NONE;

	//this request must have at least 12 bytes. If it doesn't, it's a corrupted message
	if (bufferSize < 12)
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_VALUE);
		return;
	}

	Start = word(buffer[8],buffer[9]);
	WordDataLength = word(buffer[10],buffer[11]);
	ByteDataLength = WordDataLength * 2;

	//this request must have all the bytes it wants to write. If it doesn't, it's a corrupted message
	if ( (bufferSize < (13 + ByteDataLength)) || (buffer[12] != ByteDataLength) )
	{
		ModbusError(buffer, ERR_ILLEGAL_DATA_VALUE);
		return;
	}

	//preparing response
	buffer[4] = 0;
	buffer[5] = 6; //Number of bytes after this one.

	pthread_mutex_lock(&bufferLock);
	for(int i = 0; i < WordDataLength; i++)
	{
		int position = Start + i;
		int error = writeToRegisterWithoutLocking(position, word(buffer[13 + i * 2], buffer[14 + i * 2]));
		if (error != ERR_NONE)
		{
			mb_error = error;
		}
	}
	pthread_mutex_unlock(&bufferLock);

	if (mb_error != ERR_NONE)
	{
		ModbusError(buffer, mb_error);
	}
	else
	{
		MessageLength = 12;
	}
}

/**
 * @brief Sends a Modbus response frame for the DEBUG_INFO function code.
 *
 * This function constructs a Modbus response frame for the DEBUG_INFO function code.
 * The response frame includes the number of variables defined in the PLC program.
 *
 * Modbus Response Frame (DEBUG_INFO):
 * +-----+-------+-------+
 * | MB  | Count | Count |
 * | FC  |       |       |
 * +-----+-------+-------+
 * |0x41 | High  | Low   |
 * |     | Byte  | Byte  |
 * |     |       |       |
 * +-----+-------+-------+
 *
 * @return void
 */
void debugInfo(unsigned char *mb_frame)
{
    uint16_t variableCount = get_var_count();
    MessageLength = 10;
    mb_frame[7] = MB_FC_DEBUG_INFO;
    mb_frame[8] = (uint8_t)(variableCount >> 8); // High byte
    mb_frame[9] = (uint8_t)(variableCount & 0xFF); // Low byte
}

/**
 * @brief Sends a Modbus response frame for the DEBUG_SET function code.
 *
 * This function constructs a Modbus response frame for the DEBUG_SET function code.
 * The response frame indicates whether the set trace command was successful or if
 * there was an error, such as an out-of-bounds index.
 *
 * Modbus Response Frame (DEBUG_SET):
 * +-----+------+
 * | MB  | Resp.|
 * | FC  | Code |
 * +-----+------+
 * |0x42 | Code |
 * +-----+------+
 *
 * @param varidx The index of the variable to set trace for.
 * @param flag The trace flag.
 * @param len The length of the trace data.
 * @param value Pointer to the trace data.
 * 
 * @return void
 */
void debugSetTrace(unsigned char *mb_frame, uint16_t varidx, uint8_t flag, uint16_t len, void *value)
{
    uint16_t variableCount = get_var_count();
    if (varidx >= variableCount || len > (MAX_MB_FRAME - 7))
    {
        // Respond with an error indicating that the index is out of range
        MessageLength = 9;
        mb_frame[7] = MB_FC_DEBUG_SET;
        mb_frame[8] = MB_DEBUG_ERROR_OUT_OF_BOUNDS;
        return;
    }

    // Execute set trace command
    set_trace((size_t)varidx, (bool)flag, value);

    // Response
    MessageLength = 9;
    mb_frame[7] = MB_FC_DEBUG_SET;
    mb_frame[8] = MB_DEBUG_SUCCESS;
}

/**
 * @brief Sends a Modbus response frame for the DEBUG_GET function code.
 *
 * This function constructs a Modbus response frame for the DEBUG_GET function code.
 * The response frame includes the trace data for variables within the specified index range.
 *
 * Modbus Response Frame (DEBUG_GET):
 * +-----+-------+-------+-------+-------+-------+-------+-------+-------+------+-------+
 * | MB  | Resp. | Last  | Last  | Tick  | Tick  | Tick  | Tick  | Resp. | Resp.| Data  |
 * | FC  | Code  | Index | Index |       |       |       |       | Size  | Size | Bytes |
 * +-----+-------+-------+-------+-------+-------+-------+-------+-------+------+-------+
 * |0x44 | Code  | High  | Low   | High  | Mid   | Mid   | Low   | High  | Low  | Data  |
 * |     |       | Byte  | Byte  | Byte  | Byte  | Byte  | Byte  | Byte  | Byte | Bytes |
 * +-----+-------+-------+-------+-------+-------+-------+-------+-------+------+-------+
 *
 * @param startidx The start index of the variables to get trace for.
 * @param endidx The end index of the variables to get trace for.
 * 
 * @return void
 */
void debugGetTrace(unsigned char *mb_frame, uint16_t startidx, uint16_t endidx)
{
    uint16_t variableCount = get_var_count();
    // Verify that startidx and endidx fall within the valid range of variables
    if (startidx >= variableCount || endidx >= variableCount || startidx > endidx) 
    {
        // Respond with an error indicating that the indices are out of range
        MessageLength = 9;
        mb_frame[7] = MB_FC_DEBUG_GET;
        mb_frame[8] = MB_DEBUG_ERROR_OUT_OF_BOUNDS;
        return;
    }

    uint16_t lastVarIdx = startidx;
    size_t responseSize = 0;
    uint8_t *responsePtr = &(mb_frame[17]); // Start of response data
    
    for (uint16_t varidx = startidx; varidx <= endidx; varidx++) 
    {
        size_t varSize = get_var_size(varidx);
        if ((responseSize + 17) + varSize <= MAX_MB_FRAME) // Make sure the response fits
        {
            void *varAddr = get_var_addr(varidx);

            // Copy the variable value to the response buffer
            memcpy(responsePtr, varAddr, varSize);

            // Update response pointer and size
            responsePtr += varSize;
            responseSize += varSize;

            // Update the lastVarIdx
            lastVarIdx = varidx;
        }
        else 
        {
            // Response buffer is full, break the loop
            break;
        }
    }

    MessageLength = 13 + responseSize; // Update response length
    mb_frame[7] = MB_FC_DEBUG_GET;
    mb_frame[8] = MB_DEBUG_SUCCESS;
    mb_frame[9] = (uint8_t)(lastVarIdx >> 8); // High byte
    mb_frame[10] = (uint8_t)(lastVarIdx & 0xFF); // Low byte
    mb_frame[11] = (uint8_t)((__tick >> 24) & 0xFF); // Highest byte
    mb_frame[12] = (uint8_t)((__tick >> 16) & 0xFF); // Second highest byte
    mb_frame[13] = (uint8_t)((__tick >> 8) & 0xFF);  // Second lowest byte
    mb_frame[14] = (uint8_t)(__tick & 0xFF);         // Lowest byte
    mb_frame[15] = (uint8_t)(responseSize >> 8); // High byte
    mb_frame[16] = (uint8_t)(responseSize & 0xFF); // Low byte
}

/**
 * @brief Sends a Modbus response frame for the DEBUG_GET_LIST function code.
 *
 * This function constructs a Modbus response frame for the DEBUG_GET_LIST function code.
 * The response frame includes the trace data for variables specified in the provided index list.
 *
 * Modbus Response Frame (DEBUG_GET_LIST):
 * +-----+-------+-------+-------+-------+-------+-------+-------+-------+------+-------+
 * | MB  | Resp. | Last  | Last  | Tick  | Tick  | Tick  | Tick  | Resp. | Resp.| Data  |
 * | FC  | Code  | Index | Index |       |       |       |       | Size  | Size | Bytes |
 * +-----+-------+-------+-------+-------+-------+-------+-------+-------+------+-------+
 * |0x44 | Code  | High  | Low   | High  | Mid   | Mid   | Low   | High  | Low  | Data  |
 * |     |       | Byte  | Byte  | Byte  | Byte  | Byte  | Byte  | Byte  | Byte | Bytes |
 * +-----+-------+-------+-------+-------+-------+-------+-------+-------+------+-------+
 *
 * @param numIndexes The number of indexes requested.
 * @param indexArray Pointer to the array containing variable indexes.
 * 
 * @return void
 */
void debugGetTraceList(unsigned char *mb_frame, uint16_t numIndexes, uint8_t *indexArray)
{
    uint16_t response_idx = 17;  // Start of response data in the response buffer
    uint16_t responseSize = 0;
    uint16_t lastVarIdx = 0;
    uint16_t variableCount = get_var_count();

    #ifdef MBSERIAL
        #define VARIDX_SIZE 20
    #else
        #define VARIDX_SIZE 60
    #endif

    uint16_t varidx_array[VARIDX_SIZE];

    // Validate if buffer has space for all indexes
    if (numIndexes > VARIDX_SIZE)
    {
        // Respond with a memory error
        MessageLength = 9;
        mb_frame[7] = MB_FC_DEBUG_GET_LIST;
        mb_frame[8] = MB_DEBUG_ERROR_OUT_OF_MEMORY;
        return;
    }

    // Copy all indexes to array
    for (uint16_t i = 0; i < numIndexes; i++)
    {
        varidx_array[i] = (uint16_t)indexArray[i * 2] << 8 | indexArray[i * 2 + 1];
    }

    // Validate if all requested indexes are in range
    for (uint16_t i = 0; i < numIndexes; i++) 
    {
        if (varidx_array[i] >= variableCount) 
        {
            // Respond with an error indicating that the index is out of range
            MessageLength = 3;
            mb_frame[7] = MB_FC_DEBUG_GET_LIST;
            mb_frame[8] = MB_DEBUG_ERROR_OUT_OF_BOUNDS;
            return;
        }

        // Add requested indexes and their traces to the response buffer
        size_t varSize = get_var_size(varidx_array[i]);

        // Make sure there is enough space in the response buffer
        if (response_idx + varSize <= MAX_MB_FRAME) 
        {
            // Add variable data to the response buffer
            void *varAddr = get_var_addr(varidx_array[i]);
            memcpy(&mb_frame[response_idx], varAddr, varSize);
            response_idx += varSize;
            responseSize += varSize;

            // Update the lastVarIdx
            lastVarIdx = varidx_array[i];
        } 
        else 
        {
            // Response buffer is full, break the loop
            break;
        }
    }

    // Update response length, lastVarIdx, and response size
    MessageLength = response_idx;
    mb_frame[7] = MB_FC_DEBUG_GET_LIST;
    mb_frame[8] = MB_DEBUG_SUCCESS;
    mb_frame[9] = (uint8_t)(lastVarIdx >> 8); // High byte
    mb_frame[10] = (uint8_t)(lastVarIdx & 0xFF); // Low byte
    mb_frame[11] = (uint8_t)((__tick >> 24) & 0xFF); // Highest byte
    mb_frame[12] = (uint8_t)((__tick >> 16) & 0xFF); // Second highest byte
    mb_frame[13] = (uint8_t)((__tick >> 8) & 0xFF);  // Second lowest byte
    mb_frame[14] = (uint8_t)(__tick & 0xFF);         // Lowest byte
    mb_frame[15] = (uint8_t)(responseSize >> 8); // High byte
    mb_frame[16] = (uint8_t)(responseSize & 0xFF); // Low byte
}

void debugGetMd5(unsigned char *mb_frame, void *endianness)
{
    // Check endianness
    uint16_t endian_check = 0;
    memcpy(&endian_check, endianness, 2);
    if (endian_check == 0xDEAD)
    {
        set_endianness(SAME_ENDIANNESS);
    }
    else if (endian_check == 0xADDE)
    {
        set_endianness(REVERSE_ENDIANNESS);
    }
    else
    {
        // Respond with an error indicating that the argument is wrong
        MessageLength = 9;
        mb_frame[7] = MB_FC_DEBUG_GET_MD5;
        mb_frame[8] = MB_DEBUG_ERROR_OUT_OF_BOUNDS;
        //return;
    }

    mb_frame[7] = MB_FC_DEBUG_GET_MD5;
    mb_frame[8] = MB_DEBUG_SUCCESS;

    // Copy MD5 string byte by byte to mb_frame starting from index 3
    int md5_len = 0;
    for (md5_len = 0; md5[md5_len] != '\0'; md5_len++) 
    {
        mb_frame[md5_len + 9] = md5[md5_len];
    }

    // Calculate mb_frame_len (MD5 string length + 3)
    MessageLength = md5_len + 9;
}

//-----------------------------------------------------------------------------
// This function must parse and process the client request and write back the
// response for it. The return value is the size of the response message in
// bytes.
//-----------------------------------------------------------------------------
int processModbusMessage(unsigned char *buffer, int bufferSize)
{
	MessageLength = 0;
	uint16_t field1 = (uint16_t)buffer[8] << 8 | (uint16_t)buffer[9];
    uint16_t field2 = (uint16_t)buffer[10] << 8 | (uint16_t)buffer[11];
    uint8_t flag = buffer[10];
    uint16_t len = (uint16_t)buffer[11] << 8 | (uint16_t)buffer[12];
    void *value = &buffer[13];
    void *endianness_check = &buffer[8];

	//check if the message is long enough
	if (bufferSize < 8)
	{
		ModbusError(buffer, ERR_ILLEGAL_FUNCTION);
	}

	//****************** Read Coils **********************
	else if(buffer[7] == MB_FC_READ_COILS)
	{
		ReadCoils(buffer, bufferSize);
	}

	//*************** Read Discrete Inputs ***************
	else if(buffer[7] == MB_FC_READ_INPUTS)
	{
		ReadDiscreteInputs(buffer, bufferSize);
	}

	//****************** Read Holding Registers ******************
	else if(buffer[7] == MB_FC_READ_HOLDING_REGISTERS)
	{
		ReadHoldingRegisters(buffer, bufferSize);
	}

	//****************** Read Input Registers ******************
	else if(buffer[7] == MB_FC_READ_INPUT_REGISTERS)
	{
		ReadInputRegisters(buffer, bufferSize);
	}

	//****************** Write Coil **********************
	else if(buffer[7] == MB_FC_WRITE_COIL)
	{
		WriteCoil(buffer, bufferSize);
	}

	//****************** Write Register ******************
	else if(buffer[7] == MB_FC_WRITE_REGISTER)
	{
		WriteRegister(buffer, bufferSize);
	}

	//****************** Write Multiple Coils **********************
	else if(buffer[7] == MB_FC_WRITE_MULTIPLE_COILS)
	{
		WriteMultipleCoils(buffer, bufferSize);
	}

	//****************** Write Multiple Registers ******************
	else if(buffer[7] == MB_FC_WRITE_MULTIPLE_REGISTERS)
	{
		WriteMultipleRegisters(buffer, bufferSize);
	}

	//****************** Debug Info ******************
	else if(buffer[7] == MB_FC_DEBUG_INFO)
	{
        debugInfo(buffer);
	}

	//****************** Debug Get Trace ******************
	else if(buffer[7] == MB_FC_DEBUG_GET)
	{
        //field1 = startidx, field2 = endidx
        debugGetTrace(buffer, field1, field2);
	}

	//****************** Debug Get Trace List ******************
    else if(buffer[7] == MB_FC_DEBUG_GET_LIST)
	{
        //field1 = numIndexes
    	debugGetTraceList(buffer, field1, &buffer[10]);
	}
    
	//****************** Debug Set Trace ******************
	else if(buffer[7] ==MB_FC_DEBUG_SET)
	{
    	//field1 = varidx
        debugSetTrace(buffer, field1, flag, len, value);
	}

	//****************** Debug Get MD5 ******************
    else if(buffer[7] ==MB_FC_DEBUG_GET_MD5)
	{
        debugGetMd5(buffer, endianness_check);
	}

	//****************** Function Code Error ******************
	else
	{
		ModbusError(buffer, ERR_ILLEGAL_FUNCTION);
	}

	return MessageLength;
}
