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
# See the License for the specific language governing permissions and
# limitations under the License.

project(openplc_runtime)

cmake_minimum_required(VERSION 3.0.0)

# We always include the spdlog as an include directory
include_directories(vendor/spdlog-1.3.1)
# We also always need the DNP3 and Modbus libraries
include_directories(../utils/dnp3_src/cpp/libs/include)
include_directories(../utils/libmodbus_src/src)

# This builds the primary application library.
add_subdirectory(core)

if (OPLC_BUILD_TESTS)
    message("Building tests enabled")
    # Add the unit tests into the build.
    add_subdirectory(test)

    # Build an example application that should always succeed
    add_subdirectory(example)

    # This builds a number of saample applications that ensure that code changes
    # compile correctly across all hardware layers.
    add_subdirectory(integration_test)
endif()
