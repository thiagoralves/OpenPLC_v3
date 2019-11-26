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
#ifndef OPLC_UNIT_TEST
#include <modbus.h>
#endif  // OPLC_UNIT_TEST

#include <cstdint>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <string>
#include <vector>

#include "ladder.h"
#include "master.h"
#include "master_indexed.h"
#include "../glue.h"
#include "../ini_util.h"

using namespace std;

oplc::modbusm::MasterConfig::MasterConfig() :
    name{'\0'},
    protocol(ProtocolInvalid),
    backoff_strategy(BackoffStrategyNone),
    ip_address{'\0'},
    response_timeout(chrono::milliseconds(250)),
    poll_interval(chrono::milliseconds(500))
{}

/// @brief Create the context for the device according to the configuration
/// parameters provided by the user.
/// @return the created context.
modbus_t* oplc::modbusm::MasterConfig::create() const
{
    modbus_t* mb_ctx;
    if (protocol == ProtocolTcp)
    {
        mb_ctx = modbus_new_tcp(ip_address, ip_port);
    }
    else if (protocol == ProtocolRtu)
    {
        mb_ctx = modbus_new_rtu(ip_address, rtu_baud_rate, rtu_parity, rtu_data_bit, rtu_stop_bit);
    }

    modbus_set_slave(mb_ctx, slave_id);

    auto sec = chrono::duration_cast<chrono::seconds>(response_timeout);
    auto usec = response_timeout - sec;
    modbus_set_response_timeout(mb_ctx, sec.count(), usec.count());

    return mb_ctx;
}

std::int16_t oplc::modbusm::common_cfg_handler(
    std::array<oplc::modbusm::MasterConfig, MODBUS_MASTER_MAX>& masters,
    const char* name,
    const char* value)
{
    const uint8_t max = MODBUS_MASTER_MAX - 1;
    uint8_t index;
    if (oplc::strcmp_with_index(name, "name", max, &index) == 0)
    {
        strncpy(masters[index].name, value, MASTER_ITEM_SIZE);
        masters[index].name[MASTER_ITEM_SIZE - 1] = '\0';
    }
    else if (oplc::strcmp_with_index(name, "protocol", max, &index) == 0)
    {
        if (strcmp(value, "tcp") == 0)
        {
            masters[index].protocol = ProtocolTcp;
        }
        else if (strcmp(value, "rtu") == 0)
        {
            masters[index].protocol = ProtocolRtu;
        }
        else
        {
            spdlog::warn("Unknown protocol configuration value {}", value);
        }
    }
    else if (oplc::strcmp_with_index(name, "backoff_strategy", max, &index) == 0)
    {
        if (strcmp(value, "linear_bounded") == 0)
        {
            masters[index].backoff_strategy = BackoffStrategyLinear;
        }
        else
        {
            spdlog::warn("Unknown backoff strategy configuration value {}", value);
        }
    }
    else if (oplc::strcmp_with_index(name, "slave_id", max, &index) == 0)
    {
        masters[index].slave_id = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "response_timeout", max, &index) == 0)
    {
        masters[index].response_timeout = chrono::microseconds(atoi(value));
    }
    else if (oplc::strcmp_with_index(name, "poll_interval", max, &index) == 0)
    {
        masters[index].poll_interval = chrono::microseconds(atoi(value));
    }
    else if (oplc::strcmp_with_index(name, "ip_address", max, &index) == 0)
    {
        strncpy(masters[index].ip_address, value, MASTER_ITEM_SIZE);
        masters[index].ip_address[MASTER_ITEM_SIZE - 1] = '\0';
    }
    else if (oplc::strcmp_with_index(name, "ip_port", max, &index) == 0)
    {
        masters[index].ip_port = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "rtu_baud_rate", max, &index) == 0)
    {
        masters[index].rtu_baud_rate = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "rtu_parity", max, &index) == 0)
    {
        masters[index].rtu_parity = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "rtu_data_bit", max, &index) == 0)
    {
        masters[index].rtu_data_bit = atoi(value);
    }
    else if (oplc::strcmp_with_index(name, "rtu_stop_bit", max, &index) == 0)
    {
        masters[index].rtu_stop_bit = atoi(value);
    }
    else
    {
        return -1;
    }

    return 0;
}

/** @}*/
