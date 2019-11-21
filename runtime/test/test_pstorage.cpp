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

#include <cstring>
#include <mutex>
#include <sstream>

#include "catch.hpp"
#include "fakeit.hpp"

#include "glue.h"
#include "pstorage.h"

using namespace std;

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

const char VALID_HEADER[] = { static_cast<char>(137), 'O', 'P', 'L', 'C',
                              'P', 'S', '\n', 'v', 0, '\n' };
const char* CHECKSUM  = "16d15b8416040cce48b111ce03ee3dab";

SCENARIO("pstorage_read", "")
{
    mutex glue_mutex;
    stringstream input_stream;
    input_stream.write(VALID_HEADER, 11);
    char endian_header[2] = { IS_BIG_ENDIAN, '\n'};
    input_stream.write(endian_header, 2);
    input_stream.write(CHECKSUM, strlen(CHECKSUM));
    input_stream.put('\n');

    GIVEN("simple glue variables")
    {
        IEC_LWORD lword_var = 0;
        IEC_SINT usint_var = 0;
        IEC_BOOL bool_var = 0;
        GlueBoolGroup grp {
            .index = 0,
            .values = { &bool_var, nullptr, nullptr, nullptr,
                        nullptr, nullptr, nullptr, nullptr}
        };
        const GlueVariable glue_vars[] = {
            { IECLDT_MEM, IECLST_DOUBLEWORD, 0, 0, IECVT_LWORD, &lword_var },
            { IECLDT_MEM, IECLST_BYTE, 0, 0, IECVT_USINT, &usint_var },
            { IECLDT_MEM, IECLST_BIT, 0, 0, IECVT_BOOL, &grp },
        };
        GlueVariablesBinding bindings(&glue_mutex, 3, glue_vars, CHECKSUM);

        WHEN("no data")
        {
            input_stream.seekg(0);
            REQUIRE(pstorage_read(input_stream, bindings) != 0);
        }

        WHEN("data too short")
        {
            input_stream << '1';
            input_stream.seekg(0);
            REQUIRE(pstorage_read(input_stream, bindings) != 0);
        }

        WHEN("data is valid and all zero")
        {
            char buffer[6] = {0};
            input_stream.write(buffer, 6);
            input_stream.seekg(0);
            REQUIRE(pstorage_read(input_stream, bindings) == 0);

            REQUIRE(lword_var == 0);
            REQUIRE(usint_var == 0);
            REQUIRE(bool_var == 0);
        }

        WHEN("data is valid and all one")
        {
            IEC_LWORD lword_initial_value = 1;
            IEC_USINT usint_initial_value = 1;
            char buffer[6] = {0, 0, 0, 0, 1, 1};
            // We don't (in general) know the endianness to know
            // the byte order, so we initialize the buffer based on
            // the actual memory layout
            memcpy(buffer, &lword_initial_value, 4);

            input_stream.write(buffer, 6);
            input_stream.seekg(0);
            REQUIRE(pstorage_read(input_stream, bindings) == 0);

            REQUIRE(lword_var == 1);
            REQUIRE(usint_var == 1);
            REQUIRE(bool_var == 1);
        }
    }

    GIVEN("one bool group")
    {
        IEC_BOOL bool_var0 = 0;
        IEC_BOOL bool_var1 = 0;
        IEC_BOOL bool_var7 = 0;
        GlueBoolGroup grp {
            .index = 0,
            .values = { &bool_var0, &bool_var1, nullptr, nullptr,
                        nullptr, nullptr, nullptr, &bool_var7}
        };
        const GlueVariable glue_vars[] = {
            { IECLDT_MEM, IECLST_BIT, 0, 0, IECVT_BOOL, &grp },
        };
        GlueVariablesBinding bindings(&glue_mutex, 1, glue_vars, CHECKSUM);

        WHEN("data is valid and mixture of bits set")
        {
            // We don't (in general) know the endianness to know
            // the byte order, so we initialize the buffer based on
            // the actual memory layout
            char one_char = 0x81;

            input_stream.write(&one_char, 1);
            input_stream.seekg(0);
            REQUIRE(pstorage_read(input_stream, bindings) == 0);

            REQUIRE(bool_var0 == 1);
            REQUIRE(bool_var1 == 0);
            REQUIRE(bool_var7 == 1);
        }
    }
}

SCENARIO("pstorage_run")
{
    mutex glue_mutex;
    stringstream input_stream;
    input_stream.write(VALID_HEADER, 11);
    char endian_header[2] = { IS_BIG_ENDIAN, '\n'};
    input_stream.write(endian_header, 2);
    input_stream.write(CHECKSUM, strlen(CHECKSUM));
    input_stream.put('\n');

    GIVEN("glue variables and stream")
    {
        IEC_LWORD lword_var = 1;
        IEC_SINT usint_var = 2;
        IEC_BOOL bool_var = 1;
        GlueBoolGroup grp {
            .index = 0,
            .values = { &bool_var, nullptr, nullptr, nullptr,
                        nullptr, nullptr, nullptr, nullptr}
        };
        const GlueVariable glue_vars[] = {
            { IECLDT_MEM, IECLST_DOUBLEWORD, 0, 0, IECVT_LWORD, &lword_var },
            { IECLDT_MEM, IECLST_BYTE, 0, 0, IECVT_USINT, &usint_var },
            { IECLDT_MEM, IECLST_BIT, 0, 0, IECVT_BOOL, &grp },
        };
        GlueVariablesBinding bindings(&glue_mutex, 3, glue_vars, CHECKSUM);

        oplc::config_stream cfg_stream(new stringstream(""),
                                       [](istream* s) { delete s; });

        WHEN("write once")
        {
            volatile bool run = false;
            auto create_stream = []() { return new stringstream(); };
            auto result = pstorage_run(cfg_stream, "0", bindings,
                                       run, create_stream);
            REQUIRE(result == 0);
        }
    }
}
