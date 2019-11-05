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

// This is the main entry point for a single instance of DNP3 outstation.
// This supplies a single function that you can use to start the outstation
// and configuration according to a configuration file.

#ifdef OPLC_DNP3_OUTSTATION

#include <cstdio>
#include <cstdlib>
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
#include <tuple>
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
#include <openpal/logging/ILogHandler.h>
#include <openpal/util/Uncopyable.h>

#include <spdlog/spdlog.h>

#include "ladder.h"
#include "dnp3.h"
#include "dnp3_publisher.h"
#include "dnp3_receiver.h"


/** \addtogroup openplc_runtime
 *  @{
 */

#define OPLC_CYCLE              50000000

using namespace std;
using namespace opendnp3;
using namespace openpal;
using namespace asiopal;
using namespace asiodnp3;

/// Implements the ILogHandler interface from OpenDNP3 to forward log messages
/// from OpenDNP3 to spdlog. This allows those log messages to be accessed
/// via the log message API.
class Dnp3ToSpdLogger final : public openpal::ILogHandler, private openpal::Uncopyable
{
public:

    virtual void Log(const openpal::LogEntry& entry) override {
        spdlog::info("{}", entry.message);
    }

    static std::shared_ptr<openpal::ILogHandler>Create()
    {
        return std::make_shared<Dnp3ToSpdLogger>();
    };

    Dnp3ToSpdLogger() {}
};

/// @brief Trim from both ends (in place), removing only whitespace.
/// @param s The string to trim
inline void trim(string& s) {
    // Trim from the left
    s.erase(s.begin(), find_if(s.begin(), s.end(),
        not1(ptr_fun<int, int>(isspace))));

    // Trim from the right
    s.erase(find_if(s.rbegin(), s.rend(),
        not1(ptr_fun<int, int>(isspace))).base(), s.end());
}

/// @brief Read the group number from the configuration item.
/// @param value The configuration value.
/// @return the group number or less than 0 if not a valid configuration item.
int8_t get_group_number(const string& value) {
    // Find the "group" key as the sub-item
    auto group_start = value.find("group:");
    if (group_start == string::npos) {
        // This items is missing the group key.
        spdlog::error("DNP3 bind_location missing 'group:' in config item value {}", value);
        return -1;
    }

    // Now get the group number. stoi ignores spaces and trailing
    // numeric values, so we just need to give it the start of the value
    size_t num_chars;
    int8_t group_number = stoi(value.substr(group_start + 6).c_str(), &num_chars);

    // If we didn't process any characters, then the value is not valid
    return (num_chars > 0) ? group_number : -1;
}

/// @brief Read the data point index from the configuration item.
/// @param value The configuration value.
/// @return the data point index or less than 0 if not a valid configuration item.
int16_t get_data_index(const string& value) {
    // Find the "index" key as the sub-item
    auto index_start = value.find("index:");
    if (index_start == string::npos) {
        // This items is missing the index key.
        spdlog::error("DNP3 bind_location missing 'index:' in config item value {}", value);
        return -1;
    }

    // Now get the index. stoi ignores spaces and trailing
    // numeric values, so we just need to give it the start of the value
    size_t num_chars;
    int16_t index_number = stoi(value.substr(index_start + 6).c_str(), &num_chars);

    // If we didn't process any characters, then the value is not valid
    return (num_chars > 0) ? index_number : -1;
}

/// @brief Read the data point index from the configuration item.
/// @param value The configuration value.
/// @return the data point index or less than 0 if not a valid configuration item.
string get_located_name(const string& value) {
    // Find the "index" key as the sub-item
    auto name_start = value.find("name:");
    auto name_value_start = name_start + 5;
    if (name_start == string::npos) {
        // This items is missing the index key.
        spdlog::error("DNP3 bind_location missing 'name:' in config item value {}", value);
        return "";
    }

    auto name_end = value.find(',', name_value_start);
    if (name_end == string::npos) {
        // This items is missing the name end.
        spdlog::error("DNP3 bind_location missing ending ',' for name in config item value {}", value);
        return "";
    }

    return value.substr(name_value_start, name_end - name_start - 5);
}

