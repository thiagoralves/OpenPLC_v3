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

#ifndef CORE_DNP3S_DNP3_H_
#define CORE_DNP3S_DNP3_H_

#include <cstdint>
#include <istream>
#include <memory>
#include <utility>
#include <sstream>
#include <string>

namespace asiodnp3 {
class OutstationStackConfig;
}
namespace std {
class mutex;
}

struct GlueVariable;
class GlueVariablesBinding;

/** \addtogroup openplc_runtime
 *  @{
 */

typedef const GlueVariable ConstGlueVariable;

const std::uint8_t GROUP_BINARY_COMMAND(12);
const std::uint8_t GROUP_ANALOG_COMMAND(41);
const std::uint8_t GROUP_BINARY_INPUT(1);
const std::uint8_t GROUP_BINARY_OUTPUT_STATUS(10);
const std::uint8_t GROUP_ANALOG_INPUT(30);
const std::uint8_t GROUP_ANALOG_OUTPUT_STATUS(40);
const std::uint8_t GROUP_COUNTER(20);
const std::uint8_t GROUP_FROZEN_COUNTER(21);

////////////////////////////////////////////////////////////////////////////////
/// @brief Defines a list of mapped variables for a particular group.
/// These are indexed for fast lookup based on the point index number and are
/// therefore used for commands that are received from the DNP3 master.
////////////////////////////////////////////////////////////////////////////////
struct Dnp3IndexedGroup {
    /// The size of the items array
    std::uint16_t size;
    /// The items array. Members in this array may be nullptr if they are
    /// not mapped to a glue variable.
    ConstGlueVariable** items;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Defines a glue variable that is mapped to a particular group and
/// variation for DNP3.
////////////////////////////////////////////////////////////////////////////////
struct DNP3MappedGlueVariable {
    /// The DNP3 group for this variable.
    std::uint8_t group;
    /// The DNP3 point index number for this variable.
    std::uint16_t point_index_number;
    /// The located variable that is glued to location.
    const GlueVariable* variable;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Defines the list of variables that are mapped from located variables
/// to DNP3. You can essentially iterate over this list to find every
/// located variable that is mapped.
////////////////////////////////////////////////////////////////////////////////
struct Dnp3MappedGroup {
    /// The size of the items array
    std::uint16_t size;
    /// The items array. Members in this array may be nullptr if they are
    /// not mapped to a glue variable.
    DNP3MappedGlueVariable* items;

    /// Gets the number of items in the specified group.
    std::uint16_t group_size(const std::uint8_t group) {
        std::uint16_t num(0);
        for (std::uint16_t i(0); i < size; ++i) {
            if (items[i].group == group) {
                num += 1;
            }
        }
        return num;
    }
};

////////////////////////////////////////////////////////////////////////////////
/// @brief The mapping of glue variables into this DNP3 outstation.
////////////////////////////////////////////////////////////////////////////////
struct Dnp3BoundGlueVariables {
    Dnp3BoundGlueVariables(
        std::mutex* buffer_lock,
        std::uint16_t binary_commands_size,
        std::uint16_t analog_commands_size,
        std::uint16_t measurements_size
    ) :

        buffer_lock(buffer_lock),
        binary_commands({ .size=0, .items=nullptr }),
        analog_commands({ .size=0, .items=nullptr }),
        measurements({ .size=0, .items=nullptr })
    {}

    /// @brief Mutex for the glue variables associated with this structures.
    std::mutex* buffer_lock;

    /// @brief Structure of bound glue variables for binary commands
    Dnp3IndexedGroup binary_commands;
    /// @brief Structure of bound glue variables for analog commands
    Dnp3IndexedGroup analog_commands;

    /// @brief All measurements that are sent from this outstation to the
    /// master.
    Dnp3MappedGroup measurements;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Create the outstation stack configuration using the configration
/// settings as specified in the stream.
/// @param cfg_stream The stream to read for configuration settings.
/// @param binding The mutex for exclusive access to glue variable values.
/// @return The configuration represented by the stream and any defaults and
///         the range mapping.
////////////////////////////////////////////////////////////////////////////////
asiodnp3::OutstationStackConfig dnp3_create_config(std::istream& cfg_stream,
                        const GlueVariablesBinding& binding,
                        Dnp3IndexedGroup& binary_commands,
                        Dnp3IndexedGroup& analog_commands,
                        Dnp3MappedGroup& measurements,
                        uint16_t& port);

/// @brief Start the DNP3 server running on the specified port and configured
/// using the specified stream.
///
/// The stream is specified as a function so that this function will close the
/// stream as soon as it is done with the stream.
/// @param cfg_stream An input stream to read configuration information
///                   from. This will be reset once use of the stream has
///                   been completed.
/// @param cfg_overrides A config string with the port number.
/// @param run A signal for running this server. This server terminates when
///            this signal is false.
/// @param glue_variables The glue variables that may be bound into this
///                       server.
void dnp3s_start_server(std::unique_ptr<std::istream, std::function<void(std::istream*)>>& cfg_stream,
                        const char* cfg_overrides,
                        volatile bool& run,
                        const GlueVariablesBinding& glue_variables);

/// @brief Start the DNP3 server running on the specified port and configured
/// using the specified stream.
///
/// The stream is specified as a function so that this function will close the
/// stream as soon as it is done with the stream.
/// @param glue_variables The glue variables that may be bound into this
///                       server.
/// @param run A signal for running this server. This server terminates when
///            this signal is false.
/// @param config The custom configuration for this service.
void dnp3s_service_run(const GlueVariablesBinding& binding,
                       volatile bool& run, const char* config);

/** @}*/

#endif  // CORE_DNP3S_DNP3_H_
