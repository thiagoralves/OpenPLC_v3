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
#include <catch.hpp>
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
extern const std::size_t OPLCGLUE_INPUT_SIZE(0);\n\
GlueVariable oplc_input_vars[] = {\n\
};\n\n"
#define EMPTY_OUTPUT "/// The size of the array of output variables.\n\
extern const std::size_t OPLCGLUE_OUTPUT_SIZE(0);\n\
GlueVariable oplc_output_vars[] = {\n\
};\n\n"
#define EMPTY_INPUT_BOOL "/// Size of the array of input boolean variables.\n\
extern const std::size_t OPLCGLUE_INPUT_BOOL_SIZE(0);\n\
GlueBoolGroup oplc_bool_input_vars[] = {\n\
};\n\n"
#define EMPTY_OUTPUT_BOOL "/// Size of the array of output boolean variables.\n\
extern const std::size_t OPLCGLUE_OUTPUT_BOOL_SIZE(0);\n\
GlueBoolGroup oplc_bool_output_vars[] = {\n\
};\n\n"
#define GLUE_PREFIX "/// The size of the array of glue variables.\n"


SCENARIO("Command line", "[main]") {
    GIVEN("<no pre-conditions>") {
        WHEN("-h command line arguments") {
            const char* args[2] = { "glue_generator", "-h" };
            REQUIRE(main_impl(2, args) == 0);
        }

        WHEN("--help command line arguments") {
            const char* args[2] = { "glue_generator", "--help" };
            REQUIRE(main_impl(2, args) == 0);
        }
    }
}

