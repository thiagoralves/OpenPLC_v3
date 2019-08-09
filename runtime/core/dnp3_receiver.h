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

#ifndef CORE_DNP3_RECEIVER_H_
#define CORE_DNP3_RECEIVER_H_

#include <cstdint>
#include <memory>
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
    Dnp3Receiver(
        std::shared_ptr<GlueVariables> glue_variables,
        Dnp3Range range);

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

 protected:
    void Start() final;
    void End() final;

 private:
    template<class T>
    opendnp3::CommandStatus UpdateGlueVariable(T value, std::uint16_t dnp3_index) const;

 private:
    /// The buffers for data transfer.
    std::shared_ptr<GlueVariables> glue_variables;

    /// The offsets into the glue that are valid for this instance.
    const Dnp3Range range;
};

#endif  //  CORE_DNP3_RECEIVER_H_

/** @}*/
