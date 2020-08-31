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

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # macOS bison is version 2.3, too old for our needs
    # install a newer version with brew install bison
    set(BISON_EXECUTABLE "/usr/local/opt/bison/bin/bison" CACHE PATH "Bison executable")
endif()

find_package(BISON 2.4 REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(matiec_parser iec_bison.yy ${CMAKE_CURRENT_BINARY_DIR}/iec_bison.cc)

FLEX_TARGET(matiec_lexer iec_flex.ll ${CMAKE_CURRENT_BINARY_DIR}/iec_flex.cc)

set(SOURCES
    create_enumtype_conversion_functions.cc
    stage1_2.cc 
)

add_library(stage1_2 STATIC ${SOURCES} ${BISON_matiec_parser_OUTPUTS} ${FLEX_matiec_lexer_OUTPUTS})

target_include_directories(stage1_2 PRIVATE
    ../../absyntax
)

target_compile_definitions(stage1_2 PRIVATE
    DEFAULT_LIBDIR="lib" 
    YY_BUF_SIZE=65536
)

set_property(TARGET stage1_2 PROPERTY CXX_STANDARD 98)