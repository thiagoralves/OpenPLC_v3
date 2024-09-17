//-----------------------------------------------------------------------------
// Copyright 2019 Thiago Alves
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
// This file has all the PCCC functions supported by the OpenPLC. If any
// other function is to be added to the project, it must be added here
// UAH, Sep 2019
//-----------------------------------------------------------------------------

//------------Libraries-------------//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "ladder.h"

//--------------------------------------------------------------Defines--------------------------------------------------------------------------------//

/*------------Maximum/Minimum Sizes for each buffer------------------*/
#define MAX_DISCRETE_INPUT              1024 // Digital Inputs
#define MAX_COILS                       1024 // Digital Outputs
#define MAX_HOLD_REGS                   1024 // Pure registers Analog Outputs
//#define MAX_INP_REGS                    1024 // Analog Inputs

#define MIN_16B_RANGE                   1024 //Holding Register Size 16bit (memory)
#define MAX_16B_RANGE                   2047 //Holding Register Size 16bit (memory)
#define MAX_32B_RANGE                   2047 //Holding Register Size 32bit (memory)

/*------------File Type for PCCC--------------*/
#define PCCC_INPUT_LOGICAL_SLOT			0x8c
#define PCCC_OUTPUT_LOGICAL_SLOT		0x8b
#define PCCC_INTEGER					0x89
#define PCCC_FLOATING_POINT				0x8A

#define PCCC_FN_OUTPUT					0x00
#define PCCC_FN_INPUT					0x01
#define PCCC_FN_INT						0x07
#define PCCC_FN_FLOAT					0x08

/*----------------Define functions for bit/byte operations-------------------*/
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define lowByte(w) ((unsigned char) ((w) & 0xff))
#define highByte(w) ((unsigned char) ((w) >> 8))
/*---------------------------------------------------------------------------*/

/*-----PLC Buffers - These buffer store the contents from OpenPLC-----------*/
IEC_BOOL pccc_discrete_input[MAX_DISCRETE_INPUT];
IEC_BOOL pccc_coils[MAX_COILS];
//IEC_UINT pccc_input_regs[MAX_INP_REGS];
IEC_UINT pccc_holding_regs[MAX_HOLD_REGS];
//-----------------------------------------------------------------------------------------------------------------------------------------------------//

thread_local int Pccc_MessageLength;

using namespace std;
//-----------------------------------------------------------Structure Defines--------------------------------------------------//
struct pccc_header //Structure for the Header Information for EthernetIP
{
	unsigned char *Data;    
	unsigned char *Data_Size;	
	
    unsigned char *HD_length = 5;//[5] -> Typical Header Length for Command; Response Header Length is 4
	unsigned char *HD_CMD_Code;//[1] -> Command Code
    unsigned char *HD_Status;//[1] -> Status Code
    unsigned char *HD_TransactionNum;//[2] -> Transaction Number
	unsigned char *HD_Data_Function_Code;//[1] -> Function code MSB
	unsigned char *HD_Ext_Status; //Ext Status -> only appended if Status = 0x0f[1]

	unsigned char resp_cod_hex = 0x4f; //Response Hex Value
	unsigned char *RP_CMD_Code = &resp_cod_hex;//[1] -> Reply Command Code = 0x4f
};

struct protected_logical_read_command //Struct for Reply and Command values of Read
{	
	unsigned char *CMD_Byte_Size;//[1]	
	unsigned char *RP_EXT_Status;//[1]* -> Ext Status -> only appended if Status = 0x0f[1]
};

struct protected_logical_write_command
{	
	unsigned char *CMD_Byte_Size;//[1]*
	unsigned char *RP_EXT_Status;//[1]* -> Ext Status -> only appended if Status = 0x0f[1]
};
//--------------------------------------------------------------------------------------------------------------------------------------//

//------------------------Function Declaration---------------------------------//

