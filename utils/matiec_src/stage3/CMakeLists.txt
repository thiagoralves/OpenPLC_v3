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
    stage3.cc
    flow_control_analysis.cc
    fill_candidate_datatypes.cc
    narrow_candidate_datatypes.cc
    forced_narrow_candidate_datatypes.cc
    print_datatypes_error.cc
    datatype_functions.cc
    lvalue_check.cc
    array_range_check.cc
    case_elements_check.cc
    constant_folding.cc
    declaration_check.cc
    enum_declaration_check.cc
    remove_forward_dependencies.cc
)

add_library(stage3 STATIC ${SOURCES})
target_include_directories(stage3 PRIVATE ../)
set_property(TARGET stage3 PROPERTY CXX_STANDARD 98)