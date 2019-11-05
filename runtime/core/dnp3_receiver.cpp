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

#include <cstring>
#include <spdlog/spdlog.h>

#include "dnp3_receiver.h"

using namespace opendnp3;
using namespace std;

/** \addtogroup openplc_runtime
 *  @{
 */

////////////////////////////////////////////////////////////////////////////////
/// @brief Is the specified DNP3 point something that we can map?
/// @param data_index The index of the point from DNP3.
/// @param group The group based on the command type.
////////////////////////////////////////////////////////////////////////////////
inline CommandStatus mapIsValidDnp3Index(uint16_t data_index, const Dnp3IndexedGroup& group) {
    return (data_index < group.size && group.items[data_index]->value) ? CommandStatus::SUCCESS : CommandStatus::OUT_OF_RANGE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Is the specified DNP3 point something that we can map?
/// @param data_index The index of the point from DNP3.
/// @param group The group based on the command type.
////////////////////////////////////////////////////////////////////////////////
inline bool isValidDnp3Index(uint16_t data_index, const Dnp3IndexedGroup& group) {
    return (data_index < group.size && group.items[data_index]->value);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Maps the DNP3 index to the index in our glue variables, returning
/// index or < 0 if the value is not in the range of mapped glue variables.
/// @param start The start index of valid ranges.
/// @param end One past the last valid index.
/// @param offset The offset defined for this set of values.
/// @param dnp3_index The index of the point for DNP3.
/// @return Non-negative glue index if the dnp3 index is valid, otherwise negative.

inline int16_t mapDnp3IndexToGlueIndex(uint16_t start, uint16_t stop, uint16_t offset, uint16_t dnp3_index) {
    int16_t glue_index = dnp3_index + offset;
    return glue_index >= start && glue_index < stop ? glue_index : -1;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Map the DNP3 index to a command status value. The status value is successful
/// if the index maps to something in the valid range, otherwise, maps to out of range.
/// @param start The start index of valid ranges.
/// @param end One past the last valid index.
/// @param offset The offset defined for this set of values.
/// @param dnp3_index The index of the point for DNP3.
/// @return CommandStatus::SUCCESS if the DNP3 index is in range, otherwise, CommandStatus::OUT_OF_RANGE.
////////////////////////////////////////////////////////////////////////////////
inline CommandStatus mapDnp3IndexToStatus(uint16_t start, uint16_t stop, uint16_t offset, uint16_t dnp3_index) {
    return mapDnp3IndexToGlueIndex(start, stop, offset, dnp3_index) >= 0 ? CommandStatus::SUCCESS : CommandStatus::OUT_OF_RANGE;
}

Dnp3Receiver::Dnp3Receiver(const Dnp3IndexedGroup& binary_commands, const Dnp3IndexedGroup& analog_commands) :

    binary_commands(binary_commands),
    analog_commands(analog_commands),
    binary_commands_cache(binary_commands.size > 0 ? new CacheItem<bool>[binary_commands.size] : nullptr),
    analog_commands_cache(analog_commands.size > 0 ? new CacheItem<double>[analog_commands.size] : nullptr)
{
    // We need to zero out the caches so that we don't think there is something
    // that we can handle on the first cycle
    if (binary_commands_cache != nullptr) {
        memset(binary_commands_cache, 0, sizeof(CacheItem<bool>) * binary_commands.size);
    }
    if (analog_commands_cache != nullptr) {
        memset(analog_commands_cache, 0, sizeof(CacheItem<double>) * analog_commands.size);
    }
}

/// CROB
CommandStatus Dnp3Receiver::Select(const ControlRelayOutputBlock& command, uint16_t index) {
    spdlog::trace("DNP3 select CROB index");
    return mapIsValidDnp3Index(index, binary_commands);
}

CommandStatus Dnp3Receiver::Operate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType) {
    auto code = command.functionCode;

    if (!isValidDnp3Index(index, binary_commands)) {
        return CommandStatus::OUT_OF_RANGE;
    }
    if (code != ControlCode::LATCH_ON && code != ControlCode::LATCH_OFF) {
        return CommandStatus::NOT_SUPPORTED;
    }

    lock_guard<mutex> cache_guard(cache_mutex);
    binary_commands_cache[index].has_value = true;
    binary_commands_cache[index].value = (code == ControlCode::LATCH_ON);

    return CommandStatus::SUCCESS;
}

// AnalogOut 16 (Int)
CommandStatus Dnp3Receiver::Select(const AnalogOutputInt16& command, uint16_t index) {
    spdlog::trace("DNP3 select AO int16 point status");
    return mapIsValidDnp3Index(index, analog_commands);
}

CommandStatus Dnp3Receiver::Operate(const AnalogOutputInt16& command, uint16_t index, OperateType opType) {
    spdlog::trace("DNP3 select AO int16 point status: {} written at index: {}", command.value, index);
    return this->CacheUpdatedValue<int16_t>(command.value, index);
}

// AnalogOut 32 (Int)
CommandStatus Dnp3Receiver::Select(const AnalogOutputInt32& command, uint16_t index) {
    spdlog::trace("DNP3 select AO int32 point status");
    return mapIsValidDnp3Index(index, analog_commands);
}

CommandStatus Dnp3Receiver::Operate(const AnalogOutputInt32& command, uint16_t index, OperateType opType) {
    spdlog::trace("DNP3 select AO int32 point status: {} written at index: {}", command.value, index);
    return this->CacheUpdatedValue<int32_t>(command.value, index);
}

// AnalogOut 32 (Float)
CommandStatus Dnp3Receiver::Select(const AnalogOutputFloat32& command, uint16_t index) {
    spdlog::trace("DNP3 select AO float32 point status");
    return mapIsValidDnp3Index(index, analog_commands);
}

CommandStatus Dnp3Receiver::Operate(const AnalogOutputFloat32& command, uint16_t index, OperateType opType) {
    spdlog::trace("DNP3 select AO float32 point status: {} written at index: {}", command.value, index);
    return this->CacheUpdatedValue<float>(command.value, index);
}

// AnalogOut 64
CommandStatus Dnp3Receiver::Select(const AnalogOutputDouble64& command, uint16_t index) {
    spdlog::trace("DNP3 select AO double64 point status");
    return mapIsValidDnp3Index(index, analog_commands);
}

CommandStatus Dnp3Receiver::Operate(const AnalogOutputDouble64& command, uint16_t index, OperateType opType) {
    spdlog::trace("DNP3 select AO double64 point status: {} written at index: {}", command.value, index);
    return this->CacheUpdatedValue<double>(command.value, index);
}

/// @brief Update a value in our cache. This cache is designed to operate fast
/// so that we return immediately without waiting on the glue variables lock.
/// This is quite important because the lock on the glue variables can be very
/// long (the cycle time for the PLC logic) and that would be far too long to
/// wait for DNP3 values.
/// @param value The value received over DNP3.
/// @param dnp3_index The index of the value.
template<class T>
CommandStatus Dnp3Receiver::CacheUpdatedValue(T value, uint16_t dnp3_index) {
    if (!isValidDnp3Index(dnp3_index, analog_commands)) {
        spdlog::trace("DNP3 update point at index {} is not glued", dnp3_index);
        return CommandStatus::OUT_OF_RANGE;
    }

    lock_guard<mutex> cache_guard(cache_mutex);
    analog_commands_cache[dnp3_index].has_value = true;
    analog_commands_cache[dnp3_index].value = static_cast<double>(value);

    return CommandStatus::SUCCESS;
}

/// @brief Write the point values into the glue variables for each value that
/// was received.
void Dnp3Receiver::ExchangeGlue() {
    // Acquire the locks to do the data exchange
    lock_guard<mutex> cache_guard(cache_mutex);

    for (uint16_t data_index(0); data_index < binary_commands.size; ++data_index) {
        if (binary_commands_cache[data_index].has_value) {
            binary_commands_cache[data_index].has_value = false;
            const GlueVariable* variable = binary_commands.items[data_index];
            if (!variable) {
                continue;
            }

            (*((GlueBoolGroup*)variable->value)->values[0]) = binary_commands_cache[data_index].value;
        }
    }

    for (uint16_t data_index(0); data_index < analog_commands.size; ++data_index) {
        if (analog_commands_cache[data_index].has_value) {
            analog_commands_cache[data_index].has_value = false;

            const GlueVariable* variable = analog_commands.items[data_index];
            if (!variable) {
                continue;
            }

            double value = analog_commands_cache[data_index].value;
            void* value_container = variable->value;
            switch (variable->type) {
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
            }
        }
    }
}

void Dnp3Receiver::Start() {
    spdlog::info("DNP3 receiver started");
}

void Dnp3Receiver::End() {
    spdlog::info("DNP3 receiver stopped");
}

#endif  // OPLC_DNP3_OUTSTATION

/** @}*/