uint16_t Command_Protocol(pccc_header header,unsigned char *buffer, int buffer_size);
uint16_t ParsePCCCData(unsigned char *buffer, int buffer_size);
uint16_t Protected_Logical_Read_Reply(pccc_header, unsigned char *buffer, int buffer_size);
uint16_t Protected_Logical_Write_Reply(pccc_header, unsigned char *buffer, int buffer_size);

void Pccc_ReadCoils(unsigned char *buffer, int buffer_size);
void Pccc_WriteCoil(unsigned char *buffer, int buffer_size);
void Pccc_ReadDiscreteInputs(unsigned char *buffer, int buffer_size);
void Pccc_ReadHoldingRegisters(unsigned char *buffer, int buffer_size);
//void Pccc_ReadInputRegisters(unsigned char *buffer, int buffer_size);
void Pccc_WriteRegister(unsigned char *buffer, int buffer_size);

int word_pccc(unsigned char byte1, unsigned char byte2);
int an_word_pccc(unsigned char byte1, unsigned char byte2);

//----------------------------------------------------------------------------//

//This function takes in the data from enip.cpp and places the data in the appropriate structure variables
uint16_t processPCCCMessage(unsigned char *buffer, int buffer_size)
{
	/* Variables */
	int new_pccc_length; //New PCCC Length
	pccc_header header;
	header.Data = buffer;
	header.Data_Size = buffer_size;
	
	/*Determine the new pccc length*/
	new_pccc_length = ParsePCCCData(buffer,buffer_size);
	return new_pccc_length;	 //Return the length to enip.cpp
}

uint16_t ParsePCCCData(unsigned char *buffer, int buffer_size)
{	
	/*Variables*/
	int new_pccc_length; //Variable for new PCCC length
	pccc_header header;
	
	header.HD_CMD_Code = &buffer[0];//[1] -> Command Code
	header.HD_Status = &buffer[1];////[1] -> Status Code
	header.HD_TransactionNum = &buffer[2];//[2] -> Transaction Number
	header.HD_Data_Function_Code = &buffer[4];//[1] -> Data Function Code
	
	/*Determine what command is being requested*/
	new_pccc_length = Command_Protocol(header,buffer,buffer_size);
	
	return new_pccc_length; //Return the new pccc length
}

/* Determine the Command that is being requested to execute */
uint16_t Command_Protocol(pccc_header header, unsigned char *buffer, int buffer_size)
{
	uint16_t var_pccc_length;
	
	/*If Statement to determine the command code from the Command Packet*/
	if(((unsigned int)*header.HD_CMD_Code == 0x0f) && ((unsigned int)*header.HD_Data_Function_Code == 0xA2))//Protected Logical Read
	{	
		var_pccc_length = Protected_Logical_Read_Reply(header,buffer,buffer_size);
		return var_pccc_length;
	}
	else if(((unsigned int)*header.HD_CMD_Code == 0x0f) && ( ((unsigned int)*header.HD_Data_Function_Code == 0xAA) || ((unsigned int)*header.HD_Data_Function_Code == 0xAB)))//Protected Logical Write
	{	
		var_pccc_length = Protected_Logical_Write_Reply(header,buffer,buffer_size);
		return var_pccc_length;
	}
	else
	{
		/*initialize logging system*/
		char log_msg[1000];
		sprintf(log_msg, "PCCC: Unsupportedd Command/Data Function Code!\n");
		log(log_msg); 
		return -1;
	}//return length as -1 to signify that the CMD Code/Function Code was not recognize
}

