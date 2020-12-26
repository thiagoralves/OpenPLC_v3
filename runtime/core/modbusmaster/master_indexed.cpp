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
#include <array>
#include <chrono>
#include <mutex>
#include <thread>
#ifndef OPLC_UNIT_TEST
#include <modbus.h>
#endif  // OPLC_UNIT_TEST
#include <spdlog/spdlog.h>
#include <ini.h>

#include "buffer.h"
#include "ini_util.h"
#include "master_indexed.h"
#include "master.h"

using namespace std;
using namespace oplc::modbusm;

/** \addtogroup openplc_runtime
 *  @{
 */

#define MAX_MB_IO           400

const uint8_t MAPPED_GLUE_START = 100;

oplc::ContiguousMappedValues<IEC_BOOL, MAX_MB_IO> bool_input_buf;
/// Boolean values that will be written to the remote server.
oplc::ContiguousMappedValues<IEC_BOOL, MAX_MB_IO> bool_output_buf;

/// Values that will be written to the remote server.
oplc::ContiguousMappedValues<uint16_t, MAX_MB_IO> int_output_buf;
/// Values that are read from the remote server to located variables.
oplc::ContiguousMappedValues<uint16_t, MAX_MB_IO> int_input_buf;

/// If set, then when there are errors, increment this value.
const GlueVariable* error_var = nullptr;

mutex io_lock;

/// @brief This function is called by the OpenPLC in a loop. Here the internal
/// buffers must be updated to reflect the actual Input state.
void oplc::modbusm::modbus_master_before_cycle()
{
    lock_guard<mutex> guard(io_lock);
    bool_input_buf.update_located();
    int_input_buf.update_located();
}


/// @brief This function is called by the OpenPLC in a loop. Here the internal buffers
/// must be updated to reflect the actual Output state.
void oplc::modbusm::modbus_master_after_cycle()
{
    lock_guard<mutex> guard(io_lock);
    bool_output_buf.update_cache();
    int_output_buf.update_cache();
}

/// @brief Configuration information that is supplied to the config handler.
struct IndexedMasterConfig
{
    array<MasterConfig, MODBUS_MASTER_MAX>* masters;
    array<IndexedMapping, MODBUS_MASTER_MAX>* mappings;
};

/// @brief Callback function for the ini parser. This function is called for
/// every configuration item.
int cfg_handler(void* user_data, const char* section,
                const char* name, const char* value)
{
    if (strcmp("modbusmaster", section) != 0)
    {
        return 0;
    }

    auto config = reinterpret_cast<IndexedMasterConfig*>(user_data);
    auto mappings = config->mappings;

    const uint8_t max = MODBUS_MASTER_MAX - 1; 
    uint8_t index;
    // First check if this is a common item.
    if (oplc::modbusm::common_cfg_handler(*config->masters, name, value) == 0)
    {
        // It was already handled
    }
    else if (oplc::strcmp_with_index(name, "discrete_inputs_start", max, &index) == 0)
    {
        mappings->at(index).discrete_inputs.start_address = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "discrete_inputs_size", max, &index) == 0)
    {
        mappings->at(index).discrete_inputs.num_regs = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "coils_start", max, &index) == 0)
    {
        mappings->at(index).coils.start_address = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "coils_size", max, &index) == 0)
    {
        mappings->at(index).coils.num_regs = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "input_registers_start", max, &index) == 0)
    {
        mappings->at(index).input_registers.start_address = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "input_registers_size", max, &index) == 0)
    {
        mappings->at(index).input_registers.num_regs = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "holding_registers_read_start", max, &index) == 0)
    {
        mappings->at(index).holding_read_registers.start_address = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "holding_registers_read_size", max, &index) == 0)
    {
        mappings->at(index).holding_read_registers.num_regs = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "holding_registers_start", max, &index) == 0)
    {
        mappings->at(index).holding_registers.start_address = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "holding_registers_size", max, &index) == 0)
    {
        mappings->at(index).holding_registers.num_regs = atoi(value);
    }
    else if (strcmp(name, "enabled") == 0)
    {
        // Nothing to do here - we already know this is enabled
    }
    else
    {
        spdlog::warn("Unknown configuration item {}", name);
        return -1;
    }

    return 0;
}

