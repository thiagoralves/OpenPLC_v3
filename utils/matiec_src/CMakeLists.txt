# Copyright 2020 Comm5 Tecnologia
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

# CMake build for OpenPLC runtime.

project(matiec)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Choose a build type from: Debug Release RelWithDebInfo MinSizeRel"
    FORCE)
endif()

set(PACKAGE_VERSION "0.1")
set(PACKAGE_BUGREPORT "")
set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PROJECT_NAME}")
set(PACKAGE_URL "")

add_subdirectory(stage1_2)
add_subdirectory(stage3)
add_subdirectory(stage4)
add_subdirectory(absyntax)
add_subdirectory(absyntax_utils)

include_directories(
    config
)

add_definitions(
    -DHGVERSION="None"
)

#
# Generate config.h file.
#
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.cmake
               ${CMAKE_CURRENT_SOURCE_DIR}/config/config.h)

add_executable(iec2c main.cc)
target_link_libraries(iec2c stage1_2 stage3 stage4_c absyntax absyntax_utils)
set_property(TARGET iec2c PROPERTY CXX_STANDARD 98)

add_executable(iec2iec main.cc)
target_link_libraries(iec2iec stage1_2 stage3 stage4_iec absyntax absyntax_utils)
set_property(TARGET iec2iec PROPERTY CXX_STANDARD 98)
