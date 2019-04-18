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

// Catch2 will provide a main() function
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <sstream>

// Our application is defined in a single CPP file, so to write tests against it
// we need to include it directly here. That's ok so long as the application is
// no more than one CPP file.
#define OPLCGLUE_OMIT_MAIN
#include "../glue_generator.cpp"

using namespace std;

#define PREFIX "void glueVars()\r\n{\r\n"
#define POSTFIX "}\r\n\r\n"
#define EMPTY_INPUT "/// The size of the array of input variables.\r\n\
const std:::uint16_t OPLCGLUE_INPUT_SIZE(0);\r\n\
GlueVariable oplc_input_vars[1] = {\r\n\
};\r\n\r\n"
#define EMPTY_OUTPUT "/// The size of the array of output variables.\r\n\
const std:::uint16_t OPLCGLUE_OUTPUT_SIZE(0);\r\n\
GlueVariable oplc_output_vars[1] = {\r\n\
};\r\n\r\n"


SCENARIO("Commmand line", "[main]") {
    GIVEN("<no pre-conditions>") {
        WHEN("-h command line arguments") {
            char* args[2] = { "glue_generator", "-h" };
            REQUIRE(mainImpl(2, args) == 0);
        }

        WHEN("--help command line arguments") {
            char* args[2] = { "glue_generator", "-h" };
            REQUIRE(mainImpl(2, args) == 0);
        }
    }
}

SCENARIO("", "") {
    GIVEN("IO as streams") {
        std::stringstream output_stream;
        WHEN("Contains single BOOL at %IX0") {
            std::stringstream input_stream("__LOCATED_VAR(BOOL,__IX0,I,X,0)");
            generateBody(input_stream, output_stream);

            const char* expected = PREFIX "\tbool_input[0][0] = __IX0;\r\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single BOOL at %QX0") {
            std::stringstream input_stream("__LOCATED_VAR(BOOL,__QX0,Q,X,0)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == PREFIX "\tbool_output[0][0] = __QX0;\r\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT);
        }

        WHEN("Contains single BYTE at %IB0") {
            std::stringstream input_stream("__LOCATED_VAR(BYTE,__IB0,I,B,0)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX"\tbyte_input[0] = __IB0;\r\n" POSTFIX "/// The size of the array of input variables.\r\n\
const std:::uint16_t OPLCGLUE_INPUT_SIZE(1);\r\n\
GlueVariable oplc_input_vars[1] = {\r\n\
    { IEC_BYTE, __IB0 },\r\n\
};\r\n\r\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single SINT at %IB1") {
            std::stringstream input_stream("__LOCATED_VAR(SINT,__IB1,I,B,1)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tbyte_input[1] = __IB1;\r\n" POSTFIX "/// The size of the array of input variables.\r\n\
const std:::uint16_t OPLCGLUE_INPUT_SIZE(2);\r\n\
GlueVariable oplc_input_vars[2] = {\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_SINT, __IB1 },\r\n\
};\r\n\r\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single SINT at %QB1") {
            std::stringstream input_stream("__LOCATED_VAR(SINT,__QB1,Q,B,1)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tbyte_output[1] = __QB1;\r\n" POSTFIX EMPTY_INPUT "/// The size of the array of output variables.\r\n\
const std:::uint16_t OPLCGLUE_OUTPUT_SIZE(2);\r\n\
GlueVariable oplc_output_vars[2] = {\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_SINT, __QB1 },\r\n\
};\r\n\r\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single USINT at %IB2") {
            std::stringstream input_stream("__LOCATED_VAR(USINT,__IB2,I,B,2)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tbyte_input[2] = __IB2;\r\n" POSTFIX "/// The size of the array of input variables.\r\n\
const std:::uint16_t OPLCGLUE_INPUT_SIZE(3);\r\n\
GlueVariable oplc_input_vars[3] = {\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_USINT, __IB2 },\r\n\
};\r\n\r\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single WORD at %IW0") {
            std::stringstream input_stream("__LOCATED_VAR(WORD,__IW0,I,W,0)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tint_input[0] = __IW0;\r\n" POSTFIX "/// The size of the array of input variables.\r\n\
const std:::uint16_t OPLCGLUE_INPUT_SIZE(1);\r\n\
GlueVariable oplc_input_vars[1] = {\r\n\
    { IEC_WORD, __IW0 },\r\n\
};\r\n\r\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single WORD at %QW0") {
            std::stringstream input_stream("__LOCATED_VAR(WORD,__QW0,Q,W,0)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tint_output[0] = __QW0;\r\n" POSTFIX EMPTY_INPUT "/// The size of the array of output variables.\r\n\
const std:::uint16_t OPLCGLUE_OUTPUT_SIZE(1);\r\n\
GlueVariable oplc_output_vars[1] = {\r\n\
    { IEC_WORD, __QW0 },\r\n\
};\r\n\r\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single INT at %IW1") {
            std::stringstream input_stream("__LOCATED_VAR(INT,__IW1,I,W,1)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tint_input[1] = __IW1;\r\n" POSTFIX "/// The size of the array of input variables.\r\n\
const std:::uint16_t OPLCGLUE_INPUT_SIZE(2);\r\n\
GlueVariable oplc_input_vars[2] = {\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_INT, __IW1 },\r\n\
};\r\n\r\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single UINT at %IW2") {
            std::stringstream input_stream("__LOCATED_VAR(UINT,__IW2,I,W,2)");
            generateBody(input_stream, output_stream);

            const char* expected = PREFIX "\tint_input[2] = __IW2;\r\n" POSTFIX "/// The size of the array of input variables.\r\n\
const std:::uint16_t OPLCGLUE_INPUT_SIZE(3);\r\n\
GlueVariable oplc_input_vars[3] = {\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_UINT, __IW2 },\r\n\
};\r\n\r\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains two REAL at %ID0 and %ID10") {
            std::stringstream input_stream("__LOCATED_VAR(REAL,__ID0,I,D,0)\r\n__LOCATED_VAR(REAL,__ID10,I,D,10)");
            generateBody(input_stream, output_stream);

            // Note that the type-separate glue does not support REAL types
            const char* expected = PREFIX POSTFIX "/// The size of the array of input variables.\r\n\
const std:::uint16_t OPLCGLUE_INPUT_SIZE(11);\r\n\
GlueVariable oplc_input_vars[11] = {\r\n\
    { IEC_REAL, __ID0 },\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_UNASSIGNED, nullptr },\r\n\
    { IEC_REAL, __ID10 },\r\n\
};\r\n\r\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single INT at %MW2") {
            std::stringstream input_stream("__LOCATED_VAR(INT,__MW2,M,W,2)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == PREFIX "\tint_memory[2] = __MW2;\r\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT);
        }

        WHEN("Contains single DWORD at %MD0") {
            std::stringstream input_stream("__LOCATED_VAR(DWORD,__MD2,M,D,2)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == PREFIX "\tdint_memory[2] = (IEC_DINT *)__MD2;\r\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT);
        }

        WHEN("Contains single LINT at %ML1") {
             std::stringstream input_stream("__LOCATED_VAR(LINT,__ML1,M,L,1)");
             generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == PREFIX "\tlint_memory[1] = (IEC_LINT *)__ML1;\r\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT);
        }

        WHEN("Contains single LINT at %ML1024") {
            std::stringstream input_stream("__LOCATED_VAR(LINT,__ML1024,M,L,1024)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == PREFIX "\tspecial_functions[0] = (IEC_LINT *)__ML1024;\r\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT);
        }
    }
}