/// Arguments provided to the master polling thread.
struct MasterArgs
{
    volatile bool* run;
    const MasterConfig* master;
    const IndexedMapping* mapping;
};

#define MODBUS_HANDLE_ERROR(ret, type_name) \
    if (ret == -1)\
    {\
        modbus_master_handle_error(master, mb_ctx, is_connected, type_name);\
    }

#define IF_NOT_MODBUS_ERROR(ret, type_name) \
    if (ret == -1)\
    {\
        modbus_master_handle_error(master, mb_ctx, is_connected, type_name);\
    }\
    else

void modbus_master_handle_error(const MasterConfig* master, modbus_t* mb_ctx,
                                bool& is_connected, const char* type_name)
{
    if (master->protocol == ProtocolTcp)
    {
        modbus_close(mb_ctx);
        is_connected = false;
    }

    spdlog::info("Modbus {} failed on MB device {}: {}",
                 type_name, master->name, modbus_strerror(errno));

    // TODO accessing this value is not property protected - this is old
    // and it should be fixed
    if (error_var != nullptr)
    {
        *reinterpret_cast<IEC_LINT*>(error_var->value) += 1;
    }
}

void* oplc::modbusm::modbus_master_indexed_poll(void* args)
{
    auto master_args = reinterpret_cast<MasterArgs*>(args);
    auto master = master_args->master;
    auto mapping = reinterpret_cast<const IndexedMapping*>(master_args->mapping);

    // The context for communcating with the device.
    modbus_t* mb_ctx = master->create();
    bool is_connected = false;
    // Declare a common buffer for all read operations. We read into this
    // buffer so that we can minimize the exclusive lock time that we need
    // to exchange data with the main loop.
    // This is bigger than the biggest segment we allow.
    // TODO validate that the bounds provided are smaller than this buffer.
    uint8_t rw_buffer[MAX_MB_IO * 2];

    // The number of consecutive failed attempts
    int32_t failed_attempts = 0;

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = (1000*1000*1000*28L) / master->rtu_baud_rate;

    while (*master_args->run)
    {
        auto start = chrono::steady_clock::now();

        // Verify if device is connected
        if (!is_connected)
        {
            spdlog::debug("Device {} is disconnected. Reconnecting.", master->name);
            if (modbus_connect(mb_ctx) == -1)
            {
                spdlog::error("Connection failed on MB device {}: {}", master->name, modbus_strerror(errno));
                
                // TODO accessing this value is not property protected - this is old
                // and it should be fixe
                if (error_var != nullptr)
                {
                    *reinterpret_cast<IEC_LINT*>(error_var->value) += 1;
                }
                failed_attempts += 1;
            }
            else
            {
                spdlog::info("Connected to MB device {}", master->name);
                is_connected = true;
                failed_attempts = 0;
            }
        }

        if (is_connected)
        {
            // Read discrete inputs
            if (mapping->discrete_inputs.num_regs != 0)
            {
                nanosleep(&ts, nullptr); 
                int result = modbus_read_input_bits(mb_ctx, mapping->discrete_inputs.start_address,
                                                    mapping->discrete_inputs.num_regs, rw_buffer);
                IF_NOT_MODBUS_ERROR(result, "read discrete input")
                {
                    lock_guard<mutex> guard(io_lock);
                    bool_input_buf.copy_to_cache(rw_buffer, mapping->discrete_inputs.num_regs, mapping->discrete_inputs.buffer_offset);
                }
            }

            // Write coils
            if (mapping->coils.num_regs != 0)
            {
                {
                    lock_guard<mutex> guard(io_lock);
                    bool_output_buf.copy_from_cache(rw_buffer, mapping->coils.buffer_offset, mapping->coils.num_regs);
                }

                nanosleep(&ts, NULL); 
                int result = modbus_write_bits(mb_ctx, mapping->coils.start_address, mapping->coils.num_regs, rw_buffer);
                MODBUS_HANDLE_ERROR(result, "write coils");
            }
           

            // Read input registers
            if (mapping->input_registers.num_regs != 0)
            {
                auto rw16_buffer = reinterpret_cast<uint16_t*>(rw_buffer);
                nanosleep(&ts, NULL); 
                int result = modbus_read_input_registers(mb_ctx, mapping->input_registers.start_address,
                                                         mapping->input_registers.num_regs, rw16_buffer);
                IF_NOT_MODBUS_ERROR(result, "read input registers")
                {
                    lock_guard<mutex> guard(io_lock);
                    int_input_buf.copy_to_cache(rw_buffer, result * 2, mapping->input_registers.buffer_offset);
                }
            }

            // Read holding registers
            if (mapping->holding_read_registers.num_regs != 0)
            {
                auto rw16_buffer = reinterpret_cast<uint16_t*>(rw_buffer);
                nanosleep(&ts, NULL); 
                int result = modbus_read_registers(mb_ctx, mapping->holding_read_registers.start_address,
                                                        mapping->holding_read_registers.num_regs, rw16_buffer);
                IF_NOT_MODBUS_ERROR(result, "read holding registers")
                {
                    lock_guard<mutex> guard(io_lock);
                    int_input_buf.copy_to_cache(rw_buffer, result * 2, mapping->holding_read_registers.buffer_offset);
                }
            }

            // Write holding registers
            if (mapping->holding_registers.num_regs != 0)
            {
                auto rw16_buffer = reinterpret_cast<uint16_t*>(rw_buffer);
                {
                    lock_guard<mutex> guard(io_lock);
                    int_output_buf.copy_from_cache(rw_buffer, mapping->holding_registers.buffer_offset, mapping->holding_registers.num_regs);
                }

                nanosleep(&ts, NULL); 
                int return_val = modbus_write_registers(mb_ctx, mapping->holding_registers.start_address,
                                                        mapping->holding_registers.num_regs, rw16_buffer);
                MODBUS_HANDLE_ERROR(return_val, "write holding registers");
            }
        }

        if (failed_attempts && master->backoff_strategy == BackoffStrategyLinear)
        {
            auto scale_factor = std::min(10, failed_attempts);
            this_thread::sleep_until(start + (master->poll_interval * scale_factor));
        }
        else
        {
            this_thread::sleep_until(start + master->poll_interval);
        }
    }

    modbus_free(mb_ctx);

    return 0;
}

