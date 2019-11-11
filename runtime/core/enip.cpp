//-----------------------------------------------------------------------------
// Copyright 2019 Thiago Alves
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

// This file has all the EtherNet/IP functions supported by the OpenPLC. If any
// other function is to be added to the project, it must be added here
// Hannah Hanback, Sep 2019
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <spdlog/spdlog.h>

#include "ladder.h"
#include "enip.h"

/** \addtogroup openplc_runtime
 *  @{
 */

#define ENIP_MIN_LENGTH     28

using namespace std;

thread_local unsigned char enip_session[4];	// opens thread for session

struct enip_header
{
    unsigned char *command;//[2];
    unsigned char *length;//[2];
    unsigned char *session_handle;//[4];
    unsigned char *status;//[4];
    unsigned char *sender_context;//[8];
    unsigned char *options;//[4];
    unsigned char *data;
};

struct enip_data_Unknown
{
    unsigned char *interface_handle;//[4]
    unsigned char *timeout;//[2]
    unsigned char *item_count;//[2]
    
    unsigned char *item1_id;//[2]
    unsigned char *item1_length;//[2]
    unsigned char *item1_data;//[1]
    
    unsigned char *item2_id;//[2]
    unsigned char *item2_length;//[2]
    unsigned char *item2_data;
};


struct enip_data_Unconnected
{
	unsigned char *interface_handle;//[4]
    unsigned char *timeout;//[2]
    unsigned char *item_count;//[2]
	
	unsigned char *item1_id;//[2]
	unsigned char *item1_length;//[2]
	
	unsigned char *item2_id;//[2]
	unsigned char *item2_length;//[2]
	
	unsigned char *service;//[1]   0x4b (Request)
	unsigned char *request_pathSize;//[1]
	unsigned char *request_path;//[4]
	unsigned char *requestor_idLength;//[1]
	unsigned char *vendor_id;//[2]
	unsigned char *serial_number;//[4]
	unsigned char *data;
};


struct enip_data_Connected
{
	unsigned char *interface_handle;//[4]
    unsigned char *timeout;//[2]
    unsigned char *item_count;//[2]
	
	unsigned char *item1_id;//[2]
	unsigned char *item1_length;//[2]
	
	unsigned char *item2_id;//[2]
	unsigned char *item2_length;//[2]
	
	unsigned char *service;//[1]   0x4b (Request)
	unsigned char *request_pathSize;//[1] -----------size in words
	unsigned char *request_path;//[4]
	unsigned char *actual_timeout;//[2]
	unsigned char *o2t_netConnectID;//[4]
	unsigned char *t2o_netConnectID;//[4]
	unsigned char *connect_serialNo;//[2]
	unsigned char *orig_vendorNo;//[2]
	unsigned char *orig_serialNo;//[4]
	unsigned char *timeout_multiplier;//[1]
	unsigned char *reserved;//[3]
	unsigned char *o2t_rpi;//[4]
	unsigned char *o2t_netConnectParam;//[2]
	unsigned char *t2o_rpi;//[4]
	unsigned char *t2o_netConnectParam;//[2]
	unsigned char *transport_trigger;//[1]
	unsigned char *connection_pathSize;//[1] ----- size in words
	unsigned char *connection_path;//[?]
};

struct enip_data_Connected_0x70
{
	unsigned char *interface_handle;//[4]
    unsigned char *timeout;//[2]
    unsigned char *item_count;//[2]
	
	unsigned char *item1_id;//[2]  --- 0x a1 00
	unsigned char *item1_length;//[2] --- 0x 04 00
	unsigned char *connection_id;//[4]
	
	unsigned char *item2_id;//[2] --- 0x b1 00
	unsigned char *item2_length;//[2]	length in bytes after this byte
	unsigned char *sequence_count;//[2]
	
	unsigned char *service;//[1] --- 0x4b
	unsigned char *request_pathSize;//[1] --- 0x 20 67 24 01
	unsigned char *request_path;//[4]
	unsigned char *requestor_id;//[7]
	unsigned char *pcccData;//[?]
};


