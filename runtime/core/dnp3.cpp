// Copyright 2017 Trevor Aron, 2019 Smarter Grid Solutions
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

// This is the main entry point for a single instance of DNP3 outstation.
// This supplies a single function that you can use to start the outstation
// and configuration according to a configuration file.

#ifdef OPLC_DNP3_OUTSTATION

#include <cstdio>
#include <ctime>
#include <csignal>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <utility>

#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/PrintingSOEHandler.h>
#include <asiodnp3/PrintingChannelListener.h>
#include <asiodnp3/ConsoleLogger.h>
#include <asiodnp3/UpdateBuilder.h>
#include <asiopal/UTCTimeSource.h>
#include <opendnp3/outstation/SimpleCommandHandler.h>
#include <opendnp3/LogLevels.h>

#include <spdlog/spdlog.h>

#include "ladder.h"
#include "dnp3.h"
#include "dnp3_publisher.h"
#include "dnp3_receiver.h"


/** \addtogroup openplc_runtime
 *  @{
 */

#define OPLC_CYCLE              50000000

// Initial offset parameters (yurgen1975)
int offset_di = 0;
int offset_do = 0;
int offset_ai = 0;
int offset_ao = 0;

using namespace opendnp3;
using namespace openpal;
using namespace asiopal;
using namespace asiodnp3;