/// @brief Handle the parsed config items to populate the command and
/// measurement mappings, using the information about the glue variables.
/// @param[in] config_items The list of all configuration items.
/// @param[in] bindings The struture for querying glue variables.
/// @param[out] binary_commands The binary command group to create.
/// @param[out] analog_command The analog command group to create.
/// @param[out] measurements The measurements list to create.
void bind_variables(const vector<pair<string, string>>& config_items,
                    const GlueVariablesBinding& binding,
                    Dnp3IndexedGroup& binary_commands,
                    Dnp3IndexedGroup& analog_commands,
                    Dnp3MappedGroup& measurements) {
    int16_t group_12_max_index(-1);
    int16_t group_41_max_index(-1);
    int16_t measurements_size(0);

    // We do this in several passes so that we can efficiently allocate memory.
    // That means more work up front, but save space over the application
    // lifetime.
    vector<tuple<string, int8_t, int16_t>> bindings;
    for (auto it = config_items.begin(); it != config_items.end(); ++it) {
        if (it->first != "bind_location") {
            // We are searching through all configuration items, so ignore any
            // items that are not associated with a bind location.
            continue;
        }

        // Find the name of the located variable
        string name = get_located_name((*it).second);
        int8_t group_number = get_group_number(it->second);
        int16_t data_index = get_data_index(it->second);

        if (name.empty() || group_number < 0 || data_index < 0) {
            // If one of the items is not valid, then don't handle furhter
            continue;
        }

        switch (group_number) {
            case GROUP_BINARY_COMMAND:
                group_12_max_index = max(group_12_max_index, data_index);
                break;
            case GROUP_ANALOG_COMMAND:
                group_41_max_index = max(group_41_max_index, data_index);
                break;
            case GROUP_BINARY_INPUT:
            case GROUP_BINARY_OUTPUT_STATUS:
            case GROUP_ANALOG_INPUT:
            case GROUP_ANALOG_OUTPUT_STATUS:
            case GROUP_COUNTER:
            case GROUP_FROZEN_COUNTER:
                measurements_size += 1;
                break;
            default:
               spdlog::error("DNP3 bind_location unknown group config item {}", (*it).second); 
        }

        bindings.push_back(make_tuple(name, group_number, data_index));
    }

    if (group_12_max_index >= 0) {
        binary_commands.size = group_12_max_index + 1;
        binary_commands.items = new ConstGlueVariable*[binary_commands.size];
        memset(binary_commands.items, 0, sizeof(ConstGlueVariable*) * binary_commands.size);
    }

    if (group_41_max_index >= 0) {
        analog_commands.size = group_41_max_index + 1;
        analog_commands.items = new ConstGlueVariable*[analog_commands.size];
        memset(analog_commands.items, 0, sizeof(ConstGlueVariable*) * analog_commands.size);
    }
    
    if (measurements_size > 0) {
        // We don't need to memset here because we will populate the entire array
        measurements.size = measurements_size;
        measurements.items = new DNP3MappedGlueVariable[measurements.size];
    }
    
    // Now bind each glue variable into the structure
    uint16_t meas_index(0);
    for (auto it = bindings.begin(); it != bindings.end(); ++it) {
        string name = std::get<0>(*it);
        int8_t group_number = std::get<1>(*it);
        int16_t data_index = std::get<2>(*it);

        const GlueVariable* var = binding.find(name);
        if (!var) {
            spdlog::error("Unable to bind location {} because it is not defined in the application", name); 
            continue;
        }

        switch (group_number) {
            case GROUP_BINARY_COMMAND:
                binary_commands.items[data_index] = var;
                break;
            case GROUP_ANALOG_COMMAND:
                analog_commands.items[data_index] = var;
                break;
            default:
                measurements.items[meas_index].group = group_number;
                measurements.items[meas_index].point_index_number = data_index;
                measurements.items[meas_index].variable = var;
                meas_index += 1;
                break;
        }
    }
}