//-----------------------------------------------------------------------------
// Obtains the Length in Item2_Length as variable type uint16_t
// used to know the length of pccc data within Item2_Data
// ENIP Type: Unknown
//-----------------------------------------------------------------------------
int getEnipType(struct enip_data_Unknown *enipDataUnknown, struct enip_header *header)
{	

		if (header->command[0] == 65 || header->command[0] == 0x70)
		{
			// ENIP Type Unnecessary for command execution
			// 0x65 --- register session
			return 0;
		}
		else if (enipDataUnknown->item1_id[0] == 0x81)
		{
			// PCCC type 1 - Unknown
			return 1;
		}
		else if (enipDataUnknown->item1_data[0] == 0xb2 && enipDataUnknown->item2_length[1] == 0x4b) // There is an offset of the bytes within the 
		{																								   // Unconnected and Connected type data
			// PCCC type 2 - Unconnected Data Item														   // that is accounted for to check enipType														   // This means the labels "item1_data and item2_length
			return 2;																					   // do not contain what is stated but what it would be for the correct type
		}																								  
		else if (enipDataUnknown->item1_data[0] == 0xb2 && ( (enipDataUnknown->item2_length[1]==0x54) || (enipDataUnknown->item2_length[1]==0x4e) ) )	//0x54 opens connection
		{																																			    //0x4e closes connection
			// PCCC type 3 - Connected Data Item																										
			return 3;
		}
		else if (enipDataUnknown->item1_id[0] == 0xa1)
		{
			// PCCC type 3 - Connected for 0x70 command SEND UNIT DATA	
			return 3;
		}
		else
		{
			// Unknown type ID
			// Respond with error_code
			return -1;
		}
}


//-----------------------------------------------------------------------------
// Obtains the Length in the Header Length as variable type uint16_t
// used to know the length of CIP object data
//-----------------------------------------------------------------------------
uint16_t get_HeaderLength(struct enip_header *header)
{	
	uint16_t dataLength = ((uint16_t)header->length[1] << 8) | (uint16_t)header->length[0];
	return dataLength;
}


//-----------------------------------------------------------------------------
// Obtains the Length in Item2_Length as variable type uint16_t
// used to know the length of pccc data within Item2_Data
//-----------------------------------------------------------------------------
uint16_t get_Item2_DataLength(struct enip_data_Unknown *enipDataUnknown)
{	
	uint16_t dataLength = ((uint16_t)enipDataUnknown->item2_length[1] << 8) | (uint16_t)enipDataUnknown->item2_length[0];
	return dataLength;
}


//-----------------------------------------------------------------------------
// Obtains the Length in Item2_Length as variable type uint16_t
// used to know the length of pccc data within Item2_Data
// ENIP Type: Unconnected
//-----------------------------------------------------------------------------
uint16_t get_Item2_DataLength_Unconnected(struct enip_data_Unconnected *enipDataUnconnected)
{	
	uint16_t dataLength = ((uint16_t)enipDataUnconnected->item2_length[1] << 8) | (uint16_t)enipDataUnconnected->item2_length[0];
	return dataLength;
}


//-----------------------------------------------------------------------------
// Obtains the Length in Item2_Length as variable type uint16_t
// used to know the length of pccc data within Item2_Data
// ENIP Type: Connected_0x70
//-----------------------------------------------------------------------------
uint16_t get_Item2_DataLength_Connected_0x70(struct enip_data_Connected_0x70 *enipDataConnected_0x70)
{	
	uint16_t dataLength = ((uint16_t)enipDataConnected_0x70->item2_length[1] << 8) | (uint16_t)enipDataConnected_0x70->item2_length[0];
	return dataLength;
}


//-----------------------------------------------------------------------------
// Parses the Header information into struct
//-----------------------------------------------------------------------------  
int parseEnipHeader(unsigned char *buffer, int buffer_size, struct enip_header *header, struct enip_data_Unknown *enipDataUnknown)
{	
    //verify if message is big enough
    if (buffer_size < ENIP_MIN_LENGTH)
        return -1;
    
    header->command = &buffer[0];
    header->length = &buffer[2];
    header->session_handle = &buffer[4];
    header->status = &buffer[8];
    header->sender_context = &buffer[12];
    header->options = &buffer[20];
    header->data = &buffer[24];
    
    uint16_t enip_data_size = ((uint16_t)header->length[1] << 8) | (uint16_t)header->length[0];
	
    //verify if buffer_size matches enip_data_size
    if (buffer_size - 24 < enip_data_size)
        return -1;

    return enip_data_size;
}