uint16_t Protected_Logical_Read_Reply(pccc_header header, unsigned char *buffer, int buffer_size)
{
	/*Variables*/
	protected_logical_read_command protected_LR;
	protected_LR.CMD_Byte_Size = &buffer[5];//Byte Size of data to be read
	
	/*Determining Data Length*/
	unsigned int len_resp = 4;
	len_resp = len_resp + (unsigned int)*protected_LR.CMD_Byte_Size;
	
	/*check if the message is long enough- Left in for future error handling setup*/
	/*if (buffer_size < 8)
	{
		//PCCC Error Handling; Make sure that the buffer size is at least 8
	}*/

	//****************** Read Coils **********************//
	if(buffer[6] == PCCC_FN_OUTPUT && buffer[7] == PCCC_OUTPUT_LOGICAL_SLOT) // Done/Tested
	{
		Pccc_ReadCoils(buffer, buffer_size);
	}
	
	//*************** Read Discrete Inputs ***************//
	else if(buffer[6] == PCCC_FN_INPUT && buffer[7] == PCCC_INPUT_LOGICAL_SLOT)// Done/Tested
	{
		Pccc_ReadDiscreteInputs(buffer, buffer_size);
	}

	//****************** Read Holding Registers[PURE, 16Bit Mem, 32bit MEM] ******************//
	else if((buffer[6] == PCCC_FN_INT || buffer[6] == PCCC_FN_FLOAT)  && (buffer[7] == PCCC_INTEGER || buffer[7] == PCCC_FLOATING_POINT))//Done/Tested
	{
		Pccc_ReadHoldingRegisters(buffer, buffer_size);
	}
	else
	{
		char log_msg[1000];
		sprintf(log_msg, "PCCC: Error occured while processing Protected Logical Read\n");
		log(log_msg); 
		return -1;
	}//return length as -1 to signify that the CMD Code/Function Code was not recognize
	
	/*Creating the reply packet and memcpy the data into the buffer*/
	memmove(&buffer[0], header.RP_CMD_Code, 1); //0x4f Response Code
    memmove(&buffer[1], header.HD_Status, 1); //Same from COMMAND REQUEST
    memmove(&buffer[2], header.HD_TransactionNum, 2);//Same from COMMAND REQUEST
	
	return len_resp; //Return the Resonse Packet Length for PCCC	
}

uint16_t Protected_Logical_Write_Reply(pccc_header header,unsigned char *buffer, int buffer_size) // Connected
{	
	/*Variables*/
	protected_logical_write_command protected_LW;	
	protected_LW.CMD_Byte_Size = &buffer[5];//Byte Size of data to be read
	
	/*Determining link of new PCCC Packet*/
	uint16_t len_resp = header.HD_length - 1;
	
	/*Creating the reply packet and memcpy the data into the buffer*/
	memmove(&buffer[0], header.RP_CMD_Code, 1);
    memmove(&buffer[1], header.HD_Status, 1);
    memmove(&buffer[2], header.HD_TransactionNum, 2);
	
	/*check if the message is long enough- Left in for future error handling setup*/
	/*if (buffer_size < 8)
	{
		//PCCC Error Handling; Make sure that the buffer size is at least 8
	}*/
	
	//****************** Write Coil **********************//
	if(buffer[6] == PCCC_FN_OUTPUT && buffer[7] == PCCC_OUTPUT_LOGICAL_SLOT)// Done/Tested
	{
		Pccc_WriteCoil(buffer, buffer_size);
	}

	//****************** Write Register ******************//
	else if((buffer[6] == PCCC_FN_FLOAT || buffer[6] == PCCC_FN_INT) && (buffer[7] == PCCC_INTEGER || buffer[7] == PCCC_FLOATING_POINT))//Done/Tested
	{
		Pccc_WriteRegister(buffer, buffer_size);
	}

	//****************** Function Code Error ******************/
	/*Left in for future error handling setup*/
	else
	{
		//PCCC Error Handling; Make sure that the buffer size is at least 8. If none of the defined File Type and File Numbers match, error unrecognized File Type and File Number.
	}
	return len_resp;	
}

