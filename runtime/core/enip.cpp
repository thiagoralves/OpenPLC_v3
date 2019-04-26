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
// This file has all the EtherNet/IP functions supported by the OpenPLC. If any
// other function is to be added to the project, it must be added here
// Thiago Alves, Apr 2019
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include "ladder.h"

#define ENIP_MIN_LENGTH     28

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

struct enip_data
{
    unsigned char *interface_handle;
    unsigned char *timeout;
    unsigned char *item_count;
    
    unsigned char *item1_id;
    unsigned char *item1_length;
    unsigned char *item1_data;
    
    unsigned char *item2_id;
    unsigned char *item2_length;
    unsigned char *item2_data;
};
    

thread_local unsigned char enip_session[4];

int parseEnipHeader(unsigned char *buffer, int buffer_size, struct enip_header *header)
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
    /*
    memcpy(header->command, &buffer[0], 2);
    memcpy(header->length, &buffer[2], 2);
    memcpy(header->session_handle, &buffer[4], 4);
    memcpy(header->status, &buffer[8], 4);
    memcpy(header->sender_context, &buffer[12], 8);
    memcpy(header->options, &buffer[20], 4);
    */
    header->data = &buffer[24];
    
    uint16_t enip_data_size = ((uint16_t)header->length[1] << 8) | (uint16_t)header->length[0];
    
    //verify if buffer_size matches enip_data_size
    if (buffer_size - 24 < enip_data_size)
        return -1;
    
    return enip_data_size;
}

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
// This function must parse and process the client request and write back the
// response for it. The return value is the size of the response message in
// bytes.
//-----------------------------------------------------------------------------
int processEnipMessage(unsigned char *buffer, int buffer_size)
{
    struct enip_header header;
    if (parseEnipHeader(buffer, buffer_size, &header) < 0)
        return -1;
    
    if (header.command[0] == 0x65)
        return registerEnipSession(&header);
    
    //else if (header.command[0] == 0x6f)
    //{
        
    
    else
    {
        unsigned char log_msg[1000];
        unsigned char *p = log_msg;
        p += sprintf(p, "Unknown EtherNet/IP request: ");
        for (int i = 0; i < buffer_size; i++)
        {
            p += sprintf(p, "%02x ", (unsigned char)buffer[i]);
        }
        p += sprintf(p, "\n");
        log(log_msg);
        
        return -1;
    }
}