//-----------------------------------------------------------------------------
// Parses the ENIP data from the buffer
// ENIP Type: UNKNOWN
//-----------------------------------------------------------------------------
void parseEnipUnknown(unsigned char *buffer, struct enip_data_Unknown *enipDataUnknown)
{
	enipDataUnknown->interface_handle = &buffer[24];
	enipDataUnknown->timeout = &buffer[28];
	enipDataUnknown->item_count = &buffer[30];
	
	enipDataUnknown->item1_id = &buffer[32];
	enipDataUnknown->item1_length = &buffer[34];
	enipDataUnknown->item1_data = &buffer[36];
	
	enipDataUnknown->item2_id = &buffer[37];
	enipDataUnknown->item2_length = &buffer[39];
	enipDataUnknown->item2_data = &buffer[41];
}


//-----------------------------------------------------------------------------
// Parses the ENIP data from the buffer
// ENIP Type: Unconnected
//-----------------------------------------------------------------------------
void parseEnipUnconnected(unsigned char *buffer, struct enip_data_Unconnected *enipDataUnconnected)
{
	enipDataUnconnected->interface_handle = &buffer[24];
    enipDataUnconnected->timeout = &buffer[28];
    enipDataUnconnected->item_count = &buffer[30];
	
	enipDataUnconnected->item1_id = &buffer[32];
	enipDataUnconnected->item1_length = &buffer[34];
	
	enipDataUnconnected->item2_id = &buffer[36];
	enipDataUnconnected->item2_length = &buffer[38];
	
	enipDataUnconnected->service = &buffer[40];   //0x4b (Request)
	enipDataUnconnected->request_pathSize = &buffer[41];//[1]
	enipDataUnconnected->request_path = &buffer[42];//[4]
	enipDataUnconnected->requestor_idLength = &buffer[46];//[1]
	enipDataUnconnected->vendor_id = &buffer[47];//[2]
	enipDataUnconnected->serial_number = &buffer[49];//[4]
	enipDataUnconnected->data = &buffer[53];
}


//-----------------------------------------------------------------------------
// Parses the ENIP data from the buffer
// ENIP Type: Connected
//-----------------------------------------------------------------------------
void parseEnipConnected(unsigned char *buffer, struct enip_data_Connected *enipDataConnected)
{
	enipDataConnected->interface_handle = &buffer[24];//[4]
    enipDataConnected->timeout = &buffer[28];//[2]
    enipDataConnected->item_count = &buffer[30];//[2]
	
	enipDataConnected->item1_id = &buffer[32];//[2]
	enipDataConnected->item1_length = &buffer[34];//[2]
	
	enipDataConnected->item2_id = &buffer[36];//[2]
	enipDataConnected->item2_length = &buffer[38];//[2]
	
	enipDataConnected->service = &buffer[40];//[1]   0x4b (Request)
	enipDataConnected->request_pathSize = &buffer[41];//[1] -----------size in words
	enipDataConnected->request_path = &buffer[42];//[4]
	enipDataConnected->actual_timeout = &buffer[46];//[2]
	enipDataConnected->o2t_netConnectID = &buffer[48];//[4]
	enipDataConnected->t2o_netConnectID = &buffer[52];//[4]
	enipDataConnected->connect_serialNo = &buffer[56];//[2]
	enipDataConnected->orig_vendorNo = &buffer[58];//[2]
	enipDataConnected->orig_serialNo = &buffer[60];//[4]
	enipDataConnected->timeout_multiplier = &buffer[64];//[1]
	enipDataConnected->reserved = &buffer[65];//[3]
	enipDataConnected->o2t_rpi = &buffer[68];//[4]
	enipDataConnected->o2t_netConnectParam = &buffer[72];//[2]
	enipDataConnected->t2o_rpi = &buffer[74];//[4]
	enipDataConnected->t2o_netConnectParam = &buffer[78];//[2]
	enipDataConnected->transport_trigger = &buffer[80];//[1]
	enipDataConnected->connection_pathSize = &buffer[81];//[1] ----- size in words
	enipDataConnected->connection_path = &buffer[82];//[?]
}


