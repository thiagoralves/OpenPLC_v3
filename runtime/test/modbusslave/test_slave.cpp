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
#include "modbusslave/slave.h"
#include "modbusslave/indexed_strategy.h"

using namespace std;

#define HEADER 0, 0, 0, 0, 0, 0, 0

SCENARIO("modbusslave", "")
{
    mutex glue_mutex;

    GIVEN("glue variables with multiple bit outputs")
    {
        IEC_BOOL bool_val0(1);
        IEC_BOOL bool_val1(0);
        IEC_BOOL bool_val2(1);
        IEC_BOOL bool_val3(1);
        IEC_BOOL bool_val4(0);
        IEC_BOOL bool_val5(0);
        IEC_BOOL bool_val6(1);
        IEC_BOOL bool_val7(1);
        IEC_BOOL bool_val8(1);
        auto group0 = GlueBoolGroup {
            .index = 0,
            .values = {
                &bool_val0, &bool_val1, &bool_val2, &bool_val3, &bool_val4, &bool_val5, &bool_val6, &bool_val7
            }
        };
        auto group1 = GlueBoolGroup {
            .index = 1,
            .values = {
                &bool_val8, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
            }
        };

        const GlueVariable glue_vars[] = {
            { IECLDT_OUT, IECLST_BIT, 0, 0, IECVT_BOOL, &group0 },
            { IECLDT_OUT, IECLST_BIT, 1, 0, IECVT_BOOL, &group1 },
        };
        GlueVariablesBinding bindings(&glue_mutex, 2, glue_vars, nullptr);
        IndexedStrategy strategy(bindings);

        const uint8_t BUF_SIZE(255);

        WHEN("read starts from 0 and covers multiple bytes")
        {
            //                                  FS, Start, Num, CRC
            uint8_t buffer[BUF_SIZE] = { HEADER, 1, 0, 0, 0, 9, 0 };
            int16_t size = modbus_process_message(buffer, BUF_SIZE, &strategy);
            REQUIRE(buffer[7] == 0x01);
            REQUIRE(buffer[8] == 0x02);
            REQUIRE(buffer[9] == 0xCD);
            REQUIRE(buffer[10] == 0x01);
            REQUIRE(size == 11);
        }

        WHEN("read starts from 1 and covers exactly one byte")
        {
            //                                  FS, Start, Num, CRC
            uint8_t buffer[BUF_SIZE] = { HEADER, 1, 0, 1, 0, 8, 0 };
            int16_t size = modbus_process_message(buffer, BUF_SIZE, &strategy);
            REQUIRE(buffer[7] == 0x01);
            REQUIRE(buffer[8] == 0x01);
            REQUIRE(buffer[9] == 0xE6);
            REQUIRE(size == 10);
        }

        WHEN("write starts from 0 and covers exactly two byte")
        {
            //                                   FS, Start, Num, By,    Values, CRC
            uint8_t buffer[BUF_SIZE] = { HEADER, 15, 0, 0, 0, 9, 2, 0xCD, 0x01, 0 };
            int16_t size = modbus_process_message(buffer, BUF_SIZE, &strategy);
            REQUIRE(buffer[7] == 15);
            REQUIRE(buffer[8] == 0);
            REQUIRE(buffer[9] == 0);
            REQUIRE(buffer[10] == 0);
            REQUIRE(buffer[11] == 9);
            REQUIRE(size == 12);
        }
    }

    GIVEN("glue variables with multiple bit inputs")
    {
        IEC_BOOL bool_val0(1);
        IEC_BOOL bool_val1(0);
        IEC_BOOL bool_val2(1);
        IEC_BOOL bool_val3(1);
        IEC_BOOL bool_val4(0);
        IEC_BOOL bool_val5(0);
        IEC_BOOL bool_val6(1);
        IEC_BOOL bool_val7(1);
        IEC_BOOL bool_val8(1);
        auto group0 = GlueBoolGroup {
            .index = 0,
            .values = {
                &bool_val0, &bool_val1, &bool_val2, &bool_val3, &bool_val4, &bool_val5, &bool_val6, &bool_val7
            }
        };
        auto group1 = GlueBoolGroup {
            .index = 1,
            .values = {
                &bool_val8, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
            }
        };

        const GlueVariable glue_vars[] = {
            { IECLDT_IN, IECLST_BIT, 0, 0, IECVT_BOOL, &group0 },
            { IECLDT_IN, IECLST_BIT, 1, 0, IECVT_BOOL, &group1 },
        };
        GlueVariablesBinding bindings(&glue_mutex, 2, glue_vars, nullptr);
        IndexedStrategy strategy(bindings);

        const uint8_t BUF_SIZE(255);

        WHEN("starts from 0 and covers multiple bytes")
        {
            //                                  FS, Start, Num, CRC
            uint8_t buffer[BUF_SIZE] = { HEADER, 2, 0, 0, 0, 9, 0 };
            int16_t size = modbus_process_message(buffer, BUF_SIZE, &strategy);
            REQUIRE(buffer[7] == 0x02);
            REQUIRE(buffer[8] == 0x02);
            REQUIRE(buffer[9] == 0xCD);
            REQUIRE(buffer[10] == 0x01);
            REQUIRE(size == 11);
        }

        WHEN("starts from 1 and covers exactly one byte")
        {
            //                                  FS, Start, Num, CRC
            uint8_t buffer[BUF_SIZE] = { HEADER, 2, 0, 1, 0, 8, 0 };
            int16_t size = modbus_process_message(buffer, BUF_SIZE, &strategy);
            REQUIRE(buffer[7] == 0x02);
            REQUIRE(buffer[8] == 0x01);
            REQUIRE(buffer[9] == 0xE6);
            REQUIRE(size == 10);
        }
    }

    GIVEN("glue variables with multiple types output")
    {
        IEC_INT int_val1(1);
        IEC_INT int_val2(2);
        IEC_DINT dint_val(3);
        IEC_LINT lint_val(4);
        const GlueVariable glue_vars[] = {
            { IECLDT_OUT, IECLST_WORD, 0, 0, IECVT_INT, &int_val1 },
            { IECLDT_MEM, IECLST_WORD, 1024, 0, IECVT_INT, &int_val2 },
            { IECLDT_MEM, IECLST_DOUBLEWORD, 2048, 0, IECVT_DINT, &dint_val },
            { IECLDT_MEM, IECLST_LONGWORD, 4096, 0, IECVT_LINT, &lint_val },
        };
        GlueVariablesBinding bindings(&glue_mutex, 4, glue_vars, nullptr);
        IndexedStrategy strategy(bindings);

        const uint8_t BUF_SIZE(255);

        WHEN("reads int output value")
        {
            //                                  FS, Start, Num, CRC
            uint8_t buffer[BUF_SIZE] = { HEADER, 3, 0, 0, 0, 1, 0 };
            int16_t size = modbus_process_message(buffer, BUF_SIZE, &strategy);
            REQUIRE(buffer[7] == 0x03);
            REQUIRE(buffer[8] == 0x02);
            REQUIRE(buffer[9] == 0x00);
            REQUIRE(buffer[10] == 0x01);
            REQUIRE(size == 11);
        }

        WHEN("reads int mem value")
        {
            //                                  FS,   Start,  Num, CRC
            uint8_t buffer[BUF_SIZE] = { HEADER, 3, 0x04, 0, 0, 1, 0 };
            int16_t size = modbus_process_message(buffer, BUF_SIZE, &strategy);
            REQUIRE(buffer[7] == 0x03);
            REQUIRE(buffer[8] == 0x02);
            REQUIRE(buffer[9] == 0x00);
            REQUIRE(buffer[10] == 0x02);
            REQUIRE(size == 11);
        }

        WHEN("write int output value")
        {
            //                                  FS, Start,    Val, CRC
            uint8_t buffer[BUF_SIZE] = { HEADER, 6, 0, 0, 0, 0x03, 0 };
            int16_t size = modbus_process_message(buffer, BUF_SIZE, &strategy);
            REQUIRE(buffer[7] == 0x06);
            REQUIRE(buffer[8] == 0x00);
            REQUIRE(buffer[9] == 0x00);
            REQUIRE(buffer[10] == 0x00);
            REQUIRE(buffer[11] == 0x03);
            REQUIRE(size == 12);
        }

       WHEN("write int output values")
        {
            //                                   FS, Start,  Num, By,     Val, CRC
            uint8_t buffer[BUF_SIZE] = { HEADER, 16, 0, 0,  0, 1,  2, 0, 0x02, 0 };
            int16_t size = modbus_process_message(buffer, BUF_SIZE, &strategy);
            REQUIRE(buffer[7] == 16);
            REQUIRE(buffer[8] == 0x00);
            REQUIRE(buffer[9] == 0x00);
            REQUIRE(buffer[10] == 0x00);
            REQUIRE(buffer[11] == 0x01);
            REQUIRE(size == 12);
            strategy.Exchange();
            REQUIRE(int_val1 == 2);
        }
    }

    GIVEN("glue variables with input")
    {
        IEC_INT int_val1(2);
        const GlueVariable glue_vars[] = {
            { IECLDT_IN, IECLST_WORD, 0, 0, IECVT_INT, &int_val1 },
        };
        GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars, nullptr);
        IndexedStrategy strategy(bindings);

        const uint8_t BUF_SIZE(255);

        WHEN("reads int input value")
        {
            //                                  FS, Start, Num, CRC
            uint8_t buffer[BUF_SIZE] = { HEADER, 4, 0, 0, 0, 1, 0 };
            int16_t size = modbus_process_message(buffer, BUF_SIZE, &strategy);
            REQUIRE(buffer[7] == 0x04);
            REQUIRE(buffer[8] == 0x02);
            REQUIRE(buffer[9] == 0x00);
            REQUIRE(buffer[10] == 0x02);
            REQUIRE(size == 11);
        }
    }

    GIVEN("glue variables with output bool")
    {
        IEC_BOOL bool_val1(0);
        auto group1 = GlueBoolGroup {
            .index = 0,
            .values = {
                &bool_val1, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
            }
        };
        const GlueVariable glue_vars[] = {
            { IECLDT_OUT, IECLST_BIT, 0, 0, IECVT_BOOL, &group1 },
        };
        GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars, nullptr);
        IndexedStrategy strategy(bindings);

        const uint8_t BUF_SIZE(255);

        WHEN("write one coil")
        {
            //                                  FS, Start, Num, CRC
            uint8_t buffer[BUF_SIZE] = { HEADER, 5, 0, 0, 0xFF, 0, 0 };
            int16_t size = modbus_process_message(buffer, BUF_SIZE, &strategy);
            REQUIRE(buffer[7] == 0x05);
            REQUIRE(buffer[8] == 0x00);
            REQUIRE(buffer[9] == 0x00);
            REQUIRE(buffer[10] == 0xFF);
            REQUIRE(buffer[11] == 0x00);
            REQUIRE(size == 12);
            strategy.Exchange();
            REQUIRE(bool_val1 == 1);

        }
    }
}
