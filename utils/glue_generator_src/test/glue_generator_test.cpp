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
            REQUIRE(output_stream.str() == "\tbool_input[0][0] = __IX0;\r\n");
        }

        WHEN("Contains single BOOL at %QX0") {
            std::stringstream input_stream("__LOCATED_VAR(BOOL,__QX0,Q,X,0)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tbool_output[0][0] = __QX0;\r\n");
        }

        WHEN("Contains single BYTE at %IB0") {
            std::stringstream input_stream("__LOCATED_VAR(BYTE,__IB0,I,B,0)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tbyte_input[0] = __IB0;\r\n");
        }

        WHEN("Contains single SINT at %IB1") {
            std::stringstream input_stream("__LOCATED_VAR(SINT,__IB1,I,B,1)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tbyte_input[1] = __IB1;\r\n");
        }

        WHEN("Contains single SINT at %QB1") {
            std::stringstream input_stream("__LOCATED_VAR(SINT,__QB1,Q,B,1)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tbyte_output[1] = __QB1;\r\n");
        }

        WHEN("Contains single USINT at %IB2") {
            std::stringstream input_stream("__LOCATED_VAR(USINT,__IB2,I,B,2)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tbyte_input[2] = __IB2;\r\n");
        }

        WHEN("Contains single WORD at %IW0") {
            std::stringstream input_stream("__LOCATED_VAR(WORD,__IW0,I,W,0)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tint_input[0] = __IW0;\r\n");
        }

        WHEN("Contains single WORD at %QW0") {
            std::stringstream input_stream("__LOCATED_VAR(WORD,__QW0,Q,W,0)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tint_output[0] = __QW0;\r\n");
        }

        WHEN("Contains single INT at %IW1") {
            std::stringstream input_stream("__LOCATED_VAR(INT,__IW1,I,W,1)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tint_input[1] = __IW1;\r\n");
        }

        WHEN("Contains single UINT at %IW2") {
            std::stringstream input_stream("__LOCATED_VAR(UINT,__IW2,I,W,2)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tint_input[2] = __IW2;\r\n");
        }

        WHEN("Contains single INT at %MW2") {
            std::stringstream input_stream("__LOCATED_VAR(INT,__MW2,M,W,2)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tint_memory[2] = __MW2;\r\n");
        }

        WHEN("Contains single DWORD at %MD0") {
            std::stringstream input_stream("__LOCATED_VAR(DWORD,__MD2,M,D,2)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tdint_memory[2] = (IEC_DINT *)__MD2;\r\n");
        }

        WHEN("Contains single LINT at %ML1") {
             std::stringstream input_stream("__LOCATED_VAR(LINT,__ML1,M,L,1)");
             generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tlint_memory[1] = (IEC_LINT *)__ML1;\r\n");
        }

        WHEN("Contains single LINT at %ML1024") {
            std::stringstream input_stream("__LOCATED_VAR(LINT,__ML1024,M,L,1024)");
            generateBody(input_stream, output_stream);
            REQUIRE(output_stream.str() == "\tspecial_functions[0] = (IEC_LINT *)__ML1024;\r\n");
        }
    }
}
