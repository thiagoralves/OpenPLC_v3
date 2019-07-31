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

# This project just builds the hardware layers with the main library
# to validate that they produce an application.

# These hardware layers all depend on particular Linux headers being present
# which are not available for MINGW, so don't build these tests if we are not
# on a compliant system
if (UNIX)
    # We need the core directory for it's include files
    include_directories(../core)

    # Default application built for blank (Linux)
    add_library(oplcblank ${oplc_nohw_SRC} ../hardware_layers/blank.cpp)
    target_link_libraries(oplcblank LINK_PUBLIC asiodnp3 libmodbus.so ${OPLC_PTHREAD})

    # Default application built for Neuron
    add_library(oplcneuron ${oplc_nohw_SRC} ../hardware_layers/neuron.cpp)
    target_link_libraries(oplcneuron LINK_PUBLIC asiodnp3 libmodbus.so ${OPLC_PTHREAD})

    if (NOT ${CYGWIN})
        # Default application built for Pixtend
        add_library(oplcpixtend ${oplc_nohw_SRC} ../hardware_layers/pixtend.cpp)
        target_include_directories(oplcpixtend SYSTEM PUBLIC ../test/mock_headers)
        target_link_libraries(oplcpixtend LINK_PUBLIC asiodnp3 libmodbus.so ${OPLC_PTHREAD})

        # Default application built for Pixtend2L
        add_library(oplcpixtend2l ${oplc_nohw_SRC} ../hardware_layers/pixtend2l.cpp)
        target_include_directories(oplcpixtend2l SYSTEM PUBLIC ../test/mock_headers)
        target_link_libraries(oplcpixtend2l LINK_PUBLIC asiodnp3 libmodbus.so ${OPLC_PTHREAD})

        # Default application built for Pixtend2S
        add_library(oplcpixtend2s ${oplc_nohw_SRC} ../hardware_layers/pixtend2s.cpp)
        target_include_directories(oplcpixtend2s SYSTEM PUBLIC ../test/mock_headers)
        target_link_libraries(oplcpixtend2s LINK_PUBLIC asiodnp3 libmodbus.so ${OPLC_PTHREAD})
    endif()

    # Default application built for RaspberryPI
    add_library(oplcraspberrypi ${oplc_nohw_SRC} ../hardware_layers/raspberrypi.cpp)
    target_include_directories(oplcraspberrypi SYSTEM PUBLIC ../test/mock_headers)
    target_link_libraries(oplcraspberrypi LINK_PUBLIC asiodnp3 libmodbus.so ${OPLC_PTHREAD})

    # Default application built for RaspberryPI (Old)
    add_library(oplcraspberrypiold ${oplc_nohw_SRC} ../hardware_layers/raspberrypi_old.cpp)
    target_include_directories(oplcraspberrypiold SYSTEM PUBLIC ../test/mock_headers)
    target_link_libraries(oplcraspberrypiold LINK_PUBLIC asiodnp3 libmodbus.so ${OPLC_PTHREAD})

    # Default application built for Simulink
    add_library(oplcsimulink ${oplc_nohw_SRC} ../hardware_layers/simulink.cpp)
    target_include_directories(oplcsimulink SYSTEM PUBLIC ../test/mock_headers)
    target_link_libraries(oplcsimulink LINK_PUBLIC asiodnp3 libmodbus.so ${OPLC_PTHREAD})

    # Default application built for UniPI
    add_library(oplcunipi ${oplc_nohw_SRC} ../hardware_layers/unipi.cpp)
    target_include_directories(oplcunipi SYSTEM PUBLIC ../test/mock_headers)
    target_link_libraries(oplcunipi LINK_PUBLIC asiodnp3 libmodbus.so ${OPLC_PTHREAD})

    # Default application built for Fischertechnik 
    add_library(oplcfischertechnik ${oplc_nohw_SRC} ../hardware_layers/fischertechnik.cpp)
    target_include_directories(oplcfischertechnik SYSTEM PUBLIC ../test/mock_headers)
    target_link_libraries(oplcfischertechnik LINK_PUBLIC asiodnp3 libmodbus.so ${OPLC_PTHREAD})
endif()
