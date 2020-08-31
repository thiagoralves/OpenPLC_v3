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
    absyntax_utils.cc 
    add_en_eno_param_decl.cc 
    decompose_var_instance_name.cc 
    array_dimension_iterator.cc 
    case_element_iterator.cc 
    function_call_iterator.cc 
    function_call_param_iterator.cc 
    function_param_iterator.cc 
    get_sizeof_datatype.cc 
    get_var_name.cc 
    search_il_label.cc 
    search_base_type.cc 
    search_fb_instance_decl.cc 
    search_fb_typedecl.cc 
    search_varfb_instance_type.cc 
    search_var_instance_decl.cc 
    spec_init_separator.cc 
    type_initial_value.cc 
    search_varfb_instance_type.cc 
    search_var_instance_decl.cc 
    spec_init_separator.cc 
    type_initial_value.cc 
    debug_ast.cc 
    get_datatype_info.cc
)

add_library(absyntax_utils STATIC ${SOURCES})
set_property(TARGET absyntax_utils PROPERTY CXX_STANDARD 98)
