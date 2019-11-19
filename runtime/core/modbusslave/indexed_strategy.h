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

#ifndef CORE_MODBUSSLAVE_INDEXED_STRATEGY_H_
#define CORE_MODBUSSLAVE_INDEXED_STRATEGY_H_

#include <mutex>
#include <vector>
#include "lib/iec_types_all.h"

/** \addtogroup openplc_runtime
 *  @{
 */

class GlueVariablesBinding;

const std::uint16_t NUM_REGISTER_VALUES(1024);

/// Defines the mapping between a located boolean value
/// and a cache of the value for reading with modbus.
struct MappedBool {
  MappedBool() : cached_value(0), value(nullptr) {}
  IEC_BOOL cached_value;
  IEC_BOOL *value;

  inline void update_cache() {
    if (this->value) {
      this->cached_value = *this->value;
    }
  }
};

/// Defines a write that has been submitted via Modbus
/// but may not have been applied to the located variable yet.
struct PendingBool {
  PendingBool() : has_pending(false), value(0) {}
  bool has_pending;
  IEC_BOOL value;

  /// Set the value and mark it as updated.
  inline void set(IEC_BOOL val) {
    this->has_pending = true;
    this->value = val;
  }
};

/// Defines the mapping between a located value
/// and a cache of the value for reading with modbus.
template <typename T>
struct MappedValue {
  MappedValue() : cached_value(0), value(nullptr) {}
  T cached_value;
  T* value;

  /// Initialize the glue link and the cached value.
  /// @param val The glue variable to initialize from.
  inline void init(T* val) {
    this->value = val;
    this->cached_value = *val;
  }

  inline void update_cache() {
    if (this->value) {
      this->cached_value = *this->value;
    }
  }
};

/// Defines a write that has been submitted via Modbus
/// but may not have been applied to the located variable yet.
template <typename T>
struct PendingValue {
  PendingValue() : has_pending(false), value(0) {}
  bool has_pending;
  T value;

  /// Set the value and mark it as updated.
  inline void set(T val) {
    this->has_pending = true;
    this->value = val;
  }
};

typedef std::uint8_t modbus_errno;

/// Implements a strategy that maps between modbus and located variables
/// based on the data type and the index.
/// For performance reasons, we determine the mapping when this is
/// constructed so that we can quickly apply changes. This obviously uses
/// more memory, but is far more efficient over the lifetime of the
/// application.
class IndexedStrategy {
  public:
    /// Initialize a new instance of the strategy using the bindings.
    IndexedStrategy(const GlueVariablesBinding& bindings);

    /// Exchange data between the cache and the runtime.
    void Exchange();

    /// Write a coil value into the cache.
    /// @param coil_index The index of the void to write
    /// @param value The value to write
    modbus_errno WriteCoil(std::uint16_t coil_index, bool value);

    /// Write multiple coils, where the bits are packed into bytes
    /// @param coil_index The index of the first coil to write.
    /// @param num_coils The number of coils that have values in the values array.
    /// @param values Byte-packed values.
    modbus_errno WriteMultipleCoils(std::uint16_t coil_index,
                                    std::uint16_t num_coils,
                                    std::uint8_t* values);

    /// Read the specified number of coils into the data buffer. Coils are
    /// read such that they are packed into the destination array, 8 values
    /// per 1-byte of data.
    /// @param coil_start_index The index of the first coil to read
    /// @param num_value The number of coils to read.
    /// @param values The bytes to pack the coils into.
    modbus_errno ReadCoils(std::uint16_t coil_start_index,
                           std::uint16_t num_values,
                           std::uint8_t* values);

    /// Read the specified number of discrete inputs into the data buffer.
    /// Inputs are read such that they are packed into the destination array,
    /// 8 values per 1-byte of data.
    modbus_errno ReadDiscreteInputs(std::uint16_t coil_start_index,
                                    std::uint16_t num_coils,
                                    std::uint8_t* values);
   
    /// Write the values from holding resisters into the cache.
    /// @param hr_start_index Index of the first holding register that
    /// was written.
    /// @param num_registers Number of register to write - 2 bytes per item.
    /// @param value An array of bytes to read from.
    modbus_errno WriteHoldingRegisters(std::uint16_t hr_start_index,
                                       std::uint16_t num_registers,
                                       std::uint8_t* value);

    /// Read the holding registers into the buffer.
    /// @param hr_start_index Index of the first holding register that
    /// was written.
    /// @param num_registers Number of register to write - 2 bytes per item.
    /// @param value An array of bytes to write into.
    modbus_errno ReadHoldingRegisters(std::uint16_t hr_start_index,
                                      std::uint16_t num_registers,
                                      std::uint8_t* value);

    /// Read the input registers into the buffer.
    /// @param hr_start_index Index of the first holding register that
    /// was written.
    /// @param num_registers Number of register to write - 2 bytes per item.
    /// @param value An array of bytes to write into.
    modbus_errno ReadInputRegisters(std::uint16_t hr_start_index,
                                    std::uint16_t num_registers,
                                    std::uint8_t* value);

  private:
    /// Read the boolean values from the mapped structure into the values.
    modbus_errno ReadBools(const std::vector<MappedBool>& buffer,
                           std::uint16_t coil_start_index,
                           std::uint16_t num_values,
                           std::uint8_t* values);

  private:
    std::vector<MappedBool> coil_read_buffer;
    std::vector<PendingBool> coil_write_buffer;
    std::vector<MappedBool> di_read_buffer;

    std::array<PendingValue<IEC_INT>, NUM_REGISTER_VALUES> int_register_write_buffer;
    std::array<MappedValue<IEC_INT>, NUM_REGISTER_VALUES> int_register_read_buffer;

    std::array<PendingValue<IEC_INT>, NUM_REGISTER_VALUES> intm_register_write_buffer;
    std::array<MappedValue<IEC_INT>, NUM_REGISTER_VALUES> intm_register_read_buffer;

    std::array<PendingValue<IEC_DINT>, NUM_REGISTER_VALUES> dintm_register_write_buffer;
    std::array<MappedValue<IEC_DINT>, NUM_REGISTER_VALUES> dintm_register_read_buffer;

    std::array<PendingValue<IEC_LINT>, NUM_REGISTER_VALUES> lintm_register_write_buffer;
    std::array<MappedValue<IEC_LINT>, NUM_REGISTER_VALUES> lintm_register_read_buffer;

    std::array<MappedValue<IEC_INT>, NUM_REGISTER_VALUES> int_input_read_buffer;

    // Protects access to the cached values in this class.
    std::mutex buffer_mutex;
    std::mutex* glue_mutex;
};

/** @}*/

#endif  // CORE_MODBUSSLAVE_INDEXED_STRATEGY_H_
