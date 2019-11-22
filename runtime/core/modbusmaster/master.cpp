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

#include <ini.h>
#include <spdlog/spdlog.h>
#include <modbus.h>

#include <cstdint>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

#include "ladder.h"
#include "master.h"
#include "../glue.h"
#include "../ini_util.h"

using namespace std;

/** \addtogroup openplc_runtime
 *  @{
 */

#define MAX_MB_IO           400

uint8_t bool_input_buf[MAX_MB_IO];
uint8_t bool_output_buf[MAX_MB_IO];
uint16_t int_input_buf[MAX_MB_IO];
uint16_t int_output_buf[MAX_MB_IO];

pthread_mutex_t ioLock;

/// \brief This function is called by the OpenPLC in a loop. Here the internal
/// buffers must be updated to reflect the actual Input state.
void updateBuffersIn_MB()
{
    pthread_mutex_lock(&ioLock);

    for (int i = 0; i < MAX_MB_IO; i++)
    {
        if (bool_input[100+(i/8)][i%8] != NULL) *bool_input[100+(i/8)][i%8] = bool_input_buf[i];
        if (int_input[100+i] != NULL) *int_input[100+i] = int_input_buf[i];
    }

    pthread_mutex_unlock(&ioLock);
}


/// \brief This function is called by the OpenPLC in a loop. Here the internal buffers
/// must be updated to reflect the actual Output state.
void updateBuffersOut_MB()
{
    pthread_mutex_lock(&ioLock);

    for (int i = 0; i < MAX_MB_IO; i++)
    {
        if (bool_output[100+(i/8)][i%8] != NULL) bool_output_buf[i] = *bool_output[100+(i/8)][i%8];
        if (int_output[100+i] != NULL) int_output_buf[i] = *int_output[100+i];
    }

    pthread_mutex_unlock(&ioLock);
}

enum MasterProtocol {
    ProtocolInvalid,
    ProtocolTcp,
    ProtocolRtu,
};

const uint8_t MASTER_ITEM_SIZE(100);

struct ModbusAddress
{
    uint16_t start_address;
    uint16_t num_regs;
};

/// Defines the configuration information for a particular modbus
/// master.
struct Master {
    /// A name, mostly for the purpose of logging.
    char name[MASTER_ITEM_SIZE];
    /// Which protocol do we use for communication.
    MasterProtocol protocol;
    /// The ID of the slave.
    uint8_t slave_id;
    /// The IP address (if using TCP).
    char ip_address[MASTER_ITEM_SIZE];
    /// The port (if using TCP).
    uint16_t ip_port;
    uint16_t rtu_baud_rate;
    uint8_t rtu_parity;
    uint16_t rtu_data_bit;
    uint16_t rtu_stop_bit;  
    modbus_t* mb_ctx;
    uint16_t timeout;
    bool is_connected;

    struct ModbusAddress discrete_inputs;
    struct ModbusAddress coils;
    struct ModbusAddress input_registers;
    struct ModbusAddress holding_read_registers;
    struct ModbusAddress holding_registers;

    Master() :
        name{'\0'},
        protocol(ProtocolInvalid),
        ip_address{'\0'},
        mb_ctx(nullptr),
        is_connected(false)
    {}

    void create() {
        if (protocol == ProtocolTcp) {
            mb_ctx = modbus_new_tcp(ip_address, ip_port);
        } else if (protocol == ProtocolRtu) {
            mb_ctx = modbus_new_rtu(ip_address, rtu_baud_rate, rtu_parity, rtu_data_bit, rtu_stop_bit);
        }

        modbus_set_slave(mb_ctx, slave_id);

        uint32_t to_sec = timeout / 1000;
        uint32_t to_usec = (timeout % 1000) * 1000;
        modbus_set_response_timeout(mb_ctx, to_sec, to_usec);
    }
};

struct ModbusMasterConfig {

    chrono::milliseconds polling_period;
    vector<Master>* masters;

    Master* config_item(uint8_t index) {
        size_t required_size = max(masters->size(), static_cast<size_t>(index + 1));
        if (masters->size() < required_size) {
            masters->resize(index + 1);
        }
        return &masters->at(index);
    }
};

