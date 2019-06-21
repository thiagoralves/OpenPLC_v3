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
	//analog outputs
	if (Start <= MIN_16B_RANGE)
	{
		if (int_output[Start] != NULL)
		{
			*int_output[Start] = word(buffer[10],buffer[11]);
		}
	}
	//accessing memory
	//16-bit registers
	else if (Start >= MIN_16B_RANGE && Start <= MAX_16B_RANGE)
	{
		if (int_memory[Start - MIN_16B_RANGE] != NULL)
		{
			*int_memory[Start - MIN_16B_RANGE] = word(buffer[10],buffer[11]);
		}
	}
	//32-bit registers
	else if (Start >= MIN_32B_RANGE && Start <= MAX_32B_RANGE)
	{
		if (dint_memory[(Start - MIN_32B_RANGE)/2] != NULL)
		{
			uint32_t tempValue = (uint32_t)word(buffer[10],buffer[11]);

			if ((Start - MIN_32B_RANGE) % 2 == 0) //first word
			{
				*dint_memory[(Start - MIN_32B_RANGE) / 2] = *dint_memory[(Start - MIN_32B_RANGE) / 2] & 0x0000ffff;
				*dint_memory[(Start - MIN_32B_RANGE) / 2] = *dint_memory[(Start - MIN_32B_RANGE) / 2] | (tempValue << 16);
			}
			else //second word
			{
				*dint_memory[(Start - MIN_32B_RANGE) / 2] = *dint_memory[(Start - MIN_32B_RANGE) / 2] & 0xffff0000;
				*dint_memory[(Start - MIN_32B_RANGE) / 2] = *dint_memory[(Start - MIN_32B_RANGE) / 2] | tempValue;
			}
		}
		else
		{
			mb_holding_regs[Start] = word(buffer[10],buffer[11]);
		}
	}
	//64-bit registers
	else if (Start >= MIN_64B_RANGE && Start <= MAX_64B_RANGE)
	{
		if (lint_memory[(Start - MIN_64B_RANGE)/4] != NULL)
		{
			uint64_t tempValue = (uint64_t)word(buffer[10],buffer[11]);

			if ((Start - MIN_64B_RANGE) % 4 == 0) //first word
			{
				*lint_memory[(Start - MIN_64B_RANGE) / 4] = *lint_memory[(Start - MIN_64B_RANGE) / 4] & 0x0000ffffffffffff;
				*lint_memory[(Start - MIN_64B_RANGE) / 4] = *lint_memory[(Start - MIN_64B_RANGE) / 4] | (tempValue << 48);
			}
			else if ((Start - MIN_64B_RANGE) % 4 == 1) //second word
			{
				*lint_memory[(Start - MIN_64B_RANGE) / 4] = *lint_memory[(Start - MIN_64B_RANGE) / 4] & 0xffff0000ffffffff;
				*lint_memory[(Start - MIN_64B_RANGE) / 4] = *lint_memory[(Start - MIN_64B_RANGE) / 4] | (tempValue << 32);
			}
			else if ((Start - MIN_64B_RANGE) % 4 == 2) //third word
			{
				*lint_memory[(Start - MIN_64B_RANGE) / 4] = *lint_memory[(Start - MIN_64B_RANGE) / 4] & 0xffffffff0000ffff;
				*lint_memory[(Start - MIN_64B_RANGE) / 4] = *lint_memory[(Start - MIN_64B_RANGE) / 4] | (tempValue << 16);
			}
			else if ((Start - MIN_64B_RANGE) % 4 == 3) //fourth word
			{
				*lint_memory[(Start - MIN_64B_RANGE) / 4] = *lint_memory[(Start - MIN_64B_RANGE) / 4] & 0xffffffffffff0000;
				*lint_memory[(Start - MIN_64B_RANGE) / 4] = *lint_memory[(Start - MIN_64B_RANGE) / 4] | tempValue;
			}
		}
		else
		{
			mb_holding_regs[Start] = word(buffer[10],buffer[11]);
		}
	}
	else //invalid address
	{
		mb_error = ERR_ILLEGAL_DATA_ADDRESS;
	}
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
		//analog outputs
		if (position <= MIN_16B_RANGE)
		{
			if (int_output[position] != NULL) *int_output[position] =  word(buffer[13 + i * 2], buffer[14 + i * 2]);
		}
		//accessing memory
		//16-bit registers
		else if (position >= MIN_16B_RANGE && position <= MAX_16B_RANGE)
		{
			if (int_memory[position - MIN_16B_RANGE] != NULL) *int_memory[position - MIN_16B_RANGE] = word(buffer[13 + i * 2], buffer[14 + i * 2]);
		}
		//32-bit registers
		else if (position >= MIN_32B_RANGE && position <= MAX_32B_RANGE)
		{
			if (dint_memory[(Start - MIN_32B_RANGE)/2] != NULL)
			{
				uint32_t tempValue = (uint32_t)word(buffer[13 + i * 2], buffer[14 + i * 2]);

				if ((position - MIN_32B_RANGE) % 2 == 0) //first word
				{
					*dint_memory[(position - MIN_32B_RANGE) / 2] = *dint_memory[(position - MIN_32B_RANGE) / 2] & 0x0000ffff;
					*dint_memory[(position - MIN_32B_RANGE) / 2] = *dint_memory[(position - MIN_32B_RANGE) / 2] | (tempValue << 16);
				}
				else //second word
				{
					*dint_memory[(position - MIN_32B_RANGE) / 2] = *dint_memory[(position - MIN_32B_RANGE) / 2] & 0xffff0000;
					*dint_memory[(position - MIN_32B_RANGE) / 2] = *dint_memory[(position - MIN_32B_RANGE) / 2] | tempValue;
				}
			}
			else
			{
				mb_holding_regs[position] = word(buffer[13 + i * 2], buffer[14 + i * 2]);
			}
		}
		//64-bit registers
		else if (position >= MIN_64B_RANGE && position <= MAX_64B_RANGE)
		{
			if (lint_memory[(position - MIN_64B_RANGE)/4] != NULL)
			{
				uint64_t tempValue = (uint64_t)word(buffer[13 + i * 2], buffer[14 + i * 2]);

				if ((position - MIN_64B_RANGE) % 4 == 0) //first word
				{
					*lint_memory[(position - MIN_64B_RANGE) / 4] = *lint_memory[(position - MIN_64B_RANGE) / 4] & 0x0000ffffffffffff;
					*lint_memory[(position - MIN_64B_RANGE) / 4] = *lint_memory[(position - MIN_64B_RANGE) / 4] | (tempValue << 48);
				}
				else if ((Start - MIN_64B_RANGE) % 4 == 1) //second word
				{
					*lint_memory[(position - MIN_64B_RANGE) / 4] = *lint_memory[(position - MIN_64B_RANGE) / 4] & 0xffff0000ffffffff;
					*lint_memory[(position - MIN_64B_RANGE) / 4] = *lint_memory[(position - MIN_64B_RANGE) / 4] | (tempValue << 32);
				}
				else if ((Start - MIN_64B_RANGE) % 4 == 2) //third word
				{
					*lint_memory[(position - MIN_64B_RANGE) / 4] = *lint_memory[(position - MIN_64B_RANGE) / 4] & 0xffffffff0000ffff;
					*lint_memory[(position - MIN_64B_RANGE) / 4] = *lint_memory[(position - MIN_64B_RANGE) / 4] | (tempValue << 16);
				}
				else if ((Start - MIN_64B_RANGE) % 4 == 3) //fourth word
				{
					*lint_memory[(position - MIN_64B_RANGE) / 4] = *lint_memory[(position - MIN_64B_RANGE) / 4] & 0xffffffffffff0000;
					*lint_memory[(position - MIN_64B_RANGE) / 4] = *lint_memory[(position - MIN_64B_RANGE) / 4] | tempValue;
				}
			}
			else
			{
				mb_holding_regs[Start] = word(buffer[10],buffer[11]);
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
		MessageLength = 12;
	}
}

//-----------------------------------------------------------------------------
// This function must parse and process the client request and write back the
// response for it. The return value is the size of the response message in
// bytes.
//-----------------------------------------------------------------------------
int processModbusMessage(unsigned char *buffer, int bufferSize)
{
	MessageLength = 0;

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

	//****************** Function Code Error ******************
	else
	{
		ModbusError(buffer, ERR_ILLEGAL_FUNCTION);
	}

	return MessageLength;
}
