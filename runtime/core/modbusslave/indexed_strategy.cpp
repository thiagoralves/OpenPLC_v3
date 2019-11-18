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

#include <cstdint>
#include <cstring>
#include <algorithm>

#include "indexed_strategy.h"
#include "mb_util.h"
#include "../glue.h"

/** \addtogroup openplc_runtime
 *  @{
 */

#define MAX_REGISTERS                   8192

#define MIN_16B_RANGE                   1024
#define MAX_16B_RANGE                   2047
#define MIN_32B_RANGE                   2048
#define MAX_32B_RANGE                   4095
#define MIN_64B_RANGE                   4096
#define MAX_64B_RANGE                   8191

using namespace std;

const uint8_t BOOL_PER_GROUP(8);

inline size_t bool_group_size(const GlueBoolGroup* group) {
    size_t size(0);
    for (uint8_t index = 0; index < BOOL_PER_GROUP; ++index) {
        if (group->values[index]) {
            size += 1;
        }
    }
    return size;
}

int32_t find_bounds(const GlueVariablesBinding& bindings, IecGlueValueType type,
                   IecLocationDirection dir) {
    int32_t max_index(-1);

    const GlueVariable* glue_variables = bindings.glue_variables;
    for (size_t index = 0; index < bindings.size; ++index) {
        if (type == glue_variables[index].type && dir == glue_variables[index].dir) {
            max_index = max(max_index, static_cast<int32_t>(glue_variables[index].msi));
        }
    }

    return max_index;
}

IndexedStrategy::IndexedStrategy(const GlueVariablesBinding& bindings) {

    const int32_t max_coil_index = find_bounds(bindings, IECVT_BOOL, IECLDT_OUT);
    const int32_t max_di_index = find_bounds(bindings, IECVT_BOOL, IECLDT_IN);

    // Allocate a big enough read and write buffers
    if (max_coil_index >= 0) {
        coil_read_buffer.resize((max_coil_index + 1) * BOOL_PER_GROUP);
        coil_write_buffer.resize((max_coil_index + 1) * BOOL_PER_GROUP);
    }

    if (max_di_index >= 0) {
        di_read_buffer.resize((max_di_index + 1) * BOOL_PER_GROUP);
    }

    // Now go through the items and assign the pointers and initial values
    // Note that if we have persistent storage, then the values have already
    // been initialized.

    const GlueVariable* glue_variables = bindings.glue_variables;
    for (size_t index = 0; index < bindings.size; ++index) {
        IecGlueValueType type = glue_variables[index].type;
        IecLocationDirection dir = glue_variables[index].dir;
        uint16_t msi = glue_variables[index].msi;

        if (type == IECVT_BOOL) {
            const GlueBoolGroup* group = reinterpret_cast<const GlueBoolGroup*>(glue_variables[index].value);
            // The first coil index for this variable is always
            // multiplied by the number of booleans in that group
            // If this index is out of range, then skip it.
            if (dir == IECLDT_OUT) {
                auto coil_start_index = msi * BOOL_PER_GROUP;
                for (size_t bool_index = 0; bool_index < BOOL_PER_GROUP; ++bool_index) {
                    auto coil_index = coil_start_index + bool_index;
                    if (group->values[bool_index]) {
                        coil_read_buffer[coil_index].value = group->values[bool_index];
                        coil_read_buffer[coil_index].cached_value = *group->values[bool_index];
                    }
                }
            } else if (dir == IECLDT_IN) {
                auto di_start_index = msi * BOOL_PER_GROUP;
                for (size_t bool_index = 0; bool_index < BOOL_PER_GROUP; ++bool_index) {
                    auto coil_index = di_start_index + bool_index;
                    if (group->values[bool_index]) {
                        di_read_buffer[coil_index].value = group->values[bool_index];
                        di_read_buffer[coil_index].cached_value = *group->values[bool_index];
                    }
                }
            }
        }
        else if (type == IECVT_INT) {
            if (dir == IECLDT_OUT) {
                int_register_read_buffer[msi].value = reinterpret_cast<IEC_INT*>(glue_variables[index].value);
                int_register_read_buffer[msi].cached_value = *reinterpret_cast<IEC_INT*>(glue_variables[index].value);
            } else if (dir == IECLDT_MEM && msi >= MIN_16B_RANGE && msi < MAX_16B_RANGE) {
                intm_register_read_buffer[msi - MIN_16B_RANGE].value = reinterpret_cast<IEC_INT*>(glue_variables[index].value);
                intm_register_read_buffer[msi - MIN_16B_RANGE].cached_value = *reinterpret_cast<IEC_INT*>(glue_variables[index].value);
            } else if (dir == IECLDT_IN) {
                int_input_read_buffer[msi].value = reinterpret_cast<IEC_INT*>(glue_variables[index].value);
                int_input_read_buffer[msi].cached_value = *reinterpret_cast<IEC_INT*>(glue_variables[index].value);
            }
        }
        else if (type == IECVT_DINT && dir == IECLDT_MEM && msi <= MIN_32B_RANGE && msi < MAX_32B_RANGE) {
            dintm_register_read_buffer[msi - MIN_32B_RANGE].value = reinterpret_cast<IEC_DINT*>(glue_variables[index].value);
            dintm_register_read_buffer[msi - MIN_32B_RANGE].cached_value = *reinterpret_cast<IEC_DINT*>(glue_variables[index].value);
        }
        else if (type == IECVT_LINT && dir == IECLDT_MEM && msi <= MIN_64B_RANGE && msi < MAX_64B_RANGE) {
            lintm_register_read_buffer[msi - MIN_64B_RANGE].value = reinterpret_cast<IEC_LINT*>(glue_variables[index].value);
            lintm_register_read_buffer[msi - MIN_64B_RANGE].cached_value = *reinterpret_cast<IEC_LINT*>(glue_variables[index].value);
        }
    }
}

