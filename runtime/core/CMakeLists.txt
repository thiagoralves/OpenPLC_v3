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

cmake_minimum_required(VERSION 3.0.0)

if (UNIX)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive")
endif()

# This project builds only the files that are in the core runtime
# directory that defines our library. We normally use a higher
# level CMake that coordinates a larger build process

# The lib directory contains the MATIEC files that we need
include_directories(lib)

# The primary source is everything in this directory
file(GLOB oplc_SRC *.cpp dnp3s/*.cpp)

message("In runtime")
message(${oplc_SRC})

# For other part of the build, it is useful to know what are the valid
# files without the hardware layer.
set(oplc_nohw_SRC oplc_SRC)
list(REMOVE_ITEM oplc_nohw_SRC "hardware_layer.cpp")
