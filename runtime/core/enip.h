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

// This file contains the structured used by enip.cpp to process
// EtherNet/IP requests.
// UAH, Sep 2019
//-----------------------------------------------------------------------------

/** \addtogroup openplc_runtime
 *  @{
 */

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

/** @} */
