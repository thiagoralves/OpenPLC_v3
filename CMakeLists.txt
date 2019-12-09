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

# CMake build for OpenPLC runtime.

project(openplc_project)

# If we are building libmodbus, we treat it as an external project
# so include that capability for cmake
include(ExternalProject)


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Choose a build type from: Debug Release RelWithDebInfo MinSizeRel"
    FORCE)
endif()

message("Build type is ${CMAKE_BUILD_TYPE}")

# Include settings that are specific to a particular target environment
include(${PROJECT_SOURCE_DIR}/cmake/settings.cmake)

# To build the entire application, we do code generation from ST to C. This
# is the name of the ST file that we are code generating from.
if(NOT program_name)
    set(program_name "blank_program.st")
endif()
message("User program = ${program_name}")

if(NOT USER_STSOURCE_DIR)
    set(USER_STSOURCE_DIR "../etc/st_files")
endif()
message("User program directory = ${USER_STSOURCE_DIR}")

# Enable building the application with different set of capabilties
# depending on the capabilities that we want.

option(OPLC_ALL "Build all optional capabilities" ON)

option(OPLC_ALL_SUPPORTING_TOOLS "Build required tools" OFF)
option(OPLC_MATIEC "Build MATIEC compiler" OFF)
option(OPLC_ST_OPTIMZER "Build ST optimizer" OFF)
option(OPLC_GLUE_GENERATOR "Build gluerator" OFF)

option(OPLC_ALL_SUPPORTING_LIBRARIES "Build required libraries" OFF)
option(OPLC_MODBUS "Enable Modbus" OFF)
option(OPLC_DNP3_OUTSTATION "Enable the DNP3 outstation" OFF)

option(OPLC_BUILD_USER_APPLICATION "Build the user application from ST" ON)
option(OPLC_ST_TO_C "Build target to convert ST to C" OFF)
option(OPLC_MAIN_PROGRAM "Only compile the ST file" OFF)

option(OPLC_BUILD_TESTS "Build automated tests" OFF)

# Flag to build all dependencies and source
if(OPLC_ALL)
    message("Enabling all optional components")
    set(OPLC_ALL_SUPPORTING_TOOLS ON)
    set(OPLC_ALL_SUPPORTING_LIBRARIES ON)
    set(OPLC_BUILD_USER_APPLICATION ON)
    set(OPLC_BUILD_TESTS ON)
endif()

# Flag to build the supporting tools
if(OPLC_ALL_SUPPORTING_TOOLS)
    message("Build supporting tools enabled")
    set(OPLC_MATIEC ON)
    set(OPLC_ST_OPTIMZER ON)
    set(OPLC_GLUE_GENERATOR ON)
endif()

# Flag to build the supporting tools
if(OPLC_ALL_SUPPORTING_LIBRARIES)
    message("Build supporting libraries enabled")
    set(OPLC_DNP3_OUTSTATION ON)
    set(OPLC_MODBUS ON)
endif()

# Flag to build the user application
if(OPLC_BUILD_USER_APPLICATION)
    message("Build main application enabled")
    set(OPLC_ST_TO_C ON)
    set(OPLC_MAIN_PROGRAM ON)
endif()

# For reasons that we haven't figured out yet, OpenDNP3 with OpenPLC doesn't compile
# nicely. If DNP3 is enabled, then disable it now if we are Cygwin
if(${CYGWIN})
    if (OPLC_DNP3_OUTSTATION)
        message(WARNING "DNP3 disabled for building on Cygwin")
        set(OPLC_DNP3_OUTSTATION OFF)
    endif()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

################################################################################
# Build definitions for the dependent tools (applications we need to call to
# build user projects, but not link in)
################################################################################

# If we have enabled the MATIEC, then build MATIEC. MATIEC is
# Makefile based, so include as an external project.
if(OPLC_MATIEC)
    # This whole matiec thing isn't nice, but it is how the project has
    # matiec directly included and I don't want to change that now
    message("Build and install MATIEC enabled")
    set(MATIEC_INSTALL_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    # Note that MMATIEC's output adds bin and lib directories, so we
    # want the install directory to be up one from the normal output
    # directory.
    ExternalProject_Add(matiec_proj
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils/matiec_src
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND COMMAND autoreconf -i COMMAND ./configure --prefix=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/..
        BUILD_COMMAND ${MAKE}
        COMMENT "MATIEC INSTALLED"
    )

    include_directories(${MATIEC_INSTALL_DIR}/include)
    link_directories(${MATIEC_INSTALL_DIR}/lib)

    add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/bin/iec2c${PLATFORM_EXTENSION}
        COMMAND ${CMAKE_COMMAND} -E echo "iec2c dummy target"
        DEPENDS matiec_proj)
endif()

# Compile glue generator
if(OPLC_GLUE_GENERATOR)
    message("Build and install GLUE GENERATOR enabled")
    # Build the glue generator into the bin folder
    add_subdirectory(utils/glue_generator_src)
endif()

# Compile the ST file optimizer
if(OPLC_ST_OPTIMZER)
    message("Build and install ST OPTIMIZER enabled")
    add_subdirectory(utils/st_optimizer_src)
endif()

