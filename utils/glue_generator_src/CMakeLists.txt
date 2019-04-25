cmake_minimum_required(VERSION 3.0.0)

# CMake build for OpenPLC glue generator. The glue generator takes
# generated C code from the MATIEC compiler and then generates necessary
# glue to bind the MATIEC code with the OpenPLC runtime.
project(openplc_gluegenerator)

option(WERROR "Set all warnings to errors" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Optional build capabilities
option(OPLCGLUE_ALL "Build all optional projects (tests)" ON)
option(OPLCGLUE_TEST "Build tests" OFF)

if(OPLCGLUE_ALL)
	message("enabling all optional components")
	set(OPLCGLUE_TEST ON)
endif()

# The main application that we always build
add_executable(glue_generator glue_generator.cpp)

#if(OPLCGLUE_TEST)
add_executable(glue_generator_test ./test/glue_generator_test.cpp)
#endif()