/// @brief Trim from both ends (in place), removing only whitespace.
/// @param s The string to trim
static inline void trim(std::string& s) {
    // Trim from the left
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));

    // Trim from the right
    s.erase(std::find_if(s.rbegin(), s.rend(),
        std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

/// @brief Create the outstation stack configuration using the configration settings
/// as specified in the stream.
/// @param cfg_stream The stream to read for configuration settings
/// @return The configuration represented by the stream and any defaults and
///         the range mapping.
std::pair<asiodnp3::OutstationStackConfig, Dnp3Range> create_config(std::istream& cfg_stream) {
    // We need to know the size of the database (number of points) before
    // we can do anything. To avoid doing two passes of the stream, read
    // everything into a map, then get the database size, and finally
    // process the remaining items
    std::map<std::string, std::string> cfg_values;
    std::string line;
    while (getline(cfg_stream, line)) {
        // Skip comment lines or those that are not a key-value pair
        auto pos = line.find('=');
        if (pos == std::string::npos || line[0] == '#') {
            continue;
        }

        std::string token = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        trim(token);
        trim(value);

        cfg_values[token] = value;
    }

    // Now that we know if we have the database size, we can ceate the stack config
    auto default_size = 10;
    auto db_size = cfg_values.find("database_size");
    if (db_size != cfg_values.end()) {
        default_size = atoi(db_size->second.c_str());
        cfg_values.erase(db_size);
    }

    auto config = asiodnp3::OutstationStackConfig(DatabaseSizes::AllTypes(default_size));
    Dnp3Range range = { 0, OPLCGLUE_INPUT_SIZE, 0, 0, OPLCGLUE_OUTPUT_SIZE, 0, 0, BUFFER_SIZE, 0, 0, BUFFER_SIZE, 0};

    // Finally, handle the remaining items
    for (auto it = cfg_values.begin(); it != cfg_values.end(); ++it) {
        auto token = it->first;
        auto value = it->second;
        try {
            if (token == "local_address") {
                config.link.LocalAddr = atoi(value.c_str());
            } else if (token == "remote_address") {
                config.link.RemoteAddr = atoi(value.c_str());
            } else if (token == "keep_alive_timeout") {
                if (value == "MAX") {
                    config.link.KeepAliveTimeout = openpal::TimeDuration::Max();
                } else {
                    config.link.KeepAliveTimeout = openpal::TimeDuration::Seconds(atoi(value.c_str()));
                }
            } else if (token == "enable_unsolicited") {
                if (token == "True") {
                    config.outstation.params.allowUnsolicited = true;
                } else {
                    config.outstation.params.allowUnsolicited = false;
                }
            } else if (token == "select_timeout") {
                config.outstation.params.selectTimeout = openpal::TimeDuration::Seconds(atoi(value.c_str()));
            } else if (token == "max_controls_per_request") {
                config.outstation.params.maxControlsPerRequest = atoi(value.c_str());
            } else if (token == "max_rx_frag_size") {
                config.outstation.params.maxRxFragSize = atoi(value.c_str());
            } else if (token == "max_tx_frag_size") {
                config.outstation.params.maxTxFragSize = atoi(value.c_str());
            } else if (token == "event_buffer_size") {
                config.outstation.eventBufferConfig = EventBufferConfig::AllTypes(atoi(value.c_str()));
            } else if (token == "offset_di") {
                range.bool_inputs_offset = atoi(value.c_str());
            } else if (token == "offset_do") {
                range.bool_outputs_offset = atoi(value.c_str());
            } else if (token == "offset_ai") {
                range.inputs_offset = atoi(value.c_str());
            } else if (token == "offset_ao") {
                range.outputs_offset = atoi(value.c_str());
            } else if (token == "sol_confirm_timeout") {
                config.outstation.params.solConfirmTimeout =
                    openpal::TimeDuration::Milliseconds(atoi(value.c_str()));
            } else if (token == "unsol_confirm_timeout") {
                config.outstation.params.unsolConfirmTimeout =
                    openpal::TimeDuration::Milliseconds(atoi(value.c_str()));
            } else if (token == "unsol_retry_timeout") {
                config.outstation.params.unsolRetryTimeout =
                    openpal::TimeDuration::Milliseconds(atoi(value.c_str()));
            }
        }
        catch (...) {
            spdlog::error("Malformed line {} = {}", token, value);
            exit(1);
        }
    }
    return std::make_pair(config, range);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Start the DNP3 server running on the specified port and configured
/// using the specified stream.
///
/// The stream is specified as a function so that this function will close the
/// stream as soon as it is done with the stream.
/// @param port The port to listen on
/// @param cfg_stream_fn An input stream to read configuration information from. This will be reset
///                      once use of the stream has been completed.
/// @param run A signal for running this server. This server terminates when this signal is false.
void dnp3StartServer(int port, std::unique_ptr<std::istream, std::function<void(std::istream*)>>& cfg_stream, const bool* run) {
    const uint32_t FILTERS = levels::NORMAL;

    std::pair<asiodnp3::OutstationStackConfig, Dnp3Range> config_range(create_config(*cfg_stream));

    // We are done with the file, so release the unique ptr. Normally this
    // will close the reference to the file
    cfg_stream.reset(nullptr);

    // Allocate a single thread to the pool since this is a single outstation
    // Log messages to the console
    DNP3Manager manager(1, ConsoleLogger::Create());

    // Create a listener server
    auto channel = manager.AddTCPServer("DNP3_Server", FILTERS, ChannelRetry::Default(), "0.0.0.0", port, PrintingChannelListener::Create());

    // We provide variable bindings into DNP3 so that it can support multiple outstations.
    // There are two pieces to this - the glue and the defined range for this instance.
    std::shared_ptr<GlueVariables> glue_variables = std::make_shared<GlueVariables>(
        &bufferLock,
        &(bool_input[0][0]),
        &(bool_output[0][0]),
        OPLCGLUE_INPUT_SIZE,
        oplc_input_vars,
        OPLCGLUE_OUTPUT_SIZE,
        oplc_output_vars);

    // Create a new outstation with a log level, command handler, and
    // config info this returns a thread-safe interface used for
    // updating the outstation's database.
    std::shared_ptr<ICommandHandler> cc = std::make_shared<Dnp3Receiver>(glue_variables, config_range.second);
    auto outstation = channel->AddOutstation(
        "outstation",
        cc,
        DefaultOutstationApplication::Create(),
        config_range.first);

    // Enable the outstation and start communications
    outstation->Enable();
    {
        auto publisher = std::make_shared<Dnp3Publisher>(outstation, glue_variables, config_range.second);

        spdlog::info("DNP3 outstation enabled on port {0:d} with range {1}", port, config_range.second.ToString().c_str());

        // Continuously update
        struct timespec timer_start;
        clock_gettime(CLOCK_MONOTONIC, &timer_start);

        // Run this until we get a signal to stop.
        while (*run) {
            int num_writes = publisher->WriteToPoints();
            spdlog::trace("{} data points written to outstation", num_writes);
            sleep_until(&timer_start, OPLC_CYCLE);
        }

        outstation->Disable();
        spdlog::info("DNP3 outstation disabled on port {0:d}", port);
    }

    spdlog::info("Shutting down DNP3 server");
    channel->Shutdown();

    spdlog::info("DNP3 Server deactivated");
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Function to begin DNP3 server functions. This is the normal way that
/// the DNP3 server is started.
/// @param port The port to run against.
////////////////////////////////////////////////////////////////////////////////
void dnp3StartServer(int port) {
    std::unique_ptr<std::istream, std::function<void(std::istream*)>> cfg_stream(new std::ifstream("./../webserver/dnp3.cfg"), [](std::istream* s)
        {
            reinterpret_cast<std::ifstream*>(s)->close();
            delete s;
        });
    dnp3StartServer(port, cfg_stream, &run_dnp3);
}

#endif  // OPLC_DNP3_OUTSTATION

/** @}*/