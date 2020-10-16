// Copyright 2018 Thiago Alves
// Copyright 2019 Smarter Grid Solutions
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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <functional>
#include <istream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "glue.h"
#include "ini_util.h"
#include "ladder.h"
#include "logsink.h"
#include "interactive_server.h"
#include "service/service_definition.h"
#include "service/service_registry.h"

/** \addtogroup openplc_runtime
 *  @{
 */

const uint16_t BUFFER_MAX_SIZE(1024);
std::mutex command_mutex;

time_t start_time;

// Log Buffer
#define LOG_BUFFER_SIZE 1000000
unsigned char log_buffer[LOG_BUFFER_SIZE];
std::shared_ptr<buffered_sink> log_sink;

using namespace std;

/// Copy the configuration argument from the command into the buffer
/// @param source The source of the command. This should be pointing to the
/// character right after the opening "("
/// @param target The target buffer for the command.
/// @return Zero on success. Non-zero on failure. If this function
/// fails, it marks the target as an empty string so it is still safe
/// to read the string but it will be empty.
std::int8_t copy_command_config(const char *source, char target[],
                                size_t target_size) {
    // Search through source until we find the closing ")"
    size_t end_index = 0;
    while (source[end_index] != ')' && source[end_index] != '\0')
    {
        end_index++;
    }

    // If the size is such that we would not be able to assign the
    // terminating null, then return an error.
    if (end_index >= (target_size - 1))
    {
        target[0] = '\0';
        return -1;
    }

    // Now we want to copy everything from the beginning of source
    // into target, up to the length of target. This may or many not
    // fill our target, but the size ensure we don't go over the buffer
    // size.
    std::memcpy(target, source, min(end_index, target_size));

    // And set the final null terminating character in target. In general
    // this is replacing the null terminating character with the right end.
    target[end_index] = '\0';

    return 0;
}

/// @brief Get the name of the service from the command.
/// @param command The command, starting with the character after "_"
/// @return The start of the configuration information.
const char* get_service_name(const char* command, char target[], size_t target_size)
{
    size_t end_index = 0;
    while (command[end_index] != '(' && command[end_index] != '\0')
    {
        end_index++;
    }
    
    // Now we want to copy everything from the beginning of source
    // into target, up to the length of target. This may or many not
    // fill our target, but the size ensure we don't go over the buffer
    // size.
    std::memcpy(target, command, min(end_index, target_size));

    // And set the final null terminating character in target. In general
    // this is replacing the null terminating character with the right end.
    target[end_index] = '\0';

    return command + end_index + 1;
}

/// @brief Create the socket and bind it.
/// @param port
/// @return the file descriptor for the socket, or less than 0 if a socket
/// if an error occurred.
int interactive_open_socket(uint16_t port) {
    int socket_fd;
    struct sockaddr_in server_addr;

    // Create TCP Socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        spdlog::error("Interactive Server: error creating stream socket => {}",
                      strerror(errno));
        return -1;
    }

    // Set SO_REUSEADDR
    int enable = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
        &enable, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    SetSocketBlockingEnabled(socket_fd, false);

    // Initialize Server Struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(port);

    // Bind socket
    if (bind(socket_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        spdlog::error("Interactive Server: error binding socket => {}",
                      strerror(errno));
        return -2;
    }

    // We accept max 5 pending connections
    listen(socket_fd, 5);
    spdlog::info("Interactive Server: Listening on port {}", port);

    return socket_fd;
}