template <typename T>
void exchange(array<PendingValue<T>, NUM_REGISTER_VALUES>& write_buffer,
              array<MappedValue<T>, NUM_REGISTER_VALUES>& read_buffer) {
    for (size_t index = 0; index < write_buffer.size(); ++index) {
        if (!read_buffer[index].value) {
            continue;
        }

        if (write_buffer[index].has_pending) {
            *read_buffer[index].value = write_buffer[index].value;
        }
        read_buffer[index].cached_value = *read_buffer[index].value;
    }
}

void IndexedStrategy::Exchange() {
    // Since we already figured out the mapping in an efficient structure
    // the process of exchange is simply going through the items. We first
    // handle populating writes into the structure.
    for (size_t index = 0; index < coil_write_buffer.size(); ++index) {
        if (coil_write_buffer[index].has_pending) {
            *coil_read_buffer[index].value = coil_write_buffer[index].value;
        }
    }

    exchange(int_register_write_buffer, int_register_read_buffer);
    exchange(intm_register_write_buffer, intm_register_read_buffer);
    exchange(dintm_register_write_buffer, dintm_register_read_buffer);
    exchange(lintm_register_write_buffer, lintm_register_read_buffer);

    // Update the read caches
    for (size_t index = 0; index < coil_read_buffer.size(); ++index) {
        if (coil_read_buffer[index].value) {
            coil_read_buffer[index].cached_value = *coil_read_buffer[index].value;
        }
    }
    for (size_t index = 0; index < di_read_buffer.size(); ++index) {
        if (di_read_buffer[index].value) {
            di_read_buffer[index].cached_value = *di_read_buffer[index].value;
        }
    }
}

modbus_errno IndexedStrategy::WriteCoil(uint16_t coil_index, bool value) {
    lock_guard<mutex> guard(buffer_mutex);
    if (coil_index < coil_write_buffer.size() && coil_read_buffer[coil_index].value) {
        coil_write_buffer[coil_index].has_pending = true;
        coil_write_buffer[coil_index].value = value;
        return 0;
    }
    return -1;
}

modbus_errno IndexedStrategy::WriteMultipleCoils(uint16_t coil_start_index, uint16_t num_coils, uint8_t* values) {
    lock_guard<mutex> guard(buffer_mutex);

    const uint8_t bit_values[8] = {
        0x01,
        0x02,
        0x04,
        0x08,
        0x10,
        0x20,
        0x40,
        0x80,
    };

    if (coil_start_index + num_coils >= coil_write_buffer.size()) {
        return -1;
    }

    for (uint16_t index = 0; index < num_coils; ++index) {
        // Get the value from the packed structure
        bool value = values[index / 8] & bit_values[index % 8];
        coil_write_buffer[coil_start_index + index].has_pending = true;
        coil_write_buffer[coil_start_index + index].value = value;
    }

    return 0;
}