//-----------------------------------------------------------------------------
// Concatenate two bytes into an int
//-----------------------------------------------------------------------------
int word_pccc(unsigned char byte1, unsigned char byte2)
{
	int returnValue;
	returnValue = (int)(byte1) | (int)byte2;

	return returnValue;
}
//-----------------------------------------------------------------------------
// Concatenate two bytes into an int
//-----------------------------------------------------------------------------
int an_word_pccc(unsigned char byte1, unsigned char byte2)
{
	int returnValue;
	returnValue = (int)(byte1) | (int)(byte2 << 8);

	return returnValue;
}

//-----------------------------------------------------------------------------
// Implementation of PCCC Read Coils
//-----------------------------------------------------------------------------
void Pccc_ReadCoils(unsigned char *buffer, int buffer_size) //Working QX Read
{
	int Start, ByteDataLength, Mask;
	
	/*check if the message is long enough- Left in for future error handling setup*/
	/*if (buffer_size < 10)
	{
		//PCCC Error Handling (Fill in?); This Request must have at least 10 bytes. If it doesn't, its a corrupted message
	}*/
	
	Start = word_pccc(buffer[8],buffer[9]); //Start based on the Element and Subelemnt values in the Command Packet
	Mask = log2( word_pccc(buffer[10],buffer[11]) ); //Save the byte size or byte data length to the variable from the command packet
	ByteDataLength = buffer[5];
	pthread_mutex_lock(&bufferLock);
	
	/*----Reading the values from the PLC bool_output buffer and writing to the PCCC buffer based on position----*/
	for (int i = 0; i < ByteDataLength; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			int position = Start + i * 8 + j;
			if (position < MAX_COILS)
			{
				if(bool_output[position/8][position%8] != NULL)
				{
					bitWrite(buffer[4+i], j, *bool_output[position/8][position%8]);
				}
				else
				{
					bitWrite(buffer[4+i],j,0);
				}
			}
			else
			{
				//PCCC Error Handling (Fill in?); If the position is greater than the MAX COILS, ERROR Overflow?
			}
		}
	}
	pthread_mutex_unlock(&bufferLock);
	
	/*Left in for future error handling setup*/
	/*if (pccc_error != ERR_NONE)
	{
		//PCCC Error Handling (Fill in?); Deetermine if there was an error:
	}*/
}

//-----------------------------------------------------------------------------
// Implementation of PCCC Read Discrete Inputs
//-----------------------------------------------------------------------------
void Pccc_ReadDiscreteInputs(unsigned char *buffer, int buffer_size) //Working IX Read Only
{
	int Start, ByteDataLength;
	
	/*This Request must have at least 10 bytes. If it doesn't, its a corrupted messageLeft in for future error handling setup*/
	/*if (buffer_size < 10)
	{
		//PCCC Error Handling (Fill in?); This Request must have at least 10 bytes. If it doesn't, its a corrupted message
	}*/
	
	Start = word_pccc(buffer[8],buffer[9]);//Start based on the Element and Subelemnt values in the Command Packet
	ByteDataLength = buffer[5];//Save the byte size or byte data length to the variable from the command packet
	pthread_mutex_lock(&bufferLock);
	
	/*--------Reading the values from the PLC bool_input buffer and writing to the PCCC buffer based on position--------*/
	for (int i = 0; i < ByteDataLength; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			int position = Start + i * 8 + j;
			if (position < MAX_DISCRETE_INPUT)
			{
				if(bool_input[position/8][position%8] != NULL)
				{
					bitWrite(buffer[4+i], j, *bool_input[position/8][position%8]);
				}
				else
				{
					bitWrite(buffer[4+i],j,0);
				}
			}
			else
			{
				//PCCC Error Handling (Fill in?); If the position is greater than the MAX, ERROR Overflow?
			}
		}
	}
	pthread_mutex_unlock(&bufferLock);
	
	/*Left in for future error handling setup*/
	/*if (mb_error != ERR_NONE)
	{
		//PCCC Error Handling (Fill in?); Deetermine if there was an error:
	}*/
	
}