int modbus_master_cfg_handler(void* user_data, const char* section,
                            const char* name, const char* value) {
    if (strcmp("modbusmaster", section) != 0) {
        return 0;
    }

    auto config = reinterpret_cast<ModbusMasterConfig*>(user_data);

    uint8_t index;
    if (oplc::cmpnameid(name, "name", &index) == 0) {
        strncpy(config->config_item(index)->name, value, MASTER_ITEM_SIZE);
        config->config_item(index)->name[MASTER_ITEM_SIZE - 1] = '\0';
    } else if (oplc::cmpnameid(name, "protocol", &index) == 0) {
        if (strcmp(value, "tcp") == 0) {
            config->config_item(index)->protocol = ProtocolTcp;
        } else if (strcmp(value, "rtu") == 0) {
            config->config_item(index)->protocol = ProtocolRtu;
        }
    } else if (oplc::cmpnameid(name, "slave_id", &index) == 0) {
        config->config_item(index)->slave_id = atoi(value);
    } else if (oplc::cmpnameid(name, "ip_address", &index) == 0) {
        strncpy(config->config_item(index)->ip_address, value, MASTER_ITEM_SIZE);
        config->config_item(index)->ip_address[MASTER_ITEM_SIZE - 1] = '\0';
    } else if (oplc::cmpnameid(name, "ip_port", &index) == 0) {
        config->config_item(index)->ip_port = atoi(value);
    } else if (oplc::cmpnameid(name, "rtu_baud_rate", &index) == 0) {
        config->config_item(index)->rtu_baud_rate = atoi(value);
    } else if (oplc::cmpnameid(name, "rtu_parity", &index) == 0) {
        config->config_item(index)->rtu_parity = atoi(value);
    } else if (oplc::cmpnameid(name, "rtu_data_bit", &index) == 0) {
        config->config_item(index)->rtu_data_bit = atoi(value);
    } else if (oplc::cmpnameid(name, "rtu_stop_bit", &index) == 0) {
        config->config_item(index)->rtu_stop_bit = atoi(value);
    } else if (oplc::cmpnameid(name, "discrete_inputs_start", &index) == 0) {
        config->config_item(index)->discrete_inputs.start_address = atoi(value);
    } else if (oplc::cmpnameid(name, "discrete_inputs_size", &index) == 0) {
        config->config_item(index)->discrete_inputs.num_regs = atoi(value);
    } else if (oplc::cmpnameid(name, "coils_start", &index) == 0) {
        config->config_item(index)->coils.start_address = atoi(value);
    } else if (oplc::cmpnameid(name, "coils_size", &index) == 0) {
        config->config_item(index)->coils.num_regs = atoi(value);
    } else if (oplc::cmpnameid(name, "input_registers_start", &index) == 0) {
        config->config_item(index)->input_registers.start_address = atoi(value);
    } else if (oplc::cmpnameid(name, "input_registers_size", &index) == 0) {
        config->config_item(index)->input_registers.num_regs = atoi(value);
    } else if (oplc::cmpnameid(name, "holding_registers_read_start", &index) == 0) {
        config->config_item(index)->holding_read_registers.start_address = atoi(value);
    } else if (oplc::cmpnameid(name, "holding_registers_read_size", &index) == 0) {
        config->config_item(index)->holding_read_registers.num_regs = atoi(value);
    } else if (oplc::cmpnameid(name, "holding_registers_start", &index) == 0) {
        config->config_item(index)->holding_registers.start_address = atoi(value);
    } else if (oplc::cmpnameid(name, "holding_registers_size", &index) == 0) {
        config->config_item(index)->holding_registers.num_regs = atoi(value);
    } else if (strcmp(name, "enabled") == 0) {
        // Nothing to do here - we already know this is enabled
    } else {
        spdlog::warn("Unknown configuration item {}", name);
        return -1;
    }

    return 0;
}

struct MasterArgs {
    volatile bool* run;
    chrono::milliseconds polling_period;
    vector<Master>* masters;
};

