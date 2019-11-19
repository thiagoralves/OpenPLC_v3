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

#define MIN_16B_RANGE                   1024
#define MAX_16B_RANGE                   2047
#define MIN_32B_RANGE                   2048
#define MAX_32B_RANGE                   4095
#define MIN_64B_RANGE                   4096
#define MAX_64B_RANGE                   8191

using namespace std;

/// How many booleans do we have in a group for the glue variables.
/// We have 8!
const uint8_t BOOL_PER_GROUP(8);

/// Simple indexed masks to get or set bit values in the Modbus packed
/// structure where we use every bit in the byte.
const uint8_t BOOL_BIT_MASK[8] = {
        0x01,
        0x02,
        0x04,
        0x08,
        0x10,
        0x20,
        0x40,
        0x80,
};

inline void initialize_mapped_from_group(uint16_t msi,
                                         const GlueBoolGroup* group,
                                         vector<MappedBool>& buffer) {
    auto start_index = msi * BOOL_PER_GROUP;
    for (size_t bool_index = 0; bool_index < BOOL_PER_GROUP; ++bool_index) {
        auto mapped_index = start_index + bool_index;
        if (group->values[bool_index]) {
            buffer[mapped_index].value = group->values[bool_index];
            buffer[mapped_index].cached_value = *group->values[bool_index];
        }
    }
}

IndexedStrategy::IndexedStrategy(const GlueVariablesBinding& bindings) :
    glue_mutex(bindings.buffer_lock)
 {
    lock_guard<mutex> guard(*this->glue_mutex);
    // This constructor is pretty long - what we are doing here is
    // setting up structures that map between caches and the bound
    // glue. These structures give fast (index -based) read/write of
    // values. The caches ensure that we are unlikely to have to wait
    // for a lock.

    // Allocate a big enough read and write buffers. For the "int" types,
    // we know the size in advance. For the boolean types, we don't, so
    // figure out how big of a buffer we need for boolean types.
    const int32_t max_coil_index = bindings.find_max_msi(IECVT_BOOL, IECLDT_OUT);
    const int32_t max_di_index = bindings.find_max_msi(IECVT_BOOL, IECLDT_IN);

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
                initialize_mapped_from_group(msi, group, coil_read_buffer);
            } else if (dir == IECLDT_IN) {
                initialize_mapped_from_group(msi, group, di_read_buffer);
            }
        }
        else if (type == IECVT_INT) {
            if (dir == IECLDT_OUT) {
                int_register_read_buffer[msi].init(reinterpret_cast<IEC_INT*>(glue_variables[index].value));
            } else if (dir == IECLDT_MEM && msi >= MIN_16B_RANGE && msi < MAX_16B_RANGE) {
                intm_register_read_buffer[msi - MIN_16B_RANGE].init(reinterpret_cast<IEC_INT*>(glue_variables[index].value));
            } else if (dir == IECLDT_IN) {
                int_input_read_buffer[msi].init(reinterpret_cast<IEC_INT*>(glue_variables[index].value));
            }
        }
        else if (type == IECVT_DINT && dir == IECLDT_MEM && msi <= MIN_32B_RANGE && msi < MAX_32B_RANGE) {
            dintm_register_read_buffer[msi - MIN_32B_RANGE].init(reinterpret_cast<IEC_DINT*>(glue_variables[index].value));
        }
        else if (type == IECVT_LINT && dir == IECLDT_MEM && msi <= MIN_64B_RANGE && msi < MAX_64B_RANGE) {
            lintm_register_read_buffer[msi - MIN_64B_RANGE].init(reinterpret_cast<IEC_LINT*>(glue_variables[index].value));
        }
    }
}

/// Exchange values between the read and write buffers. This would normally
/// be called periodically so that we transfer values between the cache
/// that we maintain here and the glue variables that the runtime uses.
/// @param write_buffer The write buffer we are exchanging with that contains
/// writes that have not yet be flushed to the glue.
/// @param read_buffer The read buffer that contains the glue variable
/// and a local cache of the value.
template <typename T>
void exchange(array<PendingValue<T>, NUM_REGISTER_VALUES>& write_buffer,
              array<MappedValue<T>, NUM_REGISTER_VALUES>& read_buffer) {
    for (size_t index = 0; index < write_buffer.size(); ++index) {
        // Skip any index that is not mapped to a located variable.
        if (!read_buffer[index].value) {
            continue;
        }

        // If there was a write that hasn't be written, then transfer the
        // value to the read buffer.
        if (write_buffer[index].has_pending) {
            *read_buffer[index].value = write_buffer[index].value;
        }

        // Finally, update our cached value so that we can read the value
        // without waiting.
        read_buffer[index].cached_value = *read_buffer[index].value;
    }
}

void IndexedStrategy::Exchange() {
    lock_guard<mutex> guard(*this->glue_mutex);

    // Since we already figured out the mapping in an efficient structure
    // the process of exchange is simply going through the items. We first
    // handle populating writes into the structure.

    // Update the read caches for coils and discrete inputs.
    // Only the coils can be set, so we only only to check for pending
    // writes to those.
    for (size_t index = 0; index < coil_write_buffer.size(); ++index) {
        if (coil_write_buffer[index].has_pending) {
            *coil_read_buffer[index].value = coil_write_buffer[index].value;
        }
        coil_read_buffer[index].update_cache();
    }

    // Update the boolean values.
    for (size_t index = 0; index < di_read_buffer.size(); ++index) {
        di_read_buffer[index].update_cache();
    }

    exchange(int_register_write_buffer, int_register_read_buffer);
    exchange(intm_register_write_buffer, intm_register_read_buffer);
    exchange(dintm_register_write_buffer, dintm_register_read_buffer);
    exchange(lintm_register_write_buffer, lintm_register_read_buffer);

    for (size_t index = 0; index < int_input_read_buffer.size(); ++index) {
        int_input_read_buffer[index].update_cache();
    }
}