void parseEnipDataConnected_0x70(unsigned char *buffer, struct enip_data_Connected_0x70 *enipDataConnected_0x70)
{
	enipDataConnected_0x70->interface_handle = &buffer[24];
	enipDataConnected_0x70->timeout = &buffer[28];
	enipDataConnected_0x70->item_count = &buffer[30];
	
	enipDataConnected_0x70->item1_id = &buffer[32];
	enipDataConnected_0x70->item1_length = &buffer[34];
	enipDataConnected_0x70->connection_id = &buffer[36];
	
	enipDataConnected_0x70->item2_id = &buffer[40];
	enipDataConnected_0x70->item2_length = &buffer[42];
	enipDataConnected_0x70->sequence_count = &buffer[44];
	
	enipDataConnected_0x70->service = &buffer[46];
	enipDataConnected_0x70->request_pathSize = &buffer[47];
	enipDataConnected_0x70->request_path = &buffer[48];
	enipDataConnected_0x70->requestor_id = &buffer[52];
	enipDataConnected_0x70->pcccData = &buffer[59];
}


//-----------------------------------------------------------------------------
// Registers a ENIP Session
// Command Code: 0x65
//-----------------------------------------------------------------------------  
int registerEnipSession(struct enip_header *header)
{	
    unsigned char r[4];
    srand((unsigned)time(NULL));

    for (int i = 0; i < 4; ++i) 
        r[i] = rand();
    
    memcpy(header->session_handle, &r[0], 4);
    memcpy(&enip_session[0], &r[0], 4);
    
    return ENIP_MIN_LENGTH;
}


//-----------------------------------------------------------------------------
// SendRRData
// Receives a PCCC msg and Responds
// Command Code: 0x65
//-----------------------------------------------------------------------------  
int sendRRData(int enipType, struct enip_header *header, struct enip_data_Unknown *enipDataUnknown, struct enip_data_Unconnected *enipDataUnconnected, struct enip_data_Connected *enipDataConnected)
{
	if (enipType == 1)
	{	

		//writeDataContents(enipDataUnknown);

		uint16_t currentHeaderLength = get_HeaderLength(header); // get length of current stored size
		uint16_t currentPcccSize = get_Item2_DataLength(enipDataUnknown); // get length of stored pccc size
	
		//change timeout value
		enipDataUnknown->timeout[0] = 0x00;
		enipDataUnknown->timeout[1] = 0x04;
		
		//get pointer to beginning of pccc data to be passed
		unsigned char* pcccData = enipDataUnknown->item2_data;
	
		//send pccc Data to pccc.cpp to be parsed and craft response
		// returns the new PCCC data size
		uint16_t newPcccSize = processPCCCMessage(pcccData, currentPcccSize);
		if (newPcccSize == -1)
			return -1;	//error in PCCC.cpp
	
		//change enipDataUnknown->item2_length to match new pccc data size
		enipDataUnknown->item2_length[0] = newPcccSize & 0xFF;
		enipDataUnknown->item2_length[1] = newPcccSize >> 8;
		
		//calculate new header length size
		uint16_t len = currentHeaderLength - (currentPcccSize - newPcccSize);
	
		//change header->length to match new enip data size
		header->length[0] = len & 0xFF;
		header->length[1] = len >> 8;
		
		//calculate total size of enip response message in bytes
		uint16_t messageSize = len + 24;
	
		return messageSize; // total message size in bytes
	}
	else if (enipType == 2)
	{	
		//change timeout value
		enipDataUnconnected->timeout[0] = 0x00;
		enipDataUnconnected->timeout[1] = 0x04;
		
		uint16_t currentHeaderLength = get_HeaderLength(header); // get length of current stored size
		uint16_t currentItem2Size = get_Item2_DataLength_Unconnected(enipDataUnconnected); // get length of stored pccc size
	
		//get pointer to beginning of pccc data to be passed
		unsigned char* pcccData = enipDataUnconnected->data;
	
		//send pccc Data to pccc.cpp to be parsed and craft response
		// returns the new PCCC data size
		uint16_t newPcccSize = processPCCCMessage(pcccData, currentItem2Size - 13); // get length of new pccc size
		if (newPcccSize == -1)
			return -1;	//error in PCCC.cpp
		
		//item2_length is the length of the PCCC Data + 11 (11 for number of bytes after item2_length excluding PCCC Data)
		uint16_t newItem2Size = 11 + newPcccSize;
		
		//change enipDataUnconnected->item2_length to match new pccc data size
		enipDataUnconnected->item2_length[0] = newItem2Size & 0xFF;
		enipDataUnconnected->item2_length[1] = newItem2Size >> 8;
		
		//calculate new header length size
		uint16_t newHeaderLength = currentHeaderLength - (currentItem2Size - newItem2Size);
		
		//change header->length to match new enip data size
		header->length[0] = newHeaderLength & 0xFF;
		header->length[1] = newHeaderLength >> 8;
		
		//change service 0x4b to 0xcb
		enipDataUnconnected->service[0] = 0xcb;
		
		//change request path to 0
		enipDataUnconnected->request_pathSize[0] = 0x00;
		enipDataUnconnected->request_path[0] = 0x00;
		enipDataUnconnected->request_path[1] = 0x00;
		
		//move data forward
		memmove(&enipDataUnconnected->request_path[2], enipDataUnconnected->requestor_idLength, newPcccSize + 7);//11);
		
		//obtain total size of response message in bytes
		uint16_t messageSize = newHeaderLength + 24; // 24 is the static header size
		
		return messageSize; // total message size in bytes
	}
	else if (enipType == 3)
	{
		//change timeout value
		enipDataConnected->timeout[0] = 0x00;
		enipDataConnected->timeout[1] = 0x04;
		
		//change item2_length value (always 30?)
		enipDataConnected->item2_length[0] = 0x1e;
		enipDataConnected->item2_length[1] = 0x00;
		
		//change service response  0x54->0xd4
		enipDataConnected->service[0] = 0xd4;
		
		//change request path to 0
		enipDataConnected->request_pathSize[0] = 0x00;
		enipDataConnected->request_path[0] = 0x00;
		enipDataConnected->request_path[1] = 0x00;
		
		// change o2t_netConnectID
		enipDataConnected->request_path[2] = 0x5a;
		enipDataConnected->request_path[3] = 0xf0;
		enipDataConnected->actual_timeout[0] = 0xb8;
		enipDataConnected->actual_timeout[1] = 0x01;
		
		// start at the back and move up forward
		
		// overwrite t2o_netConnectParam with response of reserved 0x00 00
		enipDataConnected->t2o_netConnectParam[0] = 0x00;
		enipDataConnected->t2o_netConnectParam[1] = 0x00;
		
		// move up to overwrite o2t_netConnectParam
		memmove(&enipDataConnected->o2t_netConnectParam[0], enipDataConnected->t2o_rpi, 6); //6 = 80841e00 00 00 
		
		// move to overwrite timeout multiplier
		memmove(&enipDataConnected->timeout_multiplier[0], enipDataConnected->o2t_rpi, 10);//10 = 80841e00 80841e00 0000
		
		// move to overwrite o2t_netConnectID
		memmove(&enipDataConnected->o2t_netConnectID[0], enipDataConnected->t2o_netConnectID, 22);
		
		//change length inside header
		header->length[0] = 0x2e;
		header->length[1] = 0x00;
	
		//calculate total size of response message in bytes
			// this will be length from header +24
			// uint16_t enip_dataSize = len + 24;
		uint16_t messageSize = 70;
		
		return messageSize;
		
	}
	else
	{
		// log error to openPLC
		return -1;
	}
	
	
}