modbus_errno IndexedStrategy::ReadCoils(uint16_t coil_start_index, uint16_t num_values, uint8_t* values) {
    auto max_index = coil_start_index + num_values - 1;
    if (max_index >= coil_read_buffer.size()) {
        return -1;
    }

    const uint8_t bit_values[8] = {
        0x01,
        0x02,
        0x04,
        0x08,
        0x10,
        0x20,
        0x40,
        0x80,
    };

    // Start by filling with 0 - we know the buffer is bigger than this can be
    memset(values, 0, num_values / 8 + 1);

    lock_guard<mutex> guard(buffer_mutex);
    for (uint16_t index = 0; index < num_values; ++index) {
        values[index / 8] |= coil_read_buffer[coil_start_index + index].cached_value ? bit_values[index % 8] : 0;
    }

    return 0;
}

modbus_errno IndexedStrategy::ReadDiscreteInputs(uint16_t di_start_index, uint16_t num_values, uint8_t* values) {
    auto max_index = di_start_index + num_values - 1;
    if (max_index >= di_read_buffer.size()) {
        return -1;
    }

    const uint8_t bit_values[8] = {
        0x01,
        0x02,
        0x04,
        0x08,
        0x10,
        0x20,
        0x40,
        0x80,
    };

    // Start by filling with 0 - we know the buffer is bigger than this can be
    memset(values, 0, num_values / 8 + 1);

    lock_guard<mutex> guard(buffer_mutex);
    for (uint16_t index = 0; index < num_values; ++index) {
        values[index / 8] |= di_read_buffer[di_start_index + index].cached_value ? bit_values[index % 8] : 0;
    }

    return 0;
}

modbus_errno IndexedStrategy::WriteHoldingRegister(uint16_t hr_start_index, uint8_t* value) {
    lock_guard<mutex> guard(buffer_mutex);

    if (hr_start_index < MIN_16B_RANGE) {
        int_register_write_buffer[hr_start_index].has_pending = true;
        int_register_write_buffer[hr_start_index].value = mb_to_word(value[0], value[1]);
    } else if (hr_start_index < MAX_16B_RANGE) {
        hr_start_index -= MIN_16B_RANGE;
        intm_register_write_buffer[hr_start_index].has_pending = true;
        intm_register_write_buffer[hr_start_index].value = mb_to_word(value[0], value[1]);
    } else if (hr_start_index < MAX_32B_RANGE) {
        hr_start_index -= MIN_32B_RANGE;
        uint32_t temp_value = (uint32_t) mb_to_word(value[0], value[1]);
        PendingValue<IEC_DINT>& dst = dintm_register_write_buffer[hr_start_index / 2];
        dst.has_pending = true;

        if (hr_start_index % 2 == 0) {
            // First word
            dst.value = dst.value & 0x0000ffff;
            dst.value = dst.value | temp_value;
        } else {
            // Second word
            dst.value = dst.value & 0xffff0000;
            dst.value = dst.value | temp_value;
        }
    } else if (hr_start_index < MAX_64B_RANGE) {
        hr_start_index -= MIN_64B_RANGE;
        uint64_t temp_value = (uint64_t) mb_to_word(value[0], value[1]);
        PendingValue<IEC_LINT>& dst = lintm_register_write_buffer[hr_start_index / 4];
        dst.has_pending = true;

        if (hr_start_index % 4 == 0) {
            // First word
            dst.value = dst.value & 0x0000ffffffffffff;
            dst.value = dst.value | (temp_value << 48);
        }
        else if (hr_start_index % 4 == 1) {
            // Second word
            dst.value = dst.value & 0xffff0000ffffffff;
            dst.value = dst.value | (temp_value << 32);
        }
        else if (hr_start_index % 4 == 2) {
            // Third word
            dst.value = dst.value & 0xffffffff0000ffff;
            dst.value = dst.value | (temp_value << 16);
        }
        else if (hr_start_index % 4 == 3) {
            // Fourth word
            dst.value = dst.value & 0xffffffffffff0000;
            dst.value = dst.value | temp_value;
        }
    } else {
        return -1;
    }
    return 0;
}

