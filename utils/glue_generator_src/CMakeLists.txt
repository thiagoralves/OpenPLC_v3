# Copyright 2019 Smarter Grid Solutions
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http ://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissionsand
# limitations under the License.

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