//-----------------------------------------------------------------------------
// SendUnitData
// Receives a PCCC msg and Responds
// Command Code: 0x70
//-----------------------------------------------------------------------------  
int sendUnitData(struct enip_header *header, struct enip_data_Connected_0x70 *enipDataConnected_0x70)
{
	//change the service response 0x4b -> 0xcb
	enipDataConnected_0x70->service[0] = 0xcb;
	
	//overwrite request path
	enipDataConnected_0x70->request_pathSize[0] = 0x00;
	enipDataConnected_0x70->request_path[0] = 0x00;
	enipDataConnected_0x70->request_path[1] = 0x00;
	
	//get pointer to beginning of pccc data to be passed
	unsigned char* pcccData = enipDataConnected_0x70->pcccData;
	
	int masked_write_check = 0;
	int write_check = 0;
	int write_check_2 = 0;
	if(pcccData[4] == 0xab)
	{
		masked_write_check = pcccData[5]; //Byte Size
	}
	
	if(pcccData[4] == 0xaa)
	{
		//write_check = 2;
		write_check = 2;
	}
	if(pcccData[4] == 0xa2 && pcccData[7] == 0x8a)
	{
		write_check_2 = -6;
	}
	//if(pcccData[4] == 0xa2 && pcccData[7] == 0x89)
	//{
		//write_check_2 = -2;
	//}
	
	//get the current Item2_Length
	uint16_t currentItem2Size = get_Item2_DataLength_Connected_0x70(enipDataConnected_0x70);
	
	//calculate the currentPcccSize
	uint16_t currentPcccSize = abs(currentItem2Size - 15);
	
	//send pccc Data to pccc.cpp to be parsed and craft response
	// returns the new PCCC data size
	uint16_t newPcccSize = processPCCCMessage(pcccData, currentPcccSize);
	if (newPcccSize == -1)
		return -1;	//error in PCCC.cpp
		
	//calculate Data Sizes
	uint16_t newItem2Size = newPcccSize + 13;
	uint16_t newHeaderSize = newPcccSize + newItem2Size + 14 + masked_write_check + write_check + write_check_2; // Use to be '+ 16' uint16_t newHeaderSize = newPcccSize + newItem2Size + 16;
	
	//change item2_length to match new cip data size
	enipDataConnected_0x70->item2_length[0] = newItem2Size & 0xFF;
	enipDataConnected_0x70->item2_length[1] = newItem2Size >> 8;
	
	//change header->length to match new enip data size
	header->length[0] = newHeaderSize & 0xFF;
	header->length[1] = newHeaderSize >> 8;
	
	//move data forward
	memmove(&enipDataConnected_0x70->request_path[2], enipDataConnected_0x70->requestor_id, newPcccSize + 7);
	
	//calculate total size of enip response message in bytes
	uint16_t messageSize = newHeaderSize + 24;
	
	return messageSize; // total message size in bytes
}


