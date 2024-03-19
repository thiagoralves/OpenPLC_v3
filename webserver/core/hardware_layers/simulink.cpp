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
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#include "ladder.h"
#include "custom_layer.h"

#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#define PORT        6668

#define ANALOG_BUF_SIZE		8
#define DIGITAL_BUF_SIZE	16

struct plcData
{
	uint16_t analogIn[ANALOG_BUF_SIZE];
	uint16_t analogOut[ANALOG_BUF_SIZE];
	bool digitalIn[DIGITAL_BUF_SIZE];
	bool digitalOut[DIGITAL_BUF_SIZE];
};

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

//-----------------------------------------------------------------------------
// Create the socket and bind it. Returns the file descriptor for the socket
// created.
//-----------------------------------------------------------------------------
int createUDPSocket(int port)
{
	int socket_fd;
	struct sockaddr_in server_addr;
	struct hostent *server;

	//Create TCP Socket
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd<0)
	{
		perror("Server: error creating stream socket");
		exit(1);
	}

	//Initialize Server Struct
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	//Bind socket
	if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("Server: error binding socket");
		exit(1);
	}

	printf("Socket %d binded successfully on port %d!\n", socket_fd, port);

	return socket_fd;
}


//-----------------------------------------------------------------------------
// Thread to send and receive data using UDP
//-----------------------------------------------------------------------------
void *exchangeData(void *arg)
{
	int socket_fd = createUDPSocket(PORT);
	int net_len;
	socklen_t cli_len;
	struct sockaddr_in client;
	struct plcData *plc_data = (struct plcData *)malloc(sizeof(struct plcData));

	cli_len = sizeof(client);

	while(1)
	{
		//printf("waiting for data...\n");
		net_len = recvfrom(socket_fd, plc_data, sizeof(*plc_data), 0, (struct sockaddr *) &client, &cli_len);
		//printf("data received!\n");
		if (net_len < 0)
		{
			printf("Error receiving data on socket %d\n", socket_fd);
		}
		else
		{
			pthread_mutex_lock(&bufferLock); //lock mutex
			for (int i = 0; i < ANALOG_BUF_SIZE; i++)
			{
                if (pinNotPresent(ignored_int_inputs, ARRAY_SIZE(ignored_int_inputs), i))
				    if (int_input[i] != NULL) *int_input[i] = plc_data->analogIn[i];

                if (pinNotPresent(ignored_int_outputs, ARRAY_SIZE(ignored_int_outputs), i))
    				if (int_output[i] != NULL) plc_data->analogOut[i] = *int_output[i];
			}
			for (int i = 0; i < DIGITAL_BUF_SIZE; i++)
			{
			    if (pinNotPresent(ignored_bool_inputs, ARRAY_SIZE(ignored_bool_inputs), i))
    				if (bool_input[i/8][i%8] != NULL) *bool_input[i/8][i%8] = plc_data->digitalIn[i];
				
				if (pinNotPresent(ignored_bool_outputs, ARRAY_SIZE(ignored_bool_outputs), i))
    				if (bool_output[i/8][i%8] != NULL) plc_data->digitalOut[i] = *bool_output[i/8][i%8];
			}
			pthread_mutex_unlock(&bufferLock); //unlock mutex

			//printf("sending data...\n");
			net_len = sendto(socket_fd, plc_data, sizeof(*plc_data), 0, (struct sockaddr *) &client, cli_len);
			if (net_len < 0)
			{
				printf("Error sending data on socket %d\n", socket_fd);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
	pthread_t thread;
	pthread_create(&thread, NULL, exchangeData, NULL);
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
	// This function here is blank because the thread that connects to the
	// Interface program is already filling the OpenPLC buffers with the
	// data that is being received.
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Output state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
	// This function here is blank because the thread that connects to the
	// Interface program is already filling the OpenPLC buffers with the
	// data that is being received.
}
