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

#ifndef RUNTIME_CORE_MODBUSMASTER_INDEXED_STRATEGY_H_
#define RUNTIME_CORE_MODBUSMASTER_INDEXED_STRATEGY_H_

#include <cstdint>

#include "glue.h"
#include "master.h"

namespace oplc
{
namespace modbusm
{

/** \addtogroup openplc_runtime
 *  @{
 */

#define MAX_MB_IO           400

/// @brief Define the mapping for modbus addresses to located variables
/// that is is based on a sequence starting an an offset in the located
/// variables and an offset in the Modbus server.
struct ModbusAddress
{
    /// The offset in the mapped buffer for exchange with the PLC.
    std::uint16_t buffer_offset;
    /// The start address on the remote server
    std::uint16_t start_address;
    /// The number of addresses on the remote server.
    std::uint16_t num_regs;

    inline void set_offset(ModbusAddress& prior)
    {
        buffer_offset = prior.buffer_offset + prior.num_regs;
    }
};

struct IndexedMapping
{
    ModbusAddress discrete_inputs;
    ModbusAddress coils;
    ModbusAddress input_registers;
    ModbusAddress holding_read_registers;
    ModbusAddress holding_registers;

    void set_offsets(IndexedMapping& prior)
    {
        // Index for the bool inputs
        discrete_inputs.set_offset(prior.discrete_inputs);
        // Index for the bool outputs
        coils.set_offset(prior.coils);
        // Index for the int inputs
        input_registers.set_offset(prior.input_registers);
        holding_read_registers.set_offset(prior.holding_read_registers);
        // Index for the int outputs
        holding_registers.set_offset(prior.holding_registers);
    }
};

/// @brief The main entry point for the thread that is responsible for polling
/// the remote server.
/// @param args The supplied data to the thread. This must be a pointer to
/// the IndexedMaster structure.
void* modbus_master_indexed_poll(void* args);

/// @brief Start the modbus master service.
///
/// @param glue_variables The glue variables that may be bound into this
///                       service.
/// @param run A signal for running this service. This service terminates when
///            this signal is false.
/// @param config The custom configuration for this service.
void modbus_master_service_run(const GlueVariablesBinding& binding,
                               volatile bool& run, const char* config);

/// @brief Lifecycle method called just prior to the PLC cycle.
void modbus_master_before_cycle();

/// @brief Lifecycle method called just after the PLC cycle.
void modbus_master_after_cycle();

}  // namespace modbusm
}  // namespace oplc

/** @}*/

#endif  // RUNTIME_CORE_MODBUSMASTER_INDEXED_STRATEGY_H_