//-----------------------------------------------------------------------------
// This function must parse and process the client request and write back the
// response for it. The return value is the size of the response message in
// bytes.
//-----------------------------------------------------------------------------
int processEnipMessage(unsigned char *buffer, int buffer_size)
{	
	// initialize logging system
	char log_msg[1000];
    char *p = log_msg;
	
	// initailize structs
    struct enip_header header;
	struct enip_data_Unknown enipDataUnknown;
	struct enip_data_Unconnected enipDataUnconnected;
	struct enip_data_Connected enipDataConnected;
	struct enip_data_Connected_0x70 enipDataConnected_0x70;

    if (parseEnipHeader(buffer, buffer_size, &header, &enipDataUnknown) < 0)
	{
        return -1;
	}
	
	parseEnipUnknown(buffer, &enipDataUnknown);

	// Register a Session
    if (header.command[0] == 0x65)	
        return registerEnipSession(&header);

	if (header.command[0] == 0x70)	// Send Unit Data ---> works with Connected Type
	{
		parseEnipDataConnected_0x70(buffer, &enipDataConnected_0x70);
		uint16_t size = sendUnitData(&header, &enipDataConnected_0x70);
		return size; //sendUnitData()
	}


	//writeDataContents(&enipDataUnknown);
    
	// select Enip type-----------------------------
	// 1 = UNKNOWN
	// 2 = Unconnected
	// 3 = Connected
	// -1 = ERROR: unsupported enip type------------
	int enipType = getEnipType(&enipDataUnknown, &header);
	
	if (enipType == 2)
	{
		parseEnipUnconnected(buffer, &enipDataUnconnected);
	}
	else if (enipType == 3)
	{
		parseEnipConnected(buffer, &enipDataConnected);
	}
	else if (enipType < 0)
	{
		// log UNKNOWN Enip Type message to open plc 
		spdlog::info("ENIP: Received unsupported EtherNet/IP Type");
	}
	
	//writeDataContents(&enipDataUnknown);
	
    //if (header.command[0] == 0x65)	// Register a Session
      //  return registerEnipSession(&header);
	
    if (header.command[0] == 0x6f)	// Send RR Data
	{
		//writeDataContents(&enipDataUnknown);
		uint16_t size = sendRRData(enipType, &header, &enipDataUnknown, &enipDataUnconnected, &enipDataConnected);
		return size;
	}
	/*else if (header.command[0] == 0x70)	// Send Unit Data ---> works with Connected Type
	{
		parseEnipDataConnected_0x70(buffer, &enipDataConnected_0x70);
		uint16_t size = sendUnitData(&header, &enipDataConnected_0x70);
		return size; //sendUnitData()
	}*/
    else
    {
        p += sprintf(p, "Unknown EtherNet/IP request: ");
        for (int i = 0; i < buffer_size; i++)
        {
            p += sprintf(p, "%02x ", (unsigned char)buffer[i]);
        }
        p += sprintf(p, "\n");
        spdlog::info(log_msg);
        
        return -1;
    }
}

/** @}*/
