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
#include <iostream>

// Our application is defined in a single CPP file, so to write tests against it
// we need to include it directly here. That's ok so long as the application is
// no more than one CPP file.
#define OPLCGLUE_OMIT_MAIN
#include "../glue_generator.cpp"

using namespace std;

#define PREFIX "void glueVars()\n{\n"
#define POSTFIX "}\n\n"
#define EMPTY_INPUT "/// The size of the array of input variables.\n\
const std::uint16_t OPLCGLUE_INPUT_SIZE(0);\n\
GlueVariable oplc_input_vars[] = {\n\
};\n\n"
#define EMPTY_OUTPUT "/// The size of the array of output variables.\n\
const std::uint16_t OPLCGLUE_OUTPUT_SIZE(0);\n\
GlueVariable oplc_output_vars[] = {\n\
};\n\n"


SCENARIO("Commmand line", "[main]") {
    GIVEN("<no pre-conditions>") {
        WHEN("-h command line arguments") {
            const char* args[2] = { "glue_generator", "-h" };
            REQUIRE(mainImpl(2, args) == 0);
        }

        WHEN("--help command line arguments") {
            const char* args[2] = { "glue_generator", "-h" };
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

            const char* expected = PREFIX "\tbool_input[0][0] = __IX0;\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single BOOL at %QX0") {
            std::stringstream input_stream("__LOCATED_VAR(BOOL,__QX0,Q,X,0)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == PREFIX "\tbool_output[0][0] = __QX0;\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT);
        }

        WHEN("Contains single BYTE at %IB0") {
            std::stringstream input_stream("__LOCATED_VAR(BYTE,__IB0,I,B,0)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX"\tbyte_input[0] = __IB0;\n" POSTFIX "/// The size of the array of input variables.\n\
const std::uint16_t OPLCGLUE_INPUT_SIZE(1);\n\
GlueVariable oplc_input_vars[] = {\n\
    { IECVT_BYTE, __IB0 },\n\
};\n\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single SINT at %IB1") {
            std::stringstream input_stream("__LOCATED_VAR(SINT,__IB1,I,B,1)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tbyte_input[1] = __IB1;\n" POSTFIX "/// The size of the array of input variables.\n\
const std::uint16_t OPLCGLUE_INPUT_SIZE(2);\n\
GlueVariable oplc_input_vars[] = {\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_SINT, __IB1 },\n\
};\n\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single SINT at %QB1") {
            std::stringstream input_stream("__LOCATED_VAR(SINT,__QB1,Q,B,1)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tbyte_output[1] = __QB1;\n" POSTFIX EMPTY_INPUT "/// The size of the array of output variables.\n\
const std::uint16_t OPLCGLUE_OUTPUT_SIZE(2);\n\
GlueVariable oplc_output_vars[] = {\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_SINT, __QB1 },\n\
};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single USINT at %IB2") {
            std::stringstream input_stream("__LOCATED_VAR(USINT,__IB2,I,B,2)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tbyte_input[2] = __IB2;\n" POSTFIX "/// The size of the array of input variables.\n\
const std::uint16_t OPLCGLUE_INPUT_SIZE(3);\n\
GlueVariable oplc_input_vars[] = {\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_USINT, __IB2 },\n\
};\n\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single WORD at %IW0") {
            std::stringstream input_stream("__LOCATED_VAR(WORD,__IW0,I,W,0)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tint_input[0] = __IW0;\n" POSTFIX "/// The size of the array of input variables.\n\
const std::uint16_t OPLCGLUE_INPUT_SIZE(1);\n\
GlueVariable oplc_input_vars[] = {\n\
    { IECVT_WORD, __IW0 },\n\
};\n\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single WORD at %QW0") {
            std::stringstream input_stream("__LOCATED_VAR(WORD,__QW0,Q,W,0)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tint_output[0] = __QW0;\n" POSTFIX EMPTY_INPUT "/// The size of the array of output variables.\n\
const std::uint16_t OPLCGLUE_OUTPUT_SIZE(1);\n\
GlueVariable oplc_output_vars[] = {\n\
    { IECVT_WORD, __QW0 },\n\
};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single INT at %IW1") {
            std::stringstream input_stream("__LOCATED_VAR(INT,__IW1,I,W,1)");
            generateBody(input_stream, output_stream);
            const char* expected = PREFIX "\tint_input[1] = __IW1;\n" POSTFIX "/// The size of the array of input variables.\n\
const std::uint16_t OPLCGLUE_INPUT_SIZE(2);\n\
GlueVariable oplc_input_vars[] = {\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_INT, __IW1 },\n\
};\n\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single UINT at %IW2") {
            std::stringstream input_stream("__LOCATED_VAR(UINT,__IW2,I,W,2)");
            generateBody(input_stream, output_stream);

            const char* expected = PREFIX "\tint_input[2] = __IW2;\n" POSTFIX "/// The size of the array of input variables.\n\
const std::uint16_t OPLCGLUE_INPUT_SIZE(3);\n\
GlueVariable oplc_input_vars[] = {\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_UINT, __IW2 },\n\
};\n\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains two REAL at %ID0 and %ID10") {
            std::stringstream input_stream("__LOCATED_VAR(REAL,__ID0,I,D,0)\n__LOCATED_VAR(REAL,__ID10,I,D,10)");
            generateBody(input_stream, output_stream);

            // Note that the type-separate glue does not support REAL types
            const char* expected = PREFIX POSTFIX "/// The size of the array of input variables.\n\
const std::uint16_t OPLCGLUE_INPUT_SIZE(11);\n\
GlueVariable oplc_input_vars[] = {\n\
    { IECVT_REAL, __ID0 },\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_UNASSIGNED, nullptr },\n\
    { IECVT_REAL, __ID10 },\n\
};\n\n" EMPTY_OUTPUT;
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single INT at %MW2") {
            std::stringstream input_stream("__LOCATED_VAR(INT,__MW2,M,W,2)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == PREFIX "\tint_memory[2] = __MW2;\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT);
        }

        WHEN("Contains single DWORD at %MD0") {
            std::stringstream input_stream("__LOCATED_VAR(DWORD,__MD2,M,D,2)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == PREFIX "\tdint_memory[2] = (IEC_DINT *)__MD2;\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT);
        }

        WHEN("Contains single LINT at %ML1") {
             std::stringstream input_stream("__LOCATED_VAR(LINT,__ML1,M,L,1)");
             generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == PREFIX "\tlint_memory[1] = (IEC_LINT *)__ML1;\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT);
        }

        WHEN("Contains single LINT at %ML1024") {
            std::stringstream input_stream("__LOCATED_VAR(LINT,__ML1024,M,L,1024)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == PREFIX "\tspecial_functions[0] = (IEC_LINT *)__ML1024;\n" POSTFIX EMPTY_INPUT EMPTY_OUTPUT);
        }
    }
}
