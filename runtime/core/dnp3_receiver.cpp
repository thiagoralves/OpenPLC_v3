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

#ifdef OPLC_DNP3_OUTSTATION

#include <spdlog/spdlog.h>

#include "dnp3_receiver.h"

using namespace opendnp3;
using namespace std;

/** \addtogroup openplc_runtime
 *  @{
 */

/// Maps the DNP3 index to the index in our glue variables, returning index or < 0
/// if the value is not in the range of mapped glue variables.
/// @param start The start index of valid ranges.
/// @param end One past the last valid index.
/// @param offset The offset defined for this set of values.
/// @param dnp3_index The index of the point for DNP3.
/// @return Non-negative glue index if the dnp3 index is valid, otherwise negative.
inline int16_t mapDnp3IndexToGlueIndex(uint16_t start, uint16_t stop, uint16_t offset, uint16_t dnp3_index) {
    int16_t glue_index = dnp3_index + offset;
    return glue_index >= start && glue_index < stop ? glue_index : -1;
}

/// Map the DNP3 index to a command status value. The status value is successful
/// if the index maps to something in the valid range, otherwise, maps to out of range.
/// @param start The start index of valid ranges.
/// @param end One past the last valid index.
/// @param offset The offset defined for this set of values.
/// @param dnp3_index The index of the point for DNP3.
/// @return CommandStatus::SUCCESS if the DNP3 index is in range, otherwise, CommandStatus::OUT_OF_RANGE.
inline CommandStatus mapDnp3IndexToStatus(uint16_t start, uint16_t stop, uint16_t offset, uint16_t dnp3_index) {
    return mapDnp3IndexToGlueIndex(start, stop, offset, dnp3_index) >= 0 ? CommandStatus::SUCCESS : CommandStatus::OUT_OF_RANGE;
}

/// Initialize a new instance of the DNP3 receiver. The receiver listens for point value updates
/// over the DNP3 channel and then maps those to the glue variables.
/// @param glue_variables The glue variables that we map onto.
/// @param range The valid range in the glue that we allow.
Dnp3Receiver::Dnp3Receiver(
    std::shared_ptr<GlueVariables> glue_variables,
    Dnp3Range range) :

    glue_variables(glue_variables),
    range(range)
{ }

/// CROB
CommandStatus Dnp3Receiver::Select(const ControlRelayOutputBlock& command, uint16_t index) {
    spdlog::trace("DNP3 select CROB index");
    return mapDnp3IndexToStatus(range.bool_outputs_start, range.bool_outputs_end, range.bool_outputs_offset, index);
}

CommandStatus Dnp3Receiver::Operate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType) {
    spdlog::trace("DNP3 operate CROB");
    auto code = command.functionCode;
    auto glue_index = mapDnp3IndexToGlueIndex(range.bool_outputs_start, range.bool_outputs_end, range.bool_outputs_offset, index);

    if (glue_index < 0) {
        return CommandStatus::OUT_OF_RANGE;
    } else if (code != ControlCode::LATCH_ON && code != ControlCode::LATCH_OFF) {
        return CommandStatus::NOT_SUPPORTED;
    }

    IEC_BOOL crob_val = (code == ControlCode::LATCH_ON);

    IEC_BOOL* glue = glue_variables->BoolOutputAt(index, 0);
    if (glue != nullptr) {
        std::lock_guard<std::mutex> lock(*glue_variables->buffer_lock);
        *glue = crob_val;
    }

    return CommandStatus::SUCCESS;
}

// AnalogOut 16 (Int)
CommandStatus Dnp3Receiver::Select(const AnalogOutputInt16& command, uint16_t index) {
    CommandStatus status = mapDnp3IndexToStatus(range.outputs_start, range.outputs_end, range.outputs_offset, index);
    spdlog::trace("DNP3 select AO int16 point status");
    return status;
}

CommandStatus Dnp3Receiver::Operate(const AnalogOutputInt16& command, uint16_t index, OperateType opType) {
    spdlog::trace("DNP3 select AO int16 point status");
    return this->UpdateGlueVariable<int16_t>(command.value, index);
}

// AnalogOut 32 (Int)
CommandStatus Dnp3Receiver::Select(const AnalogOutputInt32& command, uint16_t index) {
    CommandStatus status = mapDnp3IndexToStatus(range.outputs_start, range.outputs_end, range.outputs_offset, index);
    spdlog::trace("DNP3 select AO int32 point  status");
    return status;
}

CommandStatus Dnp3Receiver::Operate(const AnalogOutputInt32& command, uint16_t index, OperateType opType) {
    spdlog::trace("DNP3 select AO int32 point status");
    return this->UpdateGlueVariable<int32_t>(command.value, index);
}

// AnalogOut 32 (Float)
CommandStatus Dnp3Receiver::Select(const AnalogOutputFloat32& command, uint16_t index) {
    CommandStatus status = mapDnp3IndexToStatus(range.outputs_start, range.outputs_end, range.outputs_offset, index);
    spdlog::trace("DNP3 select AO float32 point status");
    return status;
}

