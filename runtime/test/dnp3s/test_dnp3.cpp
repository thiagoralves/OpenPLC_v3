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

#include <cstdint>
#include <utility>
#include <mutex>
#include <asiodnp3/OutstationStackConfig.h>

#include "catch.hpp"
#include "fakeit.hpp"

void sleep_until(timespec*, int) {}
#include "glue.h"
#include "dnp3s/dnp3.h"

using namespace asiodnp3;
using namespace std;

SCENARIO("create_config", "")
{
    mutex glue_mutex;
    unique_ptr<istream, std::function<void(istream*)>> cfg_stream(new stringstream(""), [](istream* s) { delete s; });
    Dnp3IndexedGroup binary_commands = {0};
    Dnp3IndexedGroup analog_commands = {0};
    Dnp3MappedGroup measurements = {0};
    std::uint16_t port;

    GIVEN("<input stream>")
    {
        WHEN("stream is empty creates default config")
        {
            GlueVariablesBinding bindings(&glue_mutex, 0, nullptr);
            std::stringstream input_stream;
            const OutstationStackConfig config(dnp3_create_config(input_stream, bindings, binary_commands, analog_commands, measurements, port));

            REQUIRE(config.dbConfig.binary.IsEmpty());
            REQUIRE(config.dbConfig.doubleBinary.IsEmpty());
            REQUIRE(config.dbConfig.analog.IsEmpty());
            REQUIRE(config.dbConfig.counter.IsEmpty());
            REQUIRE(config.dbConfig.frozenCounter.IsEmpty());
            REQUIRE(config.dbConfig.boStatus.IsEmpty());
            REQUIRE(config.dbConfig.aoStatus.IsEmpty());
            REQUIRE(config.dbConfig.timeAndInterval.IsEmpty());
            REQUIRE(config.link.IsMaster == false);
            REQUIRE(config.link.UseConfirms == false);
            REQUIRE(config.link.NumRetry == 0);
            REQUIRE(config.link.LocalAddr == 1024);
            REQUIRE(config.link.RemoteAddr == 1);
        }

        WHEN("stream specifies size based on glue variables for one boolean")
        {
            bool bool_var;
            const GlueVariable glue_vars[] = {
                { IECLDT_OUT, IECLST_BIT, 0, 0, IECVT_BOOL, &bool_var },
            };
            GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars);
            std::stringstream input_stream("[dnp3s]\nbind_location=name:%QX0.0,group:1,index:0,");
            const OutstationStackConfig config(dnp3_create_config(input_stream, bindings, binary_commands, analog_commands, measurements, port));

            REQUIRE(config.dbConfig.binary.Size() == 1);
            REQUIRE(config.dbConfig.doubleBinary.Size() == 0);
            REQUIRE(config.dbConfig.analog.Size() == 0);
            REQUIRE(config.dbConfig.counter.Size() == 0);
            REQUIRE(config.dbConfig.frozenCounter.Size() == 0);
            REQUIRE(config.dbConfig.boStatus.Size() == 0);
            REQUIRE(config.dbConfig.aoStatus.Size() == 0);
            REQUIRE(config.dbConfig.timeAndInterval.Size() == 0);
            REQUIRE(config.link.IsMaster == false);
            REQUIRE(config.link.UseConfirms == false);
            REQUIRE(config.link.NumRetry == 0);
            REQUIRE(config.link.LocalAddr == 1024);
            REQUIRE(config.link.RemoteAddr == 1);

            // We should have bound the one variable
            REQUIRE(measurements.items[0].variable == &(glue_vars[0]));
        }

        WHEN("stream specifies size based on glue variables for one boolean at index 1")
        {
            bool bool_var;
            const GlueVariable glue_vars[] = {
                { IECLDT_IN, IECLST_BIT, 0, 0, IECVT_BOOL, &bool_var },
            };
            GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars);
            std::stringstream input_stream("[dnp3s]\nbind_location=name:%IX0.0,group:1,index:1,");
            const OutstationStackConfig config(dnp3_create_config(input_stream, bindings, binary_commands, analog_commands, measurements, port));

            REQUIRE(config.dbConfig.binary.Size() == 1);
            REQUIRE(config.dbConfig.doubleBinary.Size() == 0);
            REQUIRE(config.dbConfig.analog.Size() == 0);
            REQUIRE(config.dbConfig.counter.Size() == 0);
            REQUIRE(config.dbConfig.frozenCounter.Size() == 0);
            REQUIRE(config.dbConfig.boStatus.Size() == 0);
            REQUIRE(config.dbConfig.aoStatus.Size() == 0);
            REQUIRE(config.dbConfig.timeAndInterval.Size() == 0);
            REQUIRE(config.link.IsMaster == false);
            REQUIRE(config.link.UseConfirms == false);
            REQUIRE(config.link.NumRetry == 0);
            REQUIRE(config.link.LocalAddr == 1024);
            REQUIRE(config.link.RemoteAddr == 1);

            // We should have bound the one variable
            REQUIRE(measurements.items[0].variable == &(glue_vars[0]));
        }

        WHEN("stream specifies size based on glue variables for one boolean command at index 1")
        {
            bool bool_var;
            const GlueVariable glue_vars[] = {
                { IECLDT_IN, IECLST_BIT, 0, 0, IECVT_BOOL, &bool_var },
            };
            GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars);
            std::stringstream input_stream("[dnp3s]\nbind_location=name:%IX0.0,group:12,index:1,");
            const OutstationStackConfig config(dnp3_create_config(input_stream, bindings, binary_commands, analog_commands, measurements, port));

            REQUIRE(config.dbConfig.binary.Size() == 0);
            REQUIRE(config.dbConfig.doubleBinary.Size() == 0);
            REQUIRE(config.dbConfig.analog.Size() == 0);
            REQUIRE(config.dbConfig.counter.Size() == 0);
            REQUIRE(config.dbConfig.frozenCounter.Size() == 0);
            REQUIRE(config.dbConfig.boStatus.Size() == 0);
            REQUIRE(config.dbConfig.aoStatus.Size() == 0);
            REQUIRE(config.dbConfig.timeAndInterval.Size() == 0);
            REQUIRE(config.link.IsMaster == false);
            REQUIRE(config.link.UseConfirms == false);
            REQUIRE(config.link.NumRetry == 0);
            REQUIRE(config.link.LocalAddr == 1024);
            REQUIRE(config.link.RemoteAddr == 1);

            // We should have bound the one variable
            REQUIRE(measurements.size == 0);
            REQUIRE(binary_commands.size == 2);
            REQUIRE(binary_commands.items[1] == &(glue_vars[0]));
        }

        WHEN("stream specifies size based on glue variables for one real at index 1")
        {
            IEC_REAL real_var(9);
            const GlueVariable glue_vars[] = {
                { IECLDT_OUT, IECLST_DOUBLEWORD, 0, 0, IECVT_REAL, &real_var },
            };
            GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars);
            std::stringstream input_stream("[dnp3s]\nbind_location=name:%QD0,group:30,index:1,");
            const OutstationStackConfig config(dnp3_create_config(input_stream, bindings, binary_commands, analog_commands, measurements, port));

            REQUIRE(config.dbConfig.binary.Size() == 0);
            REQUIRE(config.dbConfig.doubleBinary.Size() == 0);
            REQUIRE(config.dbConfig.analog.Size() == 1);
            REQUIRE(config.dbConfig.counter.Size() == 0);
            REQUIRE(config.dbConfig.frozenCounter.Size() == 0);
            REQUIRE(config.dbConfig.boStatus.Size() == 0);
            REQUIRE(config.dbConfig.aoStatus.Size() == 0);
            REQUIRE(config.dbConfig.timeAndInterval.Size() == 0);
            REQUIRE(config.link.IsMaster == false);
            REQUIRE(config.link.UseConfirms == false);
            REQUIRE(config.link.NumRetry == 0);
            REQUIRE(config.link.LocalAddr == 1024);
            REQUIRE(config.link.RemoteAddr == 1);

            // We should have bound the one variable
            REQUIRE(measurements.items[0].variable->value == &real_var);
        }
    }
}

SCENARIO("dnp3s_start_server", "")
{
    mutex glue_mutex;

    WHEN("provides configuration stream but not run")
    {
        // Configure this to start and then immediately terminate
        // the run flag is set to false. This should just return quickly
        volatile bool run_dnp3(false);
        unique_ptr<istream, std::function<void(istream*)>> cfg_stream(new stringstream(""), [](istream* s) { delete s; });
        GlueVariablesBinding bindings(&glue_mutex, 0, nullptr);

        dnp3s_start_server(cfg_stream, "20000", run_dnp3, bindings);
    }
}

#endif  // OPLC_DNP3_OUTSTATION