OutstationStackConfig dnp3_create_config(istream& cfg_stream,
                                         const GlueVariablesBinding& binding,
                                         Dnp3IndexedGroup& binary_commands,
                                         Dnp3IndexedGroup& analog_commands,
                                         Dnp3MappedGroup& measurements) {
    // We need to know the size of the database (number of points) before
    // we can do anything. To avoid doing two passes of the stream, read
    // everything into a map, then get the database size, and finally
    // process the remaining items
    vector<pair<string, string>> cfg_values;
    string line;
    while (getline(cfg_stream, line)) {
        // Skip comment lines or those that are not a key-value pair
        auto pos = line.find('=');
        if (pos == string::npos || line[0] == '#') {
            continue;
        }

        string token = line.substr(0, pos);
        string value = line.substr(pos + 1);
        trim(token);
        trim(value);

        cfg_values.push_back(make_pair(token, value));
    }

    // We need to know the size of the DNP3 database (the size of each of the
    // groups) before we can create the configuration. We can figure that out
    // based on the binding of located variables, so we need to process that
    // first
    bind_variables(cfg_values, binding, binary_commands,
                   analog_commands, measurements);

    auto config = asiodnp3::OutstationStackConfig(DatabaseSizes(
        measurements.group_size(1), // Binary
        measurements.group_size(3), // Double binary
        measurements.group_size(30), // Analog
        measurements.group_size(20), // Counter
        measurements.group_size(21), // Frozen counter
        measurements.group_size(10), // Binary output status
        measurements.group_size(40), // Analog output status
        measurements.group_size(50) // Time and interval
    ));

    // Finally, handle the remaining itemss
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
    return config;
}

void dnp3StartServer(int port,
                     unique_ptr<istream, function<void(istream*)>>& cfg_stream,
                     bool* run,
                     const GlueVariablesBinding& glue_variables) {
    const uint32_t FILTERS = levels::NORMAL;

    Dnp3IndexedGroup binary_commands = {0};
    Dnp3IndexedGroup analog_commands = {0};
    Dnp3MappedGroup measurements = {0};
    auto config(dnp3_create_config(*cfg_stream, glue_variables,
                                   binary_commands, analog_commands,
                                   measurements));

    // We are done with the file, so release the unique ptr. Normally this
    // will close the reference to the file
    cfg_stream.reset(nullptr);

    // Allocate a single thread to the pool since this is a single outstation
    // Log messages to the console
    DNP3Manager manager(1, Dnp3ToSpdLogger::Create());

    // Create a listener server
    auto channel = manager.AddTCPServer("DNP3_Server", FILTERS, ChannelRetry::Default(), "0.0.0.0", port, PrintingChannelListener::Create());

    // Create a new outstation with a log level, command handler, and
    // config info this returns a thread-safe interface used for
    // updating the outstation's database.
    shared_ptr<Dnp3Receiver> receiver = make_shared<Dnp3Receiver>(binary_commands, analog_commands);
    auto outstation = channel->AddOutstation(
        "outstation",
        receiver,
        DefaultOutstationApplication::Create(),
        config);

    // Enable the outstation and start communications
    outstation->Enable();
    {
        auto publisher = make_shared<Dnp3Publisher>(outstation, measurements);

        spdlog::info("DNP3 outstation enabled on port {0:d}", port);

        // Continuously update
        struct timespec timer_start;
        clock_gettime(CLOCK_MONOTONIC, &timer_start);

        // Run this until we get a signal to stop.
        while (*run) {
            {
                // Create a scope so we release the log after the read/write
                lock_guard<mutex> guard(*glue_variables.buffer_lock);
                // Readn and write DNP3
                int num_writes = publisher->ExchangeGlue();
                receiver->ExchangeGlue();
                spdlog::trace("{} data points written to outstation", num_writes);
            }

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
void dnp3s_start_server(int port, bool* run, const GlueVariablesBinding& binding) {
    unique_ptr<istream, function<void(istream*)>> cfg_stream(new ifstream("./../webserver/dnp3.cfg"), [](istream* s)
        {
            reinterpret_cast<ifstream*>(s)->close();
            delete s;
        });
    dnp3StartServer(port, cfg_stream, run, binding);
}

#endif  // OPLC_DNP3_OUTSTATION

/** @}*/