void* modbus_master_poll_slaves(void* args) {
    auto master_args = reinterpret_cast<MasterArgs*>(args);

    while (*master_args->run) {
        uint16_t bool_input_index = 0;
        uint16_t bool_output_index = 0;
        uint16_t int_input_index = 0;
        uint16_t int_output_index = 0;

        for (size_t i = 0; i < master_args->masters->size(); i++) {
            Master& master = master_args->masters->at(i);
            //Verify if device is connected
            if (!master.is_connected) {
                spdlog::info("Device {} is disconnected. Attempting to reconnect...", master.name);
                if (modbus_connect(master.mb_ctx) == -1)
                {
                    spdlog::error("Connection failed on MB device {}: {}", master.name, modbus_strerror(errno));
                    
                    if (special_functions[2] != NULL) *special_functions[2]++;
                    
                    // Because this device is not connected, we skip those input registers
                    bool_input_index += (master.discrete_inputs.num_regs);
                    int_input_index += (master.input_registers.num_regs);
                    int_input_index += (master.holding_read_registers.num_regs);
                    bool_output_index += (master.coils.num_regs);
                    int_output_index += (master.holding_registers.num_regs);
                }
                else
                {
                    spdlog::info("Connected to MB device {}", master.name);
                    master.is_connected = true;
                }
            }

            if (master.is_connected)
            {
                struct timespec ts;
                ts.tv_sec = 0;
                ts.tv_nsec = (1000*1000*1000*28)/master.rtu_baud_rate;

                //Read discrete inputs
                if (master.discrete_inputs.num_regs != 0)
                {
                    uint8_t *tempBuff;
                    tempBuff = (uint8_t *)malloc(master.discrete_inputs.num_regs);
                    nanosleep(&ts, NULL); 
                    int return_val = modbus_read_input_bits(master.mb_ctx, master.discrete_inputs.start_address,
                                                            master.discrete_inputs.num_regs, tempBuff);
                    if (return_val == -1)
                    {
                        if (master.protocol == ProtocolTcp)
                        {
                            modbus_close(master.mb_ctx);
                            master.is_connected = false;
                        }
                        
                        spdlog::info("Modbus Read Discrete Input Registers failed on MB device {}: {}", master.name, modbus_strerror(errno));
                        bool_input_index += (master.discrete_inputs.num_regs);
                        if (special_functions[2] != NULL) *special_functions[2]++;
                    }
                    else
                    {
                        pthread_mutex_lock(&ioLock);
                        for (int j = 0; j < return_val; j++)
                        {
                            bool_input_buf[bool_input_index] = tempBuff[j];
                            bool_input_index++;
                        }
                        pthread_mutex_unlock(&ioLock);
                    }

                    free(tempBuff);
                }

                //Write coils
                if (master.coils.num_regs != 0)
                {
                    uint8_t *tempBuff;
                    tempBuff = (uint8_t *)malloc(master.coils.num_regs);

                    pthread_mutex_lock(&ioLock);
                    for (int j = 0; j < master.coils.num_regs; j++)
                    {
                        tempBuff[j] = bool_output_buf[bool_output_index];
                        bool_output_index++;
                    }
                    pthread_mutex_unlock(&ioLock);

                    nanosleep(&ts, NULL); 
                    int return_val = modbus_write_bits(master.mb_ctx, master.coils.start_address, master.coils.num_regs, tempBuff);
                    if (return_val == -1)
                    {
                        if (master.protocol == ProtocolTcp)
                        {
                            modbus_close(master.mb_ctx);
                            master.is_connected = false;
                        }

                        spdlog::error("Modbus Write Coils failed on MB device {}: {}", master.name, modbus_strerror(errno));
                        if (special_functions[2] != NULL) *special_functions[2]++;
                    }
                    
                    free(tempBuff);
                }

                //Read input registers
                if (master.input_registers.num_regs != 0)
                {
                    uint16_t *tempBuff;
                    tempBuff = (uint16_t *)malloc(2*master.input_registers.num_regs);
                    nanosleep(&ts, NULL); 
                    int return_val = modbus_read_input_registers(master.mb_ctx, master.input_registers.start_address,
                                                                master.input_registers.num_regs, tempBuff);
                    if (return_val == -1)
                    {
                        if (master.protocol == ProtocolTcp)
                        {
                            modbus_close(master.mb_ctx);
                            master.is_connected = false;
                        }
                        
                        spdlog::error("Modbus Read Input Registers failed on MB device {}: {}", master.name, modbus_strerror(errno));
                        int_input_index += (master.input_registers.num_regs);
                        if (special_functions[2] != NULL) *special_functions[2]++;
                    }
                    else
                    {
                        pthread_mutex_lock(&ioLock);
                        for (int j = 0; j < return_val; j++)
                        {
                            int_input_buf[int_input_index] = tempBuff[j];
                            int_input_index++;
                        }
                        pthread_mutex_unlock(&ioLock);
                    }

                    free(tempBuff);
                }

                //Read holding registers
                if (master.holding_read_registers.num_regs != 0)
                {
                    uint16_t *tempBuff;
                    tempBuff = (uint16_t *)malloc(2*master.holding_read_registers.num_regs);
                    nanosleep(&ts, NULL); 
                    int return_val = modbus_read_registers(master.mb_ctx, master.holding_read_registers.start_address,
                                                           master.holding_read_registers.num_regs, tempBuff);
                    if (return_val == -1)
                    {
                        if (master.protocol == ProtocolTcp)
                        {
                            modbus_close(master.mb_ctx);
                            master.is_connected = false;
                        }
                        spdlog::error("Modbus Read Holding Registers failed on MB device {}: {}", master.name, modbus_strerror(errno));
                        int_input_index += (master.holding_read_registers.num_regs);
                        if (special_functions[2] != NULL) *special_functions[2]++;
                    }
                    else
                    {
                        pthread_mutex_lock(&ioLock);
                        for (int j = 0; j < return_val; j++)
                        {
                            int_input_buf[int_input_index] = tempBuff[j];
                            int_input_index++;
                        }
                        pthread_mutex_unlock(&ioLock);
                    }

                    free(tempBuff);
                }

                //Write holding registers
                if (master.holding_registers.num_regs != 0)
                {
                    uint16_t *tempBuff;
                    tempBuff = (uint16_t *)malloc(2*master.holding_registers.num_regs);

                    pthread_mutex_lock(&ioLock);
                    for (int j = 0; j < master.holding_registers.num_regs; j++)
                    {
                        tempBuff[j] = int_output_buf[int_output_index];
                        int_output_index++;
                    }
                    pthread_mutex_unlock(&ioLock);

                    nanosleep(&ts, NULL); 
                    int return_val = modbus_write_registers(master.mb_ctx, master.holding_registers.start_address,
                                                            master.holding_registers.num_regs, tempBuff);
                    if (return_val == -1)
                    {
                        if (master.protocol == ProtocolTcp)
                        {
                            modbus_close(master.mb_ctx);
                            master.is_connected = false;
                        }
                        
                        spdlog::error("Modbus Write Holding Registers failed on MB device {}: {}", master.name, modbus_strerror(errno));
                        if (special_functions[2] != NULL) *special_functions[2]++;
                    }
                    
                    free(tempBuff);
                }
            }
        }
        this_thread::sleep_for(master_args->polling_period);
    }

    return 0;
}

