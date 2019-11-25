// Copyright 2015 Thiago Alves
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

#ifndef RUNTIME_CORE_MODBUSMASTER_MASTER_H_
#define RUNTIME_CORE_MODBUSMASTER_MASTER_H_

#include <cstdint>
#include <chrono>
#include <array>
#ifndef OPLC_UNIT_TEST
#include <modbus.h>
#endif  // OPLC_UNIT_TEST

/** \addtogroup openplc_runtime
 *  @{
 */

class GlueVariablesBinding;

namespace oplc
{
namespace modbusm
{

#define MODBUS_MASTER_MAX 3

// These definitions declare common items that are shared between different
// modbus master implementations. Their purpose is to make it easy to
// configure different mappings, but share much of the definition.

/// @brief Defines the protocol that is selected by user configuration.
enum MasterProtocol
{
    /// @brief Indicates a protocol that is unknown.
    ProtocolInvalid = 0,
    /// @brief Use the TCP protocol.
    ProtocolTcp,
    /// @brief Use the RTU protocol.
    ProtocolRtu,
};

enum BackoffStrategy
{
    /// Don't use any strategy.
    BackoffStrategyNone = 0,
    /// Backoff according to the number of failed attempts.
    BackoffStrategyLinear,
};

/// How big of a buffer do we reserve for string items.
const std::uint8_t MASTER_ITEM_SIZE(100);

/// @brief Defines the configuration information for a particular modbus
/// master.
struct MasterConfig
{
    /// A name, mostly for the purpose of logging.
    char name[MASTER_ITEM_SIZE];
    /// Which protocol do we use for communication.
    MasterProtocol protocol;
    /// Which strategy do we use for failed communication.
    BackoffStrategy backoff_strategy;
    /// The ID of the slave.
    std::uint8_t slave_id;
    /// The IP address (if using TCP).
    char ip_address[MASTER_ITEM_SIZE];
    /// The port (if using TCP).
    std::uint16_t ip_port;
    std::uint16_t rtu_baud_rate;
    std::uint8_t rtu_parity;
    std::uint16_t rtu_data_bit;
    std::uint16_t rtu_stop_bit;  
    /// Device specific timeout.
    std::chrono::microseconds response_timeout;
    /// Device specific polling period
    std::chrono::microseconds poll_interval;

    MasterConfig();

    /// @brief Create the context for the device according to the
    /// configuration parameters provided by the user.
    /// @return the created context.
    modbus_t* create() const;
};

/// @brief Read common configuration information that relates to defining a
/// modbus master. This function would normally be called from the
/// config handler implementation that is specific to a binding strategy.
///
/// This function does not take the section name - it is assumed that this
/// is called from a context that already filters out unrelated sections.
///
/// @param masters An array of master definitions that
/// contains the read definitions.
/// @param section The config item name from the configuration file.
/// @param value. The config item value.
/// @return 0 if the name and value were handled, otherwise -1.
std::int16_t common_cfg_handler(
    std::array<MasterConfig, MODBUS_MASTER_MAX>& masters,
    const char* name, const char* value
);

}  //  namespace modbusm
}  //  namespace opld

/** @}*/

#endif  // RUNTIME_CORE_MODBUSMASTER_MASTER_H_