/// Assign the mappings from our bindings according to the strategy
/// that this master expect. In general, we map items starting from
/// 100 to the modbus master.
void assign_mappings(const GlueVariablesBinding& bindings)
{
    // Initialize comm error counter. For historical reasons,
    // this is defined to be at index 1026 in the IEC_LINT with
    // the memory storage class.
    error_var = bindings.find(IECLDT_MEM, IECLST_LONGWORD, 1026, 0);
    if (error_var != nullptr)
    {
        *reinterpret_cast<IEC_LINT*>(error_var->value) = 0;
    }

    // Assign the buffered mapping values using the glue variables
    for (auto i = 0; i < bindings.size; ++i)
    {
        auto glue_var = bindings.glue_variables[i];
        // We defined that modbus master mappings start at 100, so ignore
        // anything that is not in the right range.
        if (glue_var.msi < MAPPED_GLUE_START
            || glue_var.msi >= MAPPED_GLUE_START + MAX_MB_IO)
        {
            continue;
        }

        if (glue_var.dir == IECLDT_OUT && glue_var.size == IECLST_WORD)
        {
            int_output_buf.assign(glue_var.msi - MAPPED_GLUE_START, 
                reinterpret_cast<IEC_UINT*>(glue_var.value));
        }
        else if (glue_var.dir == IECLDT_IN && glue_var.size == IECLST_WORD)
        {
            int_input_buf.assign(glue_var.msi - MAPPED_GLUE_START,
                reinterpret_cast<IEC_UINT*>(glue_var.value));
        }
        else if (glue_var.dir == IECLDT_OUT && glue_var.size == IECLST_BIT)
        {
            auto group = reinterpret_cast<GlueBoolGroup*>(glue_var.value);
            // We assign the index, one by one, into the mapping
            for (auto i = 0; i < 8; ++i)
            {
                auto offset = glue_var.msi - MAPPED_GLUE_START;
                bool_input_buf.assign(offset * 8 + i, group->values[i]);
            }
        }
        else if (glue_var.dir == IECLDT_IN && glue_var.size == IECLST_BIT)
        {
            auto group = reinterpret_cast<GlueBoolGroup*>(glue_var.value);
            // We assign the index, one by one, into the mapping
            for (auto i = 0; i < 8; ++i)
            {
                auto offset = glue_var.msi - MAPPED_GLUE_START;
                bool_output_buf.assign(offset * 8 + i, group->values[i]);
            }
        }
    }
}

