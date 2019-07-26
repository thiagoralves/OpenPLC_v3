// Copyright 2018 Thiago Alves
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


// This is the file for the interactive server. It has procedures to create
// a socket, bind it, start network communication, and process commands. The 
// interactive server only responds to localhost and it is used to communicate
// with the Python webserver GUI only.
//
// Thiago Alves, Jun 2018
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <memory>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>

#include <spdlog/spdlog.h>

#include "ladder.h"
#include "logsink.h"

//Global Variables
bool run_modbus = 0;
int modbus_port = 502;
bool run_dnp3 = 0;
int dnp3_port = 20000;
bool run_enip = 0;
int enip_port = 44818;
unsigned char server_command[1024];
int command_index = 0;
bool processing_command = 0;
time_t start_time;
time_t end_time;

//Global Threads
pthread_t modbus_thread;
pthread_t dnp3_thread;
pthread_t enip_thread;

//Log Buffer
#define LOG_BUFFER_SIZE 1000000
unsigned char log_buffer[LOG_BUFFER_SIZE];
std::shared_ptr<buffered_sink> log_sink;


//-----------------------------------------------------------------------------
// Start the Modbus Thread
//-----------------------------------------------------------------------------
void *modbusThread(void *arg)
{
    startServer(modbus_port, MODBUS_PROTOCOL);
}


#ifdef OPLC_DNP3
//-----------------------------------------------------------------------------
// Start the DNP3 Thread
//-----------------------------------------------------------------------------
void *dnp3Thread(void *arg)
{
    dnp3StartServer(dnp3_port);
}
#endif
//-----------------------------------------------------------------------------
// Start the Enip Thread
//-----------------------------------------------------------------------------
void *enipThread(void *arg)
{
    startServer(enip_port, ENIP_PROTOCOL);
}

//-----------------------------------------------------------------------------
// Read the argument from a command function
//-----------------------------------------------------------------------------
int readCommandArgument(unsigned char *command)
{
    int i = 0;
    int j = 0;
    unsigned char argument[1024];
    
    while (command[i] != '(' && command[i] != '\0') i++;
    if (command[i] == '(') i++;
    while (command[i] != ')' && command[i] != '\0')
    {
        argument[j] = command[i];
        i++;
        j++;
        argument[j] = '\0';
    }
    
    return atoi(argument);
}

//-----------------------------------------------------------------------------
// Create the socket and bind it. Returns the file descriptor for the socket
// created.
//-----------------------------------------------------------------------------
int createSocket_interactive(int port)
{
    int socket_fd;
    struct sockaddr_in server_addr;

    //Create TCP Socket
    socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if (socket_fd < 0)
    {
		spdlog::error("Interactive Server: error creating stream socket => {}", strerror(errno));
        exit(1);
    }
    
    //Set SO_REUSEADDR
    int enable = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
	}
        
    SetSocketBlockingEnabled(socket_fd, false);
    
    //Initialize Server Struct
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);

    //Bind socket
    if (bind(socket_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
    {
		spdlog::error("Interactive Server: error binding socket => {}", strerror(errno));
        exit(1);
    }
    // we accept max 5 pending connections
    listen(socket_fd,5);
	spdlog::info("Interactive Server: Listening on port {}", port);

    return socket_fd;
}

//-----------------------------------------------------------------------------
// Blocking call. Wait here for the client to connect. Returns the file
// descriptor to communicate with the client.
//-----------------------------------------------------------------------------
int waitForClient_interactive(int socket_fd)
{
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len;

	spdlog::info("Interactive Server: waiting for new client...");

    client_len = sizeof(client_addr);
    while (run_openplc)
    {
        client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len); //non-blocking call
        if (client_fd > 0)
        {
            SetSocketBlockingEnabled(client_fd, true);
            break;
        }
        sleepms(100);
    }

    return client_fd;
}

//-----------------------------------------------------------------------------
// Blocking call. Holds here until something is received from the client.
// Once the message is received, it is stored on the buffer and the function
// returns the number of bytes received.
//-----------------------------------------------------------------------------
int listenToClient_interactive(int client_fd, unsigned char *buffer)
{
    bzero(buffer, 1024);
    int n = read(client_fd, buffer, 1024);
    return n;
}

