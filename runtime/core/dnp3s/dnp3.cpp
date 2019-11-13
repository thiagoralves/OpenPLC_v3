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

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <csignal>
#include <algorithm>
#include <chrono>
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
#include <ini.h>

#include "ladder.h"
#include "dnp3.h"
#include "dnp3_publisher.h"
#include "dnp3_receiver.h"
#include "../ini_util.h"
#include "../service/service_definition.h"


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
/// @param[in] binding_defs The list of all binding items.
/// @param[in] bindings The struture for querying glue variables.
/// @param[out] binary_commands The binary command group to create.
/// @param[out] analog_command The analog command group to create.
/// @param[out] measurements The measurements list to create.
void bind_variables(const vector<string>& binding_defs,
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
    vector<tuple<string, int8_t, int16_t>> binding_infos;
    for (auto it = binding_defs.begin(); it != binding_defs.end(); ++it) {
        // Find the name of the located variable
        string name = get_located_name(*it);
        int8_t group_number = get_group_number(*it);
        int16_t data_index = get_data_index(*it);

        if (name.empty() || group_number < 0 || data_index < 0) {
            // If one of the items is not valid, then don't handle further
            continue;
        }

        const GlueVariable* var = binding.find(name);
        if (!var) {
            spdlog::error("Unable to bind DNP3 location {} because it is not defined in the application", name); 
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
               spdlog::error("DNP3 bind_location unknown group config item {}", (*it)); 
        }

        binding_infos.push_back(make_tuple(name, group_number, data_index));
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
    for (auto it = binding_infos.begin(); it != binding_infos.end(); ++it) {
        string name = std::get<0>(*it);
        int8_t group_number = std::get<1>(*it);
        int16_t data_index = std::get<2>(*it);
        const GlueVariable* var = binding.find(name);

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

/// Container for reading in configuration from the config.ini
/// This is populated with values from the config file.
struct Dnp3Config {
    Dnp3Config() :
        poll_interval(std::chrono::milliseconds(250)),
        port(20000),
        link(false, false)
    {}

    // How fast we send and receive data into the runtime.
    std::chrono::milliseconds poll_interval;

    uint16_t port;

    /// Outstation config
    opendnp3::OutstationConfig outstation;

    /// Link layer config
    opendnp3::LinkConfig link;

    /// Descriptions of the bindings we want to create.
    vector<string> bindings;
};

int dnp3s_cfg_handler(void* user_data, const char* section,
                      const char* name, const char* value) {
    if (strcmp("dnp3s", section) != 0) {
        return 0;
    }

    auto config = reinterpret_cast<Dnp3Config*>(user_data);

    // First check for a binding, because we expect to have many more of those.
    if (strcmp(name, "bind_location") == 0) {
        config->bindings.push_back(value);
    } else if (strcmp(name, "port") == 0) {
        config->port = atoi(value);
    } else if (strcmp(name, "local_address") == 0) {
        config->link.LocalAddr = atoi(value);
    } else if (strcmp(name, "remote_address") == 0) {
        config->link.RemoteAddr = atoi(value);
    } else if (strcmp(name, "keep_alive_timeout") == 0) {
        if (strcmp(value, "MAX") == 0) {
            config->link.KeepAliveTimeout = openpal::TimeDuration::Max();
        } else {
            config->link.KeepAliveTimeout = openpal::TimeDuration::Seconds(atoi(value));
        }
    } else if (strcmp(name, "enable_unsolicited") == 0) {
        config->outstation.params.allowUnsolicited = ini_atob(value);
    } else if (strcmp(name, "select_timeout") == 0) {
        config->outstation.params.selectTimeout = openpal::TimeDuration::Seconds(atoi(value));
    } else if (strcmp(name, "max_controls_per_request") == 0) {
        config->outstation.params.maxControlsPerRequest = atoi(value);
    } else if (strcmp(name, "max_rx_frag_size") == 0) {
        config->outstation.params.maxRxFragSize = atoi(value);
    } else if (strcmp(name, "max_tx_frag_size") == 0) {
        config->outstation.params.maxTxFragSize = atoi(value);
    } else if (strcmp(name, "event_buffer_size") == 0) {
        config->outstation.eventBufferConfig = EventBufferConfig::AllTypes(atoi(value));
    } else if (strcmp(name, "sol_confirm_timeout") == 0) {
        config->outstation.params.solConfirmTimeout =
            openpal::TimeDuration::Milliseconds(atoi(value));
    } else if (strcmp(name, "unsol_confirm_timeout") == 0) {
        config->outstation.params.unsolConfirmTimeout =
            openpal::TimeDuration::Milliseconds(atoi(value));
    } else if (strcmp(name, "unsol_retry_timeout") == 0) {
        config->outstation.params.unsolRetryTimeout =
            openpal::TimeDuration::Milliseconds(atoi(value));
    } else if (strcmp(name, "enabled") == 0) {
        // Nothing to do here - we already know this is enabled
    } else {
        spdlog::warn("Unknown configuration item {}", name);
        return -1;
    }

    return 0;
}

OutstationStackConfig dnp3_create_config(istream& cfg_stream,
                                         const GlueVariablesBinding& binding,
                                         Dnp3IndexedGroup& binary_commands,
                                         Dnp3IndexedGroup& analog_commands,
                                         Dnp3MappedGroup& measurements,
                                         uint16_t& port,
                                         std::chrono::milliseconds& poll_interval) {
    // We need to know the size of the database (number of points) before
    // we can do anything. To avoid doing two passes of the stream, read
    // everything into a map, then get the database size, and finally
    // process the remaining items
    Dnp3Config dnp3_config;
    ini_parse_stream(istream_fgets, &cfg_stream, dnp3s_cfg_handler, &dnp3_config);

    // We need to know the size of the DNP3 database (the size of each of the
    // groups) before we can create the configuration. We can figure that out
    // based on the binding of located variables, so we need to process that
    // first
    bind_variables(dnp3_config.bindings, binding, binary_commands,
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

    config.outstation = dnp3_config.outstation;
    config.link = dnp3_config.link;

    port = dnp3_config.port;
    poll_interval = dnp3_config.poll_interval;

    return config;
}

void dnp3s_start_server(unique_ptr<istream, function<void(istream*)>>& cfg_stream,
                        const char* cfg_overrides,
                        volatile bool& run,
                        const GlueVariablesBinding& glue_variables) {
    const uint32_t FILTERS = levels::NORMAL;

    Dnp3IndexedGroup binary_commands = {0};
    Dnp3IndexedGroup analog_commands = {0};
    Dnp3MappedGroup measurements = {0};
    uint16_t port;
    chrono::milliseconds poll_interval;
    auto config(dnp3_create_config(*cfg_stream, glue_variables,
                                   binary_commands, analog_commands,
                                   measurements, port, poll_interval));

    // If we have a config override, then check for the port number
    port = strlen(cfg_overrides) > 0 ? atoi(cfg_overrides) : port;

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

        // Run this until we get a signal to stop.
        while (run) {
            {
                // Create a scope so we release the log after the read/write
                lock_guard<mutex> guard(*glue_variables.buffer_lock);
                // Read and write DNP3
                int num_writes = publisher->ExchangeGlue();
                receiver->ExchangeGlue();
                spdlog::trace("{} data points written to outstation", num_writes);
            }

            this_thread::sleep_for(poll_interval);
        }

        outstation->Disable();
        spdlog::info("DNP3 outstation disabled on port {0:d}", port);
    }

    spdlog::info("Shutting down DNP3 server");
    channel->Shutdown();

    spdlog::info("DNP3 Server deactivated");
}

/// @brief Function to begin DNP3 server functions. This is the normal way that
/// the DNP3 server is started.
void dnp3s_service_run(const GlueVariablesBinding& binding, volatile bool& run, const char* config) {
    unique_ptr<istream, function<void(istream*)>> cfg_stream(new ifstream("../etc/config.ini"), [](istream* s)
        {
            reinterpret_cast<ifstream*>(s)->close();
            delete s;
        });
    dnp3s_start_server(cfg_stream, config, run, binding);
}

#endif  // OPLC_DNP3_OUTSTATION

/** @}*/