SCENARIO("", "") {
    GIVEN("IO as streams") {
        std::stringstream output_stream;
        md5_byte_t digest[16];

        WHEN("Contains single BOOL at %IX0.8 is invalid index") {
            std::stringstream input_stream("__LOCATED_VAR(BOOL,__IX0_8,I,X,0,8)");
            auto result = generate_body(input_stream, output_stream, digest);
            REQUIRE(result != 0);
        }

        WHEN("Contains single BOOL at %IX0") {
            std::stringstream input_stream("__LOCATED_VAR(BOOL,__IX0_0,I,X,0)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tbool_input[0][0] = __IX0_0;\n" POSTFIX
                "GlueBoolGroup ___IG0 { .index=0, .values={ __IX0_0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, } };\n"
                "GlueBoolGroup* __IG0(&___IG0);\n"
                GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_IN, IECLST_BIT, 0, 0, IECVT_BOOL,  __IG0 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single BOOL at %QX0") {
            std::stringstream input_stream("__LOCATED_VAR(BOOL,__QX0,Q,X,0)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tbool_output[0][0] = __QX0;\n" POSTFIX
                "GlueBoolGroup ___QG0 { .index=0, .values={ __QX0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, } };\n"
                "GlueBoolGroup* __QG0(&___QG0);\n"
                GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_OUT, IECLST_BIT, 0, 0, IECVT_BOOL,  __QG0 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains multiple BOOL at %QX0") {
            std::stringstream input_stream("__LOCATED_VAR(BOOL,__QX0_0,Q,X,0,0)\n__LOCATED_VAR(BOOL,__QX0_2,Q,X,0,2)\n__LOCATED_VAR(BOOL,__QX1_3,Q,X,1,3)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tbool_output[0][0] = __QX0_0;\n\tbool_output[0][2] = __QX0_2;\n\tbool_output[1][3] = __QX1_3;\n" POSTFIX
                "GlueBoolGroup ___QG0 { .index=0, .values={ __QX0_0, nullptr, __QX0_2, nullptr, nullptr, nullptr, nullptr, nullptr, } };\n"
                "GlueBoolGroup* __QG0(&___QG0);\n"
                "GlueBoolGroup ___QG1 { .index=1, .values={ nullptr, nullptr, nullptr, __QX1_3, nullptr, nullptr, nullptr, nullptr, } };\n"
                "GlueBoolGroup* __QG1(&___QG1);\n"
                GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(2);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_OUT, IECLST_BIT, 0, 0, IECVT_BOOL,  __QG0 },\n"
                "    { IECLDT_OUT, IECLST_BIT, 1, 0, IECVT_BOOL,  __QG1 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single BYTE at %IB0") {
            std::stringstream input_stream("__LOCATED_VAR(BYTE,__IB0,I,B,0)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX"\tbyte_input[0] = __IB0;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_IN, IECLST_BYTE, 0, 0, IECVT_BYTE,  __IB0 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single SINT at %IB1") {
            std::stringstream input_stream("__LOCATED_VAR(SINT,__IB1,I,B,1)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tbyte_input[1] = __IB1;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_IN, IECLST_BYTE, 1, 0, IECVT_SINT,  __IB1 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single SINT at %QB1") {
            std::stringstream input_stream("__LOCATED_VAR(SINT,__QB1,Q,B,1)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tbyte_output[1] = __QB1;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_OUT, IECLST_BYTE, 1, 0, IECVT_SINT,  __QB1 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single USINT at %IB2") {
            std::stringstream input_stream("__LOCATED_VAR(USINT,__IB2,I,B,2)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tbyte_input[2] = __IB2;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_IN, IECLST_BYTE, 2, 0, IECVT_USINT,  __IB2 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single WORD at %IW0") {
            std::stringstream input_stream("__LOCATED_VAR(WORD,__IW0,I,W,0)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tint_input[0] = __IW0;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_IN, IECLST_WORD, 0, 0, IECVT_WORD,  __IW0 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single WORD at %QW0") {
            std::stringstream input_stream("__LOCATED_VAR(WORD,__QW0,Q,W,0)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tint_output[0] = __QW0;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_OUT, IECLST_WORD, 0, 0, IECVT_WORD,  __QW0 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single INT at %IW1") {
            std::stringstream input_stream("__LOCATED_VAR(INT,__IW1,I,W,1)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tint_input[1] = __IW1;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_IN, IECLST_WORD, 1, 0, IECVT_INT,  __IW1 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single UINT at %IW2") {
            std::stringstream input_stream("__LOCATED_VAR(UINT,__IW2,I,W,2)");
            generate_body(input_stream, output_stream, digest);

            const char* expected = PREFIX "\tint_input[2] = __IW2;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_IN, IECLST_WORD, 2, 0, IECVT_UINT,  __IW2 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains two REAL at %ID0 and %ID10") {
            std::stringstream input_stream("__LOCATED_VAR(REAL,__ID0,I,D,0)\n__LOCATED_VAR(REAL,__ID10,I,D,10)");
            generate_body(input_stream, output_stream, digest);

            // Note that the type-separate glue does not support REAL types
            const char* expected = PREFIX POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(2);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_IN, IECLST_DOUBLEWORD, 0, 0, IECVT_REAL,  __ID0 },\n"
                "    { IECLDT_IN, IECLST_DOUBLEWORD, 10, 0, IECVT_REAL,  __ID10 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single INT at %MW2") {
            std::stringstream input_stream("__LOCATED_VAR(INT,__MW2,M,W,2)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tint_memory[2] = __MW2;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_MEM, IECLST_WORD, 2, 0, IECVT_INT,  __MW2 },\n"
                "};\n\n";
             REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single DWORD at %MD0") {
            std::stringstream input_stream("__LOCATED_VAR(DWORD,__MD2,M,D,2)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tdint_memory[2] = (IEC_DINT *)__MD2;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_MEM, IECLST_DOUBLEWORD, 2, 0, IECVT_DWORD,  __MD2 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single LINT at %ML1") {
            std::stringstream input_stream("__LOCATED_VAR(LINT,__ML1,M,L,1)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tlint_memory[1] = (IEC_LINT *)__ML1;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_MEM, IECLST_LONGWORD, 1, 0, IECVT_LINT,  __ML1 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }

        WHEN("Contains single LINT at %ML1024") {
            std::stringstream input_stream("__LOCATED_VAR(LINT,__ML1024,M,L,1024)");
            generate_body(input_stream, output_stream, digest);
            const char* expected = PREFIX "\tspecial_functions[0] = (IEC_LINT *)__ML1024;\n" POSTFIX GLUE_PREFIX
                "extern const std::size_t OPLCGLUE_GLUE_SIZE(1);\n"
                "/// The packed glue variables.\n"
                "extern const GlueVariable oplc_glue_vars[] = {\n"
                "    { IECLDT_MEM, IECLST_LONGWORD, 1024, 0, IECVT_LINT,  __ML1024 },\n"
                "};\n\n";
            REQUIRE(output_stream.str() == expected);
        }
    }
}