//-----------------------------------------------------------------------------
// Process client's commands for the interactive server
//-----------------------------------------------------------------------------
void processCommand(unsigned char *buffer, int client_fd)
{
    spdlog::info("Process command received {}", buffer);

    int count_char = 0;
    
    if (processing_command)
    {
        count_char = sprintf(buffer, "Processing command...\n");
        write(client_fd, buffer, count_char);
        return;
    }
    
    if (strncmp(buffer, "quit()", 6) == 0)
    {
        processing_command = true;
		spdlog::info("Issued quit() command");
        if (run_modbus)
        {
            run_modbus = 0;
            pthread_join(modbus_thread, NULL);
			spdlog::info("Modbus server was stopped");
        }
        if (run_dnp3)
        {
            run_dnp3 = 0;
            pthread_join(dnp3_thread, NULL);
			spdlog::info("DNP3 server was stopped");
        }
        run_openplc = 0;
        processing_command = false;
    }
    else if (strncmp(buffer, "start_modbus(", 13) == 0)
    {
        processing_command = true;
		modbus_port = readCommandArgument(buffer);
		spdlog::info("Issued start_modbus() command to start on port: {}", modbus_port);
        
        if (run_modbus)
        {
			spdlog::info("Modbus server already active. Restarting on port: {}", modbus_port);
            //Stop Modbus server
            run_modbus = 0;
            pthread_join(modbus_thread, NULL);
			spdlog::info("Modbus server was stopped");
        }
        //Start Modbus server
        run_modbus = 1;
        pthread_create(&modbus_thread, NULL, modbusThread, NULL);
        processing_command = false;
    }
    else if (strncmp(buffer, "stop_modbus()", 13) == 0)
    {
        processing_command = true;
		spdlog::info("Issued stop_modbus() command");
        if (run_modbus)
        {
            run_modbus = 0;
            pthread_join(modbus_thread, NULL);
			spdlog::info("Modbus server was stopped");
        }
        processing_command = false;
    }

#ifdef OPLC_DNP3
    else if (strncmp(buffer, "start_dnp3(", 11) == 0)
    {
        processing_command = true;
		dnp3_port = readCommandArgument(buffer);
		spdlog::info("Issued start_dnp3() command to start on port: {}", dnp3_port);
        if (run_dnp3)
        {
			spdlog::info("DNP3 server already active. Restarting on port: {}", dnp3_port);
            //Stop DNP3 server
            run_dnp3 = 0;
            pthread_join(dnp3_thread, NULL);
			spdlog::info("DNP3 server was stopped");
        }
        //Start DNP3 server
        run_dnp3 = 1;
        pthread_create(&dnp3_thread, NULL, dnp3Thread, NULL);
        processing_command = false;
    }
    else if (strncmp(buffer, "stop_dnp3()", 11) == 0)
    {
        processing_command = true;
		spdlog::info("Issued stop_dnp3() command");
        if (run_dnp3)
        {
            run_dnp3 = 0;
            pthread_join(dnp3_thread, NULL);
			spdlog::info("DNP3 server was stopped");
        }
        processing_command = false;
    }
#endif

    else if (strncmp(buffer, "start_enip(", 11) == 0)
    {
        processing_command = true;
        spdlog::info("Issued start_enip() command to start on port: {}", readCommandArgument(buffer));
        enip_port = readCommandArgument(buffer);
        if (run_enip)
        {
	        spdlog::info("EtherNet/IP server already active. Restarting on port: {}", enip_port);
            //Stop Enip server
            run_enip = 0;
            pthread_join(enip_thread, NULL);
	        spdlog::info("EtherNet/IP server was stopped");
        }
        //Start Enip server
        run_enip = 1;
        pthread_create(&enip_thread, NULL, enipThread, NULL);
        processing_command = false;
    }
    else if (strncmp(buffer, "stop_enip()", 11) == 0)
    {
        processing_command = true;
	    spdlog::info("Issued stop_enip() command");
        if (run_enip)
        {
            run_enip = 0;
            pthread_join(enip_thread, NULL);
	        spdlog::info("EtherNet/IP server was stopped");
        }
        processing_command = false;
    }
    else if (strncmp(buffer, "runtime_logs()", 14) == 0)
    {
        processing_command = true;
        spdlog::debug("Issued runtime_logs() command");
        std::string data = log_sink->data();
        write(client_fd, data.c_str(), data.size());
        processing_command = false;
        return;
    }
    else if (strncmp(buffer, "exec_time()", 11) == 0)
    {
        processing_command = true;
        time(&end_time);
        count_char = sprintf(buffer, "%llu\n", (unsigned long long)difftime(end_time, start_time));
        write(client_fd, buffer, count_char);
        processing_command = false;
        return;
    }
    else
    {
        processing_command = true;
        count_char = sprintf(buffer, "Error: unrecognized command\n");
        write(client_fd, buffer, count_char);
        processing_command = false;
        return;
    }
    
    count_char = sprintf(buffer, "OK\n");
    write(client_fd, buffer, count_char);
}