void modbus_master_run(oplc::config_stream& cfg_stream,
                       const char* cfg_overrides,
                       const GlueVariablesBinding& bindings,
                       volatile bool& run) {
    // Read the configuration information for the masters
    vector<Master> master_defs;
    ModbusMasterConfig config {
        .polling_period = chrono::milliseconds(100),
        .masters = &master_defs,
    };
    ini_parse_stream(oplc::istream_fgets, cfg_stream.get(),
                     modbus_master_cfg_handler, &config);
    cfg_stream.reset(nullptr);

    // Create the context for each master
    for (size_t index = 0; index < master_defs.size(); ++index) {
        master_defs[index].create();
    }

    //Initialize comm error counter
    if (special_functions[2] != NULL) {
        *special_functions[2] = 0;
    }

    // Start a unified polling thread for all masters
    auto master_args = new MasterArgs {
        .run = &run,
        .polling_period = chrono::milliseconds(config.polling_period),
        .masters = &master_defs };
    pthread_t thread;
    int ret = pthread_create(&thread, nullptr, modbus_master_poll_slaves, master_args);
    if (ret == 0) {
        pthread_detach(thread);
    } else {
        delete master_args;
    }

    while (run) {
        // Sleep for a while to determine if we should terminate
        // A better approach is targeted as a future story
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    // Terminate the unified polling thread.
    pthread_join(thread, nullptr);
}

void modbus_master_service_run(const GlueVariablesBinding& binding,
                               volatile bool& run, const char* config) {
    auto cfg_stream = oplc::open_config();
    modbus_master_run(cfg_stream, config, binding, run);
}

/** @}*/
