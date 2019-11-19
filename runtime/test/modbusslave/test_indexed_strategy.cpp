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

#include <mutex>

#include "catch.hpp"

#include "glue.h"
#include "modbusslave/indexed_strategy.h"

using namespace std;

SCENARIO("indexed_strategy", "")
{
    mutex glue_mutex;

    GIVEN("glue variables with single bit output glue variable")
    {
        IEC_BOOL bool_val(0);
        auto group = GlueBoolGroup { .index=0, .values={ &bool_val, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr } };

        const GlueVariable glue_vars[] = {
            { IECLDT_OUT, IECLST_BIT, 0, 0, IECVT_BOOL, &group },
        };
        GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars, nullptr);
        IndexedStrategy strategy(bindings);

        uint8_t buffer[1];

        WHEN("read coils when value is false")
        {
            bool_val = 0;
            strategy.Exchange();
            REQUIRE(strategy.ReadCoils(0, 1, buffer) == 0);
            REQUIRE(buffer[0] == 0);
        }

        WHEN("read coils when value is true")
        {
            bool_val = 1;
            strategy.Exchange();
            REQUIRE(strategy.ReadCoils(0, 1, buffer) == 0);
            REQUIRE(buffer[0] == 1);
        }

        WHEN("write coils when value is true")
        {
            bool_val = 0;
            REQUIRE(strategy.WriteCoil(0, true) == 0);
            strategy.Exchange();
            REQUIRE(bool_val == 1);
        }
    }

    GIVEN("glue variables with single int output glue variable")
    {
        IEC_INT int_val(0);
        const GlueVariable glue_vars[] = {
            { IECLDT_OUT, IECLST_WORD, 0, 0, IECVT_INT, &int_val },
        };
        GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars, nullptr);
        IndexedStrategy strategy(bindings);

        uint8_t buffer[1];

        WHEN("read holding register when value is 0")
        {
            int_val = 0;
            uint8_t buffer[2];
            strategy.Exchange();
            REQUIRE(strategy.ReadHoldingRegisters(0, 1, buffer) == 0);
            REQUIRE(buffer[0] == 0);
            REQUIRE(buffer[1] == 0);
        }

        WHEN("read holding register when value is 1")
        {
            int_val = 1;
            uint8_t buffer[2];
            strategy.Exchange();
            REQUIRE(strategy.ReadHoldingRegisters(0, 1, buffer) == 0);
            REQUIRE(buffer[0] == 0);
            REQUIRE(buffer[1] == 1);
        }

        WHEN("write holding register when value is 1")
        {
            int_val = 0;
            uint8_t buffer[2] = { 0, 1 };
            REQUIRE(strategy.WriteHoldingRegisters(0, 1, buffer) == 0);
            strategy.Exchange();
            REQUIRE(int_val == 1);
        }
    }

    GIVEN("glue variables with single int mem glue variable")
    {
        IEC_INT int_val(0);
        const GlueVariable glue_vars[] = {
            { IECLDT_MEM, IECLST_WORD, 1024, 0, IECVT_INT, &int_val },
        };
        GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars, nullptr);
        IndexedStrategy strategy(bindings);

        uint8_t buffer[1];

        WHEN("read holding register when value is 0")
        {
            int_val = 0;
            uint8_t buffer[2];
            strategy.Exchange();
            REQUIRE(strategy.ReadHoldingRegisters(1024, 1, buffer) == 0);
            REQUIRE(buffer[0] == 0);
            REQUIRE(buffer[1] == 0);
        }

        WHEN("read holding register when value is 1")
        {
            int_val = 1;
            uint8_t buffer[2];
            strategy.Exchange();
            REQUIRE(strategy.ReadHoldingRegisters(1024, 1, buffer) == 0);
            REQUIRE(buffer[0] == 0);
            REQUIRE(buffer[1] == 1);
        }

        WHEN("write holding register when value is 1")
        {
            int_val = 0;
            uint8_t buffer[2] = { 0, 1 };
            REQUIRE(strategy.WriteHoldingRegisters(1024, 1, buffer) == 0);
            strategy.Exchange();
            REQUIRE(int_val == 1);
        }
    }

    GIVEN("glue variables with single dint mem glue variable")
    {
        IEC_DINT dint_val(0);
        const GlueVariable glue_vars[] = {
            { IECLDT_MEM, IECLST_DOUBLEWORD, 2048, 0, IECVT_DINT, &dint_val },
        };
        GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars, nullptr);
        IndexedStrategy strategy(bindings);

        uint8_t buffer[1];

        WHEN("read holding register when value is 0")
        {
            dint_val = 0;
            uint8_t buffer[4];
            strategy.Exchange();
            REQUIRE(strategy.ReadHoldingRegisters(2048, 2, buffer) == 0);
            REQUIRE(buffer[0] == 0);
            REQUIRE(buffer[1] == 0);
            REQUIRE(buffer[2] == 0);
            REQUIRE(buffer[3] == 0);
        }

        WHEN("read holding register when value is 1")
        {
            dint_val = 1;
            uint8_t buffer[4];
            strategy.Exchange();
            REQUIRE(strategy.ReadHoldingRegisters(2048, 2, buffer) == 0);
            REQUIRE(buffer[0] == 0);
            REQUIRE(buffer[1] == 0);
            REQUIRE(buffer[2] == 0);
            REQUIRE(buffer[3] == 1);
        }
    }

    GIVEN("glue variables with single lint mem glue variable")
    {
        IEC_LINT lint_val(0);
        const GlueVariable glue_vars[] = {
            { IECLDT_MEM, IECLST_LONGWORD, 4096, 0, IECVT_LINT, &lint_val },
        };
        GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars, nullptr);
        IndexedStrategy strategy(bindings);

        uint8_t buffer[1];

        WHEN("read holding register when value is 0")
        {
            lint_val = 0;
            uint8_t buffer[8];
            strategy.Exchange();
            REQUIRE(strategy.ReadHoldingRegisters(4096, 4, buffer) == 0);
            REQUIRE(buffer[0] == 0);
            REQUIRE(buffer[1] == 0);
            REQUIRE(buffer[2] == 0);
            REQUIRE(buffer[3] == 0);
            REQUIRE(buffer[4] == 0);
            REQUIRE(buffer[5] == 0);
            REQUIRE(buffer[6] == 0);
            REQUIRE(buffer[7] == 0);
        }

        WHEN("read holding register when value is 1")
        {
            lint_val = 1;
            uint8_t buffer[8];
            strategy.Exchange();
            REQUIRE(strategy.ReadHoldingRegisters(4096, 4, buffer) == 0);
            REQUIRE(buffer[0] == 0);
            REQUIRE(buffer[1] == 0);
            REQUIRE(buffer[2] == 0);
            REQUIRE(buffer[3] == 0);
            REQUIRE(buffer[4] == 0);
            REQUIRE(buffer[5] == 0);
            REQUIRE(buffer[6] == 0);
            REQUIRE(buffer[7] == 1);
        }
    }
}