//-----------------------------------------------------------------------------
// Process client's request
//-----------------------------------------------------------------------------
void processMessage_interactive(unsigned char *buffer, int bufferSize, int client_fd)
{
    for (int i = 0; i < bufferSize; i++)
    {
        if (buffer[i] == '\r' || buffer[i] == '\n' || command_index >= 1024)
        {
            processCommand(server_command, client_fd);
            command_index = 0;
            break;
        }
        server_command[command_index] = buffer[i];
        command_index++;
        server_command[command_index] = '\0';
    }
}

//-----------------------------------------------------------------------------
// Thread to handle requests for each connected client
//-----------------------------------------------------------------------------
void *handleConnections_interactive(void *arguments)
{
    int client_fd = *(int *)arguments;
    unsigned char buffer[1024];
    int messageSize;

	spdlog::info("Interactive Server: Thread created for client ID: {}", client_fd);

    while(run_openplc)
    {
        //unsigned char buffer[1024];
        //int messageSize;

        messageSize = listenToClient_interactive(client_fd, buffer);
        if (messageSize <= 0 || messageSize > 1024)
        {
            // something has  gone wrong or the client has closed connection
            if (messageSize == 0)
            {
				spdlog::info("Interactive Server: client ID: {} has closed the connection", client_fd);
            }
            else
            {
				spdlog::error("Interactive Server: Something is wrong with the client ID: {} message Size : {}", client_fd, messageSize);
            }
            break;
        }

        processMessage_interactive(buffer, messageSize, client_fd);
    }

    spdlog::debug("Closing client socket and calling pthread_exit in interactive_server.cpp");
    closeSocket(client_fd);
	spdlog::info("Terminating interactive server connections");
    pthread_exit(NULL);
}

//-----------------------------------------------------------------------------
// Function to start the server. It receives the port number as argument and
// creates an infinite loop to listen and parse the messages sent by the
// clients
//-----------------------------------------------------------------------------
void startInteractiveServer(int port)
{
    int socket_fd, client_fd;
    socket_fd = createSocket_interactive(port);

    while(run_openplc)
    {
        client_fd = waitForClient_interactive(socket_fd); //block until a client connects
        if (client_fd < 0)
        {
			spdlog::error("Interactive Server: Error accepting client!");
        }
        else
        {
            int arguments[1];
            pthread_t thread;
            int ret = -1;

			spdlog::info("Interactive Server: Client accepted! Creating thread for the new client ID: {}", client_fd);
            arguments[0] = client_fd;
            ret = pthread_create(&thread, NULL, handleConnections_interactive, arguments);
            if (ret==0) 
            {
                pthread_detach(thread);
            }
        }
    }

	spdlog::info("Closing socket...");
    closeSocket(socket_fd);
    closeSocket(client_fd);

	spdlog::info("Terminating interactive server thread");
}

void initializeLogging(int argc,char **argv)
{
    log_sink.reset(new buffered_sink(log_buffer, LOG_BUFFER_SIZE));
    spdlog::default_logger()->sinks().push_back(log_sink);
}
