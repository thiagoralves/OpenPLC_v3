//-----------------------------------------------------------------------------
// Copyright 2022 Thiago Alves
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
// This is the file for the network routines of the OpenPLC. It has procedures
// to create a socket and connect to a server.
// Thiago Alves, Nov 2022
//-----------------------------------------------------------------------------


#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "ladder.h"

#define METHOD_UDP 1
#define METHOD_TCP 0

int connect_to_tcp_server(uint8_t *ip_address, uint16_t port, int method)
{
    int sockfd, connfd;
    char log_msg[1000];
    struct sockaddr_in servaddr, cli;
    
    if (method == METHOD_TCP)
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
    else if (method == METHOD_UDP)
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sockfd == -1)
    {
        sprintf(log_msg, "TCP Client: error creating TCP socket => %s\n", strerror(errno));
        log(log_msg);
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));
    
    //Configure socket
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip_address);
    servaddr.sin_port = htons(port);
    
    //Connect to server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        sprintf(log_msg, "TCP Client: error connecting to server => %s\n", strerror(errno));
        log(log_msg);
        close(sockfd);
        return -1;
    }
    
    //Set non-blocking socket
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        sprintf(log_msg, "TCP Client: error reading flags from TCP socket => %s\n", strerror(errno));
        log(log_msg);
        return -1;
    }
    flags = (flags | O_NONBLOCK);
    if (fcntl(sockfd, F_SETFL, flags) != 0)
    {
        sprintf(log_msg, "TCP Client: error setting flags for TCP socket => %s\n", strerror(errno));
        log(log_msg);
        return -1;
    }
    
    return sockfd;
}

int send_tcp_message(uint8_t *msg, size_t msg_size, int socket_id)
{
    int bytes_sent = 0;
    char log_msg[1000];
    bytes_sent = write(socket_id, msg, msg_size);
    if (bytes_sent < 0)
    {
        sprintf(log_msg, "TCP Client: error sending msg to server => %s\n", strerror(errno));
        log(log_msg);
        return -1;
    }
    
    return bytes_sent;
}

int receive_tcp_message(uint8_t *msg_buffer, size_t buffer_size, int socket_id)
{
    int bytes_received = 0;
    char log_msg[1000];
    bytes_received = read(socket_id, msg_buffer, buffer_size);
    
    if (bytes_received < 0 && bytes_received != EAGAIN && bytes_received != EWOULDBLOCK)
    {
        //sprintf(log_msg, "TCP Client: error receiving msg from server => %s\n", strerror(errno));
        //log(log_msg);
        return -1;
    }
    else
    {
        msg_buffer[bytes_received] = 0;
        sprintf(log_msg, "TCP Client: msg from server => %s\n", msg_buffer);
        log(log_msg);
    }
    
    return bytes_received;
}

int close_tcp_connection(int socket_id)
{
    return close(socket_id);
}






















