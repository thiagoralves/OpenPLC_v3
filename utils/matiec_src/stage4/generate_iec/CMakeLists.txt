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

set(SOURCES
    generate_iec.cc
    ../stage4.cc
)

add_library(stage4_iec STATIC ${SOURCES})
target_include_directories(stage4_iec PRIVATE ../../../absyntax)
set_property(TARGET stage4_iec PROPERTY CXX_STANDARD 98)