/// Run the modbus master. This function does not return until
/// this service is terminated.
void modbus_master_run(oplc::config_stream& cfg_stream,
                       const char* cfg_overrides,
                       const GlueVariablesBinding& bindings,
                       volatile bool& run)
{
    // Read the configuration information for the masters
    array<MasterConfig, MODBUS_MASTER_MAX> master_defs;
    array<IndexedMapping, MODBUS_MASTER_MAX> mapping_defs;
    array<std::thread, MODBUS_MASTER_MAX> threads; 
    IndexedMasterConfig config
    {
        .masters = &master_defs,
        .mappings = &mapping_defs,
    };
    ini_parse_stream(oplc::istream_fgets, cfg_stream.get(),
                     cfg_handler, &config);
    cfg_stream.reset(nullptr);

    assign_mappings(bindings);

    // Create a thread for polling each master. We use multiple threads
    // so that communication timeouts related to one master will not
    // impact others.
    for (size_t index = 0; index < master_defs.size(); ++index)
    {
        if (index != 0)
        {
            // Initialize the mapping offset from the prior master definition.
            // This sets things so that each master knows which part of the
            // exchange that it will write to.
            mapping_defs[index].set_offsets(mapping_defs[index - 1]);
        }

        // This might happen if it is disabled or there are gaps in the
        // the indices for the declared devices.
        if (master_defs[index].protocol == ProtocolInvalid)
        {
            continue;
        }

        auto master_args = new MasterArgs
        {
            .run = &run,
            .master = &master_defs[index],
            .mapping = &mapping_defs[index],
        };

        try {
            threads[index] = std::thread(oplc::modbusm::modbus_master_indexed_poll, 
                                     master_args);
            threads[index].detach(); //?
        } 
        catch (const std::system_error& ecvt) 
        {
            delete master_args;
        }
    }

    while (run)
    {
        // Sleep for a while to determine if we should terminate
        // A better approach is targeted as a future story
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    // Terminate the unified polling thread. It is important to wait
    // here because we passed information to the modbus thread that is
    // on this stack.
    for (size_t index = 0; index < threads.size(); ++index)
    {
        if (threads[index].joinable())
        {
            threads[index].join();
        }
    }
}

void oplc::modbusm::modbus_master_service_run(const GlueVariablesBinding& binding,
                                              volatile bool& run,
                                              const char* config)
{
    auto cfg_stream = oplc::open_config();
    modbus_master_run(cfg_stream, config, binding, run);
}


/** @}*/
