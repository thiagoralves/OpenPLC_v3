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

#include <algorithm>
#include <cstring>

#include "service_definition.h"
#include "service_registry.h"
#include "interactive_server.h"
#include "pstorage.h"
#include "modbusslave/slave.h"
#include "modbusmaster/master_indexed.h"
#include "dnp3s/dnp3.h"

using namespace std;
using namespace oplc::modbusm;

ServiceDefinition* services[] = {
    new ServiceDefinition("interactive", interactive_service_run),
    new ServiceDefinition("pstorage", pstorage_service_run, pstorage_service_init),
    new ServiceDefinition("modbusslave", modbus_slave_service_run),
    new ServiceDefinition("modbusmaster", modbus_master_service_run, modbus_master_before_cycle, modbus_master_after_cycle),
#ifdef OPLC_DNP3_OUTSTATION
    new ServiceDefinition("dnp3s", dnp3s_service_run),
#endif
};

ServiceDefinition* services_find(const char* name)
{
    ServiceDefinition** item = find_if(begin(services), end(services), [name] (ServiceDefinition* def)
    {
        return strcmp(def->id(), name) == 0;
    });

    return (item != end(services)) ? *item : nullptr;
}

void services_stop()
{
    for_each(begin(services), end(services), [] (ServiceDefinition* def)
    {
        def->stop();
    });
}

void services_init()
{
    for_each(begin(services), end(services), [] (ServiceDefinition* def)
    {
        def->initialize();
    });
}

void services_finalize()
{
    for_each(begin(services), end(services), [] (ServiceDefinition* def)
    {
        def->finalize();
    });
}

void services_before_cycle() {
    std::for_each(std::begin(services), std::end(services), [] (ServiceDefinition* def)
    {
        def->before_cycle();
    });
}

void services_after_cycle() {
    std::for_each(std::begin(services), std::end(services), [] (ServiceDefinition* def)
    {
        def->after_cycle();
    });
}
