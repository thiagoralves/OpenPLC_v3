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

#ifndef CORE_DNP3_DNP3_RECEIVER_H_
#define CORE_DNP3_DNP3_RECEIVER_H_

#include <cstdint>
#include <opendnp3/outstation/SimpleCommandHandler.h>

#include "dnp3.h"
#include "glue.h"

/** \addtogroup openplc_runtime
 *  @{
 */

////////////////////////////////////////////////////////////////////////////////
/// \brief The receiver defines the interface between the DNP3 channel
/// and the glue arrays that are written to from DNP3.
////////////////////////////////////////////////////////////////////////////////
class Dnp3Receiver : public opendnp3::ICommandHandler {
 public:
    /// Initialize a new instance of the DNP3 receiver. The receiver listens for point value updates
    /// over the DNP3 channel and then maps those to the glue variables.
    /// @param binary_commands The glue variables for the binary commands.
    /// @param analog_commands The glue variables for the analog commands.
    Dnp3Receiver(const Dnp3IndexedGroup& binary_commands, const Dnp3IndexedGroup& analog_commands);

    opendnp3::CommandStatus Select(const opendnp3::ControlRelayOutputBlock& command, std::uint16_t index) override;

    opendnp3::CommandStatus Operate(const opendnp3::ControlRelayOutputBlock& command, std::uint16_t index, opendnp3::OperateType opType) override;

    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt16& command, std::uint16_t index) override;

    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt16& command, std::uint16_t index, opendnp3::OperateType opType) override;

    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt32& command, std::uint16_t index) override;

    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt32& command, std::uint16_t index, opendnp3::OperateType opType) override;

    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputFloat32& command, std::uint16_t index) override;

    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputFloat32& command, std::uint16_t index, opendnp3::OperateType opType) override;

    opendnp3::CommandStatus Select(const opendnp3::AnalogOutputDouble64& command, std::uint16_t index) override;

    opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputDouble64& command, std::uint16_t index, opendnp3::OperateType opType) override;

    void ExchangeGlue();
 protected:
    void Start() final;
    void End() final;

 private:
    template<class T>
    opendnp3::CommandStatus CacheUpdatedValue(T value, std::uint16_t dnp3_index);

    template <typename T>
    struct CacheItem {
       bool has_value;
       T value;
    };

 private:
    /// The buffers for data transfer.
    const Dnp3IndexedGroup& binary_commands;
    const Dnp3IndexedGroup& analog_commands;

    std::mutex cache_mutex;

    CacheItem<bool>* const binary_commands_cache;
    CacheItem<double>* const analog_commands_cache;
};

#endif  //  CORE_DNP3_DNP3_RECEIVER_H_

/** @}*/