modbus_errno IndexedStrategy::WriteCoil(uint16_t coil_index, bool value) {
    lock_guard<mutex> guard(buffer_mutex);
    if (coil_index < coil_write_buffer.size()
        && coil_read_buffer[coil_index].value) {
        coil_write_buffer[coil_index].set(value);
        return 0;
    }
    return -1;
}

modbus_errno IndexedStrategy::WriteMultipleCoils(uint16_t coil_start_index,
                                                 uint16_t num_coils,
                                                 uint8_t* values) {
    if (coil_start_index + num_coils >= coil_write_buffer.size()) {
        return -1;
    }

    lock_guard<mutex> guard(buffer_mutex);
    for (uint16_t index = 0; index < num_coils; ++index) {
        // Get the value from the packed structure
        bool value = values[index / 8] & BOOL_BIT_MASK[index % 8];
        coil_write_buffer[coil_start_index + index].set(value);
    }

    return 0;
}

modbus_errno IndexedStrategy::ReadCoils(uint16_t coil_start_index,
                                        uint16_t num_values,
                                        uint8_t* values) {
    return this->ReadBools(coil_read_buffer, coil_start_index, num_values, values);
}

modbus_errno IndexedStrategy::ReadDiscreteInputs(uint16_t di_start_index,
                                                 uint16_t num_values,
                                                 uint8_t* values) {
    return this->ReadBools(di_read_buffer, di_start_index, num_values, values);
}

modbus_errno IndexedStrategy::ReadBools(const vector<MappedBool>& buffer,
                                        uint16_t start_index,
                                        uint16_t num_values,
                                        uint8_t* values) {
   auto max_index = start_index + num_values - 1;
    if (max_index >= buffer.size()) {
        return -1;
    }

    // Start by filling with 0 - we know the buffer is bigger than this can be
    memset(values, 0, num_values / 8 + 1);

    lock_guard<mutex> guard(buffer_mutex);
    for (uint16_t index = 0; index < num_values; ++index) {
        values[index / 8] |= buffer[start_index + index].cached_value ? BOOL_BIT_MASK[index % 8] : 0;
    }

    return 0;
}

modbus_errno IndexedStrategy::WriteHoldingRegisters(uint16_t hr_start_index,
                                                    uint16_t num_registers,
                                                    uint8_t* value) {
    // Each holding register is 2 bytes. Depending on the destination value
    // we are either updating the entire value or part of the value.
    // Here we go through each index and update the appropriate part of the value.
    lock_guard<mutex> guard(buffer_mutex);

    for (uint16_t index = 0; index < num_registers; ++index) {
        uint16_t hr_index = hr_start_index + index;
        uint16_t word = mb_to_word(value[0], value[1]);

        if (hr_index < MIN_16B_RANGE) {
            int_register_write_buffer[hr_index].set(word);
        } else if (hr_index < MAX_16B_RANGE) {
            hr_index -= MIN_16B_RANGE;
            intm_register_write_buffer[hr_index].set(word);
        } else if (hr_index < MAX_32B_RANGE) {
            hr_index -= MIN_32B_RANGE;
            // The word we got is part of a larger 32-bit value, and we will
            // bit shift to write the appropriate part. Resize to 32-bits
            // so we can shift appropriately.
            uint32_t partial_value = (uint32_t) word;
            PendingValue<IEC_DINT>& dst = dintm_register_write_buffer[hr_index / 2];
            dst.has_pending = true;

            if (hr_index % 2 == 0) {
                // First word
                dst.value = dst.value & 0x0000ffff;
                dst.value = dst.value | partial_value;
            } else {
                // Second word
                dst.value = dst.value & 0xffff0000;
                dst.value = dst.value | partial_value;
            }
        } else if (hr_index < MAX_64B_RANGE) {
            hr_index -= MIN_64B_RANGE;
            // Same as with a 32-bit value, here we are updating part of a
            // 64-bit value, so resize so we can bit-shift appropriately.
            uint64_t partial_value = (uint64_t) word;
            PendingValue<IEC_LINT>& dst = lintm_register_write_buffer[hr_index / 4];
            dst.has_pending = true;

            auto word_index = hr_index % 4;
            switch (word_index) {
                case 0:
                    // First word
                    dst.value = dst.value & 0x0000ffffffffffff;
                    dst.value = dst.value | (partial_value << 48);
                    break;
                case 1:
                    // Second word
                    dst.value = dst.value & 0xffff0000ffffffff;
                    dst.value = dst.value | (partial_value << 32);
                    break;
                case 2:
                    // Third word
                    dst.value = dst.value & 0xffffffff0000ffff;
                    dst.value = dst.value | (partial_value << 16);
                    break;
                case 3:
                    // Fourth word
                    dst.value = dst.value & 0xffffffffffff0000;
                    dst.value = dst.value | partial_value;
                    break;
            }
        } else {
            return -1;
        }

        // Move our buffer pointer so that we will handle the next register
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

        if (hr_index >= MIN_16B_RANGE) {
            return -1;
        }

        uint16_t val = int_input_read_buffer[hr_index].cached_value;

        value[0] = mb_high_byte(val);
        value[1] = mb_low_byte(val);
        // Move to the next 16-bit position
        value += 2;
    }

    return 0;
}

/** @}*/
