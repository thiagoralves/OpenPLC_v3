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

# This project builds a simple example application
if (NOT ${CYGWIN})
    include_directories(../core/lib)

    file(GLOB oplc_example_SRC *.cpp)
    file(GLOB oplc_SRC ../core/*.cpp)
    add_executable(oplcexample ${oplc_example_SRC} ${oplc_SRC})

    target_link_libraries(oplcexample pthread ${LIBMODBUS})
    if(OPLC_DNP3_OUTSTATION)
        target_link_libraries(oplcexample asiodnp3 asiopal opendnp3 openpal)
    endif()
endif()