/// Poll for new clients that want to connect to the runtime.
///
/// This returns a client file desriptor for each client that wants to
/// attach.
/// @param run A flag that is set to false when we should stop polling.
/// @param socket_fd The socket file descriptor we are listening on.
/// @return the client file descriptor.
int interactive_wait_new_client(volatile bool& run, int socket_fd) {
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len;

    spdlog::trace("Interactive Server: waiting for new client...");

    client_len = sizeof(client_addr);
    while (run)
    {
        // Non-blocking call
        client_fd = accept(socket_fd, (struct sockaddr *)&client_addr,
                           &client_len);

        if (client_fd > 0)
        {
            SetSocketBlockingEnabled(client_fd, true);
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    return client_fd;
}

/// Write to the file descriptor and check the result, logging
/// if the result indicates anything other than success.
void write_checked(int fd, const void *buf, size_t count)
{
    auto result = write(fd, buf, count);
    if (result < 0) {
        spdlog::error("Error writing to file descriptor {}", fd);
    }
}

/// Handle a single command from the client.
/// @param command The command as a text string.
/// @param client_fd The file descriptor to write a response to.
void interactive_client_command(const char* command, int client_fd)
{
    // A buffer for the command configuration information.
    char command_buffer[BUFFER_MAX_SIZE];

    unique_lock<mutex> lock(command_mutex, defer_lock);
    if (!lock.try_lock())
    {
        spdlog::trace("Command skipped - already processing {}", command);
        int count_char = sprintf(command_buffer, "Another command in progress...\n");
        write_checked(client_fd, command_buffer, count_char);
        return;
    }

    spdlog::trace("Command received {}", command);

    if (strncmp(command, "quit()", 6) == 0)
    {
        spdlog::info("Issued quit() command");
        run_openplc = 0;
    }
    else if (strncmp(command, "start_", 6) == 0)
    {
        char service_name[MAX_SERVICE_NAME_SIZE];
        auto config_start = get_service_name(command + 6, service_name, MAX_SERVICE_NAME_SIZE);

        ServiceDefinition* def = services_find(service_name);
        if (!def)
        {
            spdlog::error("Unrecognized service name {}", service_name);
            int count_char = sprintf(command_buffer, "Error: unrecognized service name '%s'\n", service_name);
            write_checked(client_fd, command_buffer, count_char);
            return;
        }

        if (copy_command_config(config_start, command_buffer, BUFFER_MAX_SIZE) == 0)
        {
            def->start(command_buffer);
        }
        else
        {
           spdlog::error("Unable to start service due {} to missing config", service_name);
        }

    }
    else if (strncmp(command, "stop_", 5) == 0)
    {
        char service_name[MAX_SERVICE_NAME_SIZE];
        auto config_start = get_service_name(command + 5, service_name, MAX_SERVICE_NAME_SIZE);

        ServiceDefinition* def = services_find(service_name);
        if (!def)
        {
            int count_char = sprintf(command_buffer, "Error: unrecognized service name '%s'\n", service_name);
            write_checked(client_fd, command_buffer, count_char);
            return;
        }

        def->stop();
    }
    else if (strncmp(command, "runtime_logs()", 14) == 0)
    {
        spdlog::trace("Issued runtime_logs() command");
        std::string data = log_sink->data();
        write_checked(client_fd, data.c_str(), data.size());
        return;
    }
    else if (strncmp(command, "exec_time()", 11) == 0)
    {
        time_t end_time;
        time(&end_time);
        int count_char = sprintf(command_buffer, "%llu\n", (unsigned long long)difftime(end_time, start_time));
        write_checked(client_fd, command_buffer, count_char);
        return;
    }
    else
    {
        spdlog::error("Unrecognized command {}", command_buffer);
        int count_char = sprintf(command_buffer, "Error: unrecognized command\n");
        write_checked(client_fd, command_buffer, count_char);
        return;
    }

    int count_char = sprintf(command_buffer, "OK\n");
    write_checked(client_fd, command_buffer, count_char);
}

/// Defines the arguments that client threads receive.
struct ClientArgs
{
    /// The client file descriptor for reading and writing.
    int client_fd;
    /// A flag that can indicate termination of the thread.
    volatile bool* run;
};

/// Response to a client connection in a unique thread.
/// @param arguments The arguments to the thread - must be the client arguments
/// struture. This thread will be responsible for freeing the arguments.
/// @return always nullptr.
void* interactive_client_run(void* arguments)
{
    auto client_args = reinterpret_cast<ClientArgs*>(arguments);

    char buffer[BUFFER_MAX_SIZE];
    int message_size;

    while (*client_args->run)
    {
        memset(buffer, 0, BUFFER_MAX_SIZE);
        message_size = read(client_args->client_fd, buffer, BUFFER_MAX_SIZE);

        if (message_size <= 0)
        {
            spdlog::trace("Interactive Server: client ID: {} has closed the connection", client_args->client_fd);
            break;
        }

        if (message_size > BUFFER_MAX_SIZE)
        {
            spdlog::error("Interactive Server: Message size is too large for client {}", client_args->client_fd);
            break;
        }

        // Process the received buffer, splitting into commands
        char* start = buffer;
        uint16_t cur_pos = 0;

        while (cur_pos < BUFFER_MAX_SIZE && buffer[cur_pos] != '\0')
        {
            if (buffer[cur_pos] == '\n' || buffer[cur_pos] == '\r')
            {
                // Terminate the command
                buffer[cur_pos] = '\0';
                interactive_client_command(start, client_args->client_fd);
            }
            cur_pos += 1;
        }
    }

    close_socket(client_args->client_fd);
    spdlog::trace("Interactive server connection completed");
    delete client_args;

    return nullptr;
}

/// Run the interactive server for responding to control and measurement
/// requests through the socket API. This is the primary means for
/// controlling the runtime.
void interactive_run(oplc::config_stream& cfg_stream,
                     const char* cfg_overrides,
                     const GlueVariablesBinding& bindings,
                     volatile bool& run)
{
    const uint16_t port = 43628;
    int socket_fd = interactive_open_socket(port);

    // Listen for new connections to our socket. When we have a new
    // connection, we spawn a new thread to handle that connection.
    while (run)
    {
        int client_fd = interactive_wait_new_client(run, socket_fd);

        if (client_fd < 0)
        {
            spdlog::error("Interactive Server: Error accepting client!");
            continue;
        }

        auto args = new ClientArgs { .client_fd = client_fd, .run = &run };

        spdlog::trace("Interactive Server: Client accepted! Creating thread for the new client ID: {}", client_fd);
        try {
            std::thread thread(interactive_client_run, args);
            thread.detach();
        } 
        catch (const std::system_error& e) 
        {
            spdlog::error("Interactive Server: Thread creation exception {}! Error message: {}...", e.code().value(), e.what());
            delete args;
            close_socket(client_fd);
        }
    }

    close_socket(socket_fd);
}

void interactive_service_run(const GlueVariablesBinding& binding,
                             volatile bool& run, const char* config)
{
    auto cfg_stream = oplc::open_config();
    interactive_run(cfg_stream, config, binding, run);
}

void initialize_logging(int argc, char **argv)
{
    log_sink.reset(new buffered_sink(log_buffer, LOG_BUFFER_SIZE));
    spdlog::default_logger()->sinks().push_back(log_sink);
}

/** @}*/