CommandStatus Dnp3Receiver::Operate(const AnalogOutputFloat32& command, uint16_t index, OperateType opType) {
    spdlog::trace("DNP3 select AO float32 point status");
    return this->UpdateGlueVariable<float>(command.value, index);
}

// AnalogOut 64
CommandStatus Dnp3Receiver::Select(const AnalogOutputDouble64& command, uint16_t index) {
    CommandStatus status = mapDnp3IndexToStatus(range.outputs_start, range.outputs_end, range.outputs_offset, index);
    spdlog::trace("DNP3 select AO double64 point status");
    return status;
}

CommandStatus Dnp3Receiver::Operate(const AnalogOutputDouble64& command, uint16_t index, OperateType opType) {
    spdlog::trace("DNP3 select AO double64 point status");
    return this->UpdateGlueVariable<double>(command.value, index);
}

template<class T>
CommandStatus Dnp3Receiver::UpdateGlueVariable(T value, uint16_t dnp3_index) const {
    int16_t glue_index = mapDnp3IndexToGlueIndex(range.outputs_start, range.outputs_end, range.outputs_offset, dnp3_index);

    if (glue_index < 0 || glue_index >= glue_variables->outputs_size) {
        spdlog::trace("DNP3 update point is out of mapped glue range");
        return CommandStatus::OUT_OF_RANGE;
    }

    // Next check if we have a value mapped at that particular index in the glue variables (the IO locations can have holes)
    IecGlueValueType type = glue_variables->outputs[glue_index].type;
    void* value_container = glue_variables->outputs[glue_index].value;

    if (type == IECVT_UNASSIGNED || value_container == nullptr) {
        spdlog::trace("DNP3 update point for glue index is not mapped");
        return CommandStatus::OUT_OF_RANGE;
    }

    // We have a container we can write to, but we need to update the value as appropriate
    // for the particular value container type.
    std::lock_guard<std::mutex> lock(*glue_variables->buffer_lock);
    CommandStatus status = CommandStatus::SUCCESS;
    switch (type) {
        case IECVT_BYTE:
        {
            *(reinterpret_cast<IEC_BYTE*>(value_container)) = static_cast<IEC_BYTE>(value);
            break;
        }
        case IECVT_SINT:
        {
            *(reinterpret_cast<IEC_SINT*>(value_container)) = static_cast<IEC_SINT>(value);
            break;
        }
        case IECVT_USINT:
        {
            *(reinterpret_cast<IEC_USINT*>(value_container)) = static_cast<IEC_USINT>(value);
            break;
        }
        case IECVT_INT:
        {
            *(reinterpret_cast<IEC_INT*>(value_container)) = static_cast<IEC_INT>(value);
            break;
        }
        case IECVT_UINT:
        {
            *(reinterpret_cast<IEC_USINT*>(value_container)) = static_cast<IEC_USINT>(value);
            break;
        }
        case IECVT_WORD:
        {
            *(reinterpret_cast<IEC_WORD*>(value_container)) = static_cast<IEC_WORD>(value);
            break;
        }
        case IECVT_DINT:
        {
            *(reinterpret_cast<IEC_DINT*>(value_container)) = static_cast<IEC_DINT>(value);
            break;
        }
        case IECVT_UDINT:
        {
            *(reinterpret_cast<IEC_UDINT*>(value_container)) = static_cast<IEC_UDINT>(value);
            break;
        }
        case IECVT_DWORD:
        {
            *(reinterpret_cast<IEC_DWORD*>(value_container)) = static_cast<IEC_DWORD>(value);
            break;
        }
        case IECVT_REAL:
        {
            *(reinterpret_cast<IEC_REAL*>(value_container)) = static_cast<IEC_REAL>(value);
            break;
        }
        case IECVT_LREAL:
        {
            *(reinterpret_cast<IEC_LREAL*>(value_container)) = static_cast<IEC_LREAL>(value);
            break;
        }
        case IECVT_LWORD:
        {
            *(reinterpret_cast<IEC_LWORD*>(value_container)) = static_cast<IEC_LWORD>(value);
            break;
        }
        case IECVT_LINT:
        {
            *(reinterpret_cast<IEC_LINT*>(value_container)) = static_cast<IEC_LINT>(value);
            break;
        }
        case IECVT_ULINT:
        {
            *(reinterpret_cast<IEC_ULINT*>(value_container)) = static_cast<IEC_ULINT>(value);
            break;
        }
        default:
        {
            status = CommandStatus::NOT_SUPPORTED;
            break;
        }
    }

    return status;
}

void Dnp3Receiver::Start() {
    spdlog::info("DNP3 Receiver Started");
}

void Dnp3Receiver::End() {
    spdlog::info("DNP3 Receiver Stopped");
}

#endif  // OPLC_DNP3_OUTSTATION

/** @}*/