# We need a custom target to define the build order so that we
# build these dependent tools before building the main application.
# We cannot depend on the subdirectory targets directly in most versions
# of CMake, so we depend on the generated files instead.
add_custom_target(dependent_tools ALL
    DEPENDS ${PROJECT_SOURCE_DIR}/bin/iec2c${PLATFORM_EXTENSION}
    DEPENDS ${PROJECT_SOURCE_DIR}/bin/st_optimizer${PLATFORM_EXTENSION}
    DEPENDS ${PROJECT_SOURCE_DIR}/bin/glue_generator${PLATFORM_EXTENSION}
)

################################################################################
# Build our dependent libraries - these get linked into the OpenPLC application
################################################################################

# If we have enabled the Modbus, then build Modbus. Modbus is
# Makefile based
if(OPLC_MODBUS)
    message("Build and install LIBMODBUS enabled")
    include_directories(utils/libmodbus_src/src)
    ExternalProject_Add(libmodbus_proj
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils/libmodbus_src
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./autogen.sh COMMAND ./configure --prefix=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/..
        BUILD_COMMAND ${MAKE}
        COMMENT "LIBMODBUS INSTALLED")
else()
    include_directories(utils/libmodbus_src/src)
endif()

# If we have enabled the DNP3 outstation, then build opendnp3. Opendnp3 is
# CMake based, so we can just add that as a subdirectory and everything will
# work nicely.
if(OPLC_DNP3_OUTSTATION)
    message("Build and install OpenDNP3 enabled")
    add_definitions(-DOPLC_DNP3_OUTSTATION)
    add_subdirectory(utils/dnp3_src)
    include_directories(utils/dnp3_src/cpp/libs/include)
endif()

################################################################################
# Build the OpenPLC application
################################################################################

# This defines the GLOB for compiling the runtime with user files
add_subdirectory(runtime)

if(OPLC_ST_TO_C)
    message("Generate C files enabled")

    # Dummy output rebuilded everytime. Needed for Config0.c and Res0.c files generation
    add_custom_command(OUTPUT dummy_output
        COMMAND ${CMAKE_COMMAND} -E make_directory ../etc/src
        COMMAND ./st_optimizer${PLATFORM_EXTENSION} ${USER_STSOURCE_DIR}/${program_name} ${USER_STSOURCE_DIR}/${program_name}
        COMMAND ./iec2c${PLATFORM_EXTENSION} -I ../runtime/lib -T ../etc/src ${USER_STSOURCE_DIR}/${program_name}
        COMMAND ./glue_generator${PLATFORM_EXTENSION} ../etc/src/LOCATED_VARIABLES.h ../etc/src/glueVars.cpp
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
        DEPENDS always_rebuild dependent_tools
    )

    # Always rebuild. Used for dependency creation and synchonization during compilation
    add_custom_command(OUTPUT always_rebuild
        COMMAND ${CMAKE_COMMAND} -E echo
    )

    # Matiec compilation is skipped for windows platform
    set(files_generator_deps "dummy_output")
    list(APPEND files_generator_deps "matiec_proj")

    add_custom_target(files_generator
        DEPENDS dummy_output
        BYPRODUCTS ${PROJECT_SOURCE_DIR}/etc/src/glueVars.cpp ${PROJECT_SOURCE_DIR}/etc/src/Config0.c ${PROJECT_SOURCE_DIR}/etc/src/Res0.c
        COMMAND echo "Generating glueVars..."
    )
endif()

if(OPLC_MAIN_PROGRAM)
    message("Compile main program enabled")

    set(OPLC_USER_DIR ${PROJECT_SOURCE_DIR}/etc/src)
    file(GLOB oplc_SRC runtime/core/*.cpp runtime/core/dnp3s/*.cpp runtime/core/modbusslave/*.cpp runtime/core/modbusmaster/*.cpp runtime/core/service/*.cpp runtime/vendor/inih-r46/*.c)

    include_directories(${OPLC_USER_DIR})
    include_directories(runtime/core)
    include_directories(runtime/core/lib)
    include_directories(runtime/vendor/spdlog-1.3.1)
    include_directories(runtime/vendor/inih-r46)
    include_directories(./include)

    # The makefile based project put their libs here
    link_directories(./lib)

    add_executable(openplc ${oplc_SRC} ${OPLC_USER_DIR}/Config0.c ${OPLC_USER_DIR}/Res0.c ${OPLC_USER_DIR}/glueVars.cpp)

    target_include_directories(openplc SYSTEM PUBLIC ${OPLC_CORE_DIR}/lib)

    set_target_properties(openplc PROPERTIES SUFFIX "")
    set_target_properties(openplc PROPERTIES OUTPUT_NAME "openplc")
    set_target_properties(openplc PROPERTIES PREFIX "")

    add_dependencies(openplc files_generator)
    if(OPLC_MODBUS)
        add_dependencies(openplc libmodbus_proj)
    endif()

    target_link_libraries(openplc ${OPLC_PTHREAD} modbus)
    if (OPLC_DNP3_OUTSTATION)
        target_compile_definitions(openplc PRIVATE OPLC_DNP3_OUTSTATION)
        target_link_libraries(openplc asiodnp3 asiopal opendnp3 openpal)
    endif()

    if(OPLC_PLATFORM_RPI)
        target_link_libraries(openplc wiringPi rt)
    endif()

    # -fpermissive will allow some nonconforming code to compile
    add_compile_options(openplc -fpermissive)

    file(WRITE ${PROJECT_SOURCE_DIR}/etc/active_program ${program_name})
endif()