//-----------------------------------------------------------------------------
// Implementation of PCCC Read Holding Registers
//-----------------------------------------------------------------------------
void Pccc_ReadHoldingRegisters(unsigned char *buffer, int buffer_size) // QW Read
{	
	int Start, an_Start, WordDataLength, ByteDataLength;

	/*this request must have at least 10 bytes. If it doesn't, it's a corrupted message - Left in for future error handling setup*/
	/*if (buffer_size < 10)
	{
		//PCCC Error Handling (Fill in?); This Request must have at least 10 bytes. If it doesn't, its a corrupted message
	}*/

	Start = word_pccc(buffer[8],buffer[9]);//Start based on the Element and Subelemnt values in the Command Packet
	ByteDataLength = buffer[5];//Save the byte size or byte data length to the variable from the command packet
	WordDataLength = ByteDataLength / 2;//Calculate the word data length based on the byte data length
	unsigned int Temp_FileT = buffer[7];//Value will be changed potentially during this process, save the File Type Value from command packet
	unsigned int Temp_FileN = buffer[6];//Value will be changed potentially during this process, save the File Number Value from command packet

	/*asked for too many registers - Left in for future error handling setup*/
	/*if (ByteDataLength > 255)
	{
		//PCCC Error Handling (Fill in?); This Request must have at greater than 255 bytes. If it does, its a corrupted message
		//return;
	}*/

	pthread_mutex_lock(&bufferLock);
	/*--------Reading the values from the PLC int_output, int_memory, and dint_memory buffer and writing to the PCCC buffer based on position--------*/
	for(int i = 0; i < WordDataLength; i++)
	{
		int position = Start + i;
		//int an_position = an_Start + i;
		if ((position <= MIN_16B_RANGE) && (Temp_FileN == PCCC_FN_INT && Temp_FileT == PCCC_INTEGER))
		{
			if (int_output[position] != NULL)
			{
				buffer[ 4 + position * 2] = lowByte(*int_output[position]);
				buffer[5 + position * 2] = highByte(*int_output[position]);
			}
			else
			{
				buffer[ 4 + position * 2] = 0;
				buffer[5 + position * 2] = 0;
			}
		}
		//accessing memory
		//16-bit registers
		else if ((position >= MIN_16B_RANGE && position <= MAX_16B_RANGE) && (Temp_FileN == PCCC_FN_INT && Temp_FileT == PCCC_INTEGER))
		{
			if (int_memory[position - MIN_16B_RANGE] != NULL)
			{
				buffer[ 4 + position * 2] = lowByte(*int_memory[position - MIN_16B_RANGE]);
				buffer[5 + position * 2] = highByte(*int_memory[position - MIN_16B_RANGE]);
			}
			else
			{
				buffer[ 4 + position * 2] = 0;
				buffer[5 + position * 2] = 0;
			}
		}
		
		//32-bit registers
		else if (Temp_FileN == PCCC_FN_FLOAT && Temp_FileT == PCCC_FLOATING_POINT && (position % 2 == 0))
		{
			position = position/2;
			uint32_t tempValue = *dint_memory[position];
			
			buffer[4+(4*position)] = tempValue;
			buffer[5+(4*position)] = tempValue >> 8;
			buffer[6+(4*position)] = tempValue >> 16;
			buffer[7+(4*position)] = tempValue >> 24;
		
		}
		/*Left in for future error handling setup-Invalid Address*/
		else
		{
			//PCCC Error Handling (Fill in?); If none of the above are recognized, error
		}

	}
	pthread_mutex_unlock(&bufferLock);

}