modbus_errno IndexedStrategy::WriteHoldingRegisters(uint16_t hr_start_index, uint16_t num_registers, uint8_t* value) {
    lock_guard<mutex> guard(buffer_mutex);

    for (uint16_t index = 0; index < num_registers; ++index) {
        uint16_t hr_index = hr_start_index + index;

        if (hr_index < MIN_16B_RANGE) {
            int_register_write_buffer[hr_index].has_pending = true;
            int_register_write_buffer[hr_index].value = mb_to_word(value[0], value[1]);
        } else if (hr_index < MAX_16B_RANGE) {
            hr_index -= MIN_16B_RANGE;
            intm_register_write_buffer[hr_index].has_pending = true;
            intm_register_write_buffer[hr_index].value = mb_to_word(value[0], value[1]);
        } else if (hr_index < MAX_32B_RANGE) {
            hr_index -= MIN_32B_RANGE;
            uint32_t temp_value = (uint32_t) mb_to_word(value[0], value[1]);
            PendingValue<IEC_DINT>& dst = dintm_register_write_buffer[hr_index / 2];
            dst.has_pending = true;

            if (hr_index % 2 == 0) {
                // First word
                dst.value = dst.value & 0x0000ffff;
                dst.value = dst.value | temp_value;
            } else {
                // Second word
                dst.value = dst.value & 0xffff0000;
                dst.value = dst.value | temp_value;
            }
        } else if (hr_index < MAX_64B_RANGE) {
            hr_index -= MIN_64B_RANGE;
            uint64_t temp_value = (uint64_t) mb_to_word(value[0], value[1]);
            PendingValue<IEC_LINT>& dst = lintm_register_write_buffer[hr_index / 4];
            dst.has_pending = true;

            if (hr_index % 4 == 0) {
                // First word
                dst.value = dst.value & 0x0000ffffffffffff;
                dst.value = dst.value | (temp_value << 48);
            }
            else if (hr_index % 4 == 1) {
                // Second word
                dst.value = dst.value & 0xffff0000ffffffff;
                dst.value = dst.value | (temp_value << 32);
            }
            else if (hr_index % 4 == 2) {
                // Third word
                dst.value = dst.value & 0xffffffff0000ffff;
                dst.value = dst.value | (temp_value << 16);
            }
            else if (hr_index % 4 == 3) {
                // Fourth word
                dst.value = dst.value & 0xffffffffffff0000;
                dst.value = dst.value | temp_value;
            }
        } else {
            return -1;
        }

        value += 2;
    }
    return 0;
}

modbus_errno IndexedStrategy::ReadHoldingRegisters(uint16_t hr_start_index, uint16_t num_registers, uint8_t* value) {
    lock_guard<mutex> guard(buffer_mutex);

    for (uint16_t index = 0; index < num_registers; ++index) {
        uint16_t hr_index = hr_start_index + index;
        uint16_t val;
        if (hr_index < MIN_16B_RANGE) {
            val = int_register_read_buffer[hr_index].cached_value;
        } else if (hr_index < MAX_16B_RANGE) {
            hr_index -= MIN_16B_RANGE;
            val = intm_register_read_buffer[hr_index].cached_value;
        } else if (hr_index < MAX_32B_RANGE) {
            hr_index -= MIN_32B_RANGE;
            if (hr_index % 2 == 0) {
                val = (uint16_t)(dintm_register_read_buffer[hr_index / 2].cached_value >> 16);
            } else {
                val = (uint16_t)(dintm_register_read_buffer[hr_index / 2].cached_value & 0xffff);
            }
        } else if (hr_index < MAX_64B_RANGE) {
            hr_index -= MIN_64B_RANGE;
            if (hr_index %4 == 0) {
                val = (uint16_t)(lintm_register_read_buffer[hr_index / 4].cached_value >> 48);
            } else if (hr_index %4 == 1) {
                val = (uint16_t)(lintm_register_read_buffer[hr_index / 4].cached_value >> 32);
            } else if (hr_index %4 == 2) {
                val = (uint16_t)(lintm_register_read_buffer[hr_index / 4].cached_value >> 16);
            } else if (hr_index %4 == 3) {
                val = (uint16_t)(lintm_register_read_buffer[hr_index / 4].cached_value & 0xffff);
            }
        } else {
            return -1;
        }

        value[0] = mb_high_byte(val);
        value[1] = mb_low_byte(val);
        // Move to the next 16-bit position
        value += 2;
    }
    return 0;
}

modbus_errno IndexedStrategy::ReadInputRegisters(uint16_t hr_start_index, uint16_t num_registers, uint8_t* value) {
    lock_guard<mutex> guard(buffer_mutex);

    for (uint16_t index = 0; index < num_registers; ++index) {
        uint16_t hr_index = hr_start_index + index;
        uint16_t val;
        if (hr_index < MIN_16B_RANGE) {
            val = int_input_read_buffer[hr_index].cached_value;
        } else {
            return -1;
        }

        value[0] = mb_high_byte(val);
        value[1] = mb_low_byte(val);
        // Move to the next 16-bit position
        value += 2;
    }
    return 0;
}

/** @}*/
