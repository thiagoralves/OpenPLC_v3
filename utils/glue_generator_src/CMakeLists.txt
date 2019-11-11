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

project(glue_generator LANGUAGES CXX VERSION 0.0.1)

# CMake build for OpenPLC glue generator. The glue generator takes
# generated C code from the MATIEC compiler and then generates necessary
# glue to bind the MATIEC code with the OpenPLC runtime.

option(WERROR "Set all warnings to errors" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(vendor/catch2-2.7.0)

# The main application that we always build
add_executable(glue_generator glue_generator.cpp md5.c)

if (OPLC_BUILD_TESTS)
    add_executable(gg_unit_test test/glue_generator_test.cpp md5.c)
    enable_testing()
    add_test(NAME glue_generator_test COMMAND gg_unit_test)
endif()