//-----------------------------------------------------------------------------
// Implementation of PCCC Write Coil
//-----------------------------------------------------------------------------
 void Pccc_WriteCoil(unsigned char *buffer, int buffer_size) //QX Write NEEDS WRITE MULTIPLE
 {
	int Start, Mask;
	int mask_offset = 0;
	
	/*Left in for future error handling setup*/
	/*if(buffer_size < 10)
	{
		//ModbusError(buffer, ERR_ILLEGAL_DATA_ADDRESS);
		//return;
	}
	*/
	
	/*For the Write Mask, there has to be a maskoffset due to an extra two bytes */
	if((unsigned int)buffer[4] == 0xAB)
	{
		mask_offset = buffer[5]; //Byte Size 
	}
	
	Start = word_pccc(buffer[8],buffer[9]);//Start based on the Element and Subelemnt values in the Command Packet
	Mask = log2( word_pccc(buffer[10],buffer[11]) );// Maskoffset based on the mask value in Masked Protected Write Command Packet
	
	/*--------Determines if the values inside the PCCC data has a 1 or 0 in it. Writes that value to the bool_output based on the contents of the data in PCCC Buffer-------*/
	if(Start < MAX_COILS)
	{
		unsigned char value;
		if(word_pccc(buffer[10 + mask_offset],buffer[11 + mask_offset]) > 0)
		{
			value = 1;
		}
		else
		{
			value = 0; 
		}
		pthread_mutex_lock(&bufferLock);
		if(bool_output[Start][Mask] != NULL)
		{
			*bool_output[Start][Mask] = value;
		}
		pthread_mutex_unlock(&bufferLock);
	}
	
}


//-----------------------------------------------------------------------------
// Implementation of PCCC Write Holding Register
//-----------------------------------------------------------------------------
void Pccc_WriteRegister(unsigned char *buffer, int buffer_size) // QW Write
{
	int Start, WordDataLength, ByteDataLength;

	Start = word_pccc(buffer[8],buffer[9]);//Start based on the Element and Subelemnt values in the Command Packet
	int an_Start = an_word_pccc(buffer[8],buffer[9]);//Different Start method for INTs based on the Element and Subelemnt values in the Command Packet
	ByteDataLength = buffer[5];//Save the byte size or byte data length to the variable from the command packet
	WordDataLength = ByteDataLength / 2;//Calculate the word data length based on the byte data length
	unsigned int Temp_FileT = buffer[7];//Value will be changed potentially during this process, save the File Type Value from command packet
	unsigned int Temp_FileN = buffer[6];//Value will be changed potentially during this process, save the File Number Value from command packet

	pthread_mutex_lock(&bufferLock);
	
	/*--------Determines if the values inside the PCCC data has data. Writes that value to the appropriate PLC Buffer based on the contents of the data in PCCC Buffer-------*/
	for(int i = 0; i < WordDataLength; i++)
	{
		int position = Start + i;
		//analog outputs
		if ((position <= MIN_16B_RANGE) && (Temp_FileN == PCCC_FN_INT && (Temp_FileT == PCCC_INTEGER)))
		{
			if (int_output[position] != NULL) *int_output[position] =  an_word_pccc(buffer[10 + i], buffer[11 + i]);//look at this closer
		}
		//accessing memory
		//16-bit registers
		else if ((position >= MIN_16B_RANGE && position <= MAX_16B_RANGE) && (Temp_FileN == PCCC_FN_OUTPUT && (Temp_FileT == PCCC_INTEGER)))
		{
			if (int_memory[position - MIN_16B_RANGE] != NULL) *int_memory[position - MIN_16B_RANGE] = an_word_pccc(buffer[10 + i], buffer[11 + i]);//look at this closer
		}
		//32-bit registers
		if (Temp_FileN == PCCC_FN_FLOAT && (Temp_FileT == PCCC_FLOATING_POINT))
		{
			if (dint_memory[position] != NULL)
			{
				uint32_t tempValue = buffer[10 + i] | buffer[11 + i] << 8 | buffer[12 + i] << 16 | buffer[13 + i] <<24;//look at this closer
				
				*dint_memory[position] = tempValue;
				
				i += 4;
			}
			else
			{
				pccc_holding_regs[position] = an_word_pccc(buffer[10 + i], buffer[11 + i]);//look at this closer might need to copy from temp
			}
		}
	
	pthread_mutex_unlock(&bufferLock);

	}
}