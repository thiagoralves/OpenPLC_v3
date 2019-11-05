// Copyright 2019 Smarter Grid Solutions
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.

#include <cstdlib>
#include <string>

#include "glue.h"
#include "ladder.h"

/// @brief Locates a partiular glue variable from the list of all glue
/// variables.
/// @param dir The direction of the variable.
/// @param size The size of the variable.
/// @param msi The most significant index of the variable.
/// @param lsi The least significant index of the variable (only relevant
/// for boolean types).
/// @return The variable, or nullptr if there is no such variable.
const GlueVariable* GlueVariablesBinding::find(IecLocationDirection dir,
                                               IecLocationSize size,
                                               std::uint16_t msi,
                                               std::uint8_t lsi) const {
    for (std::uint16_t i = 0; i < this->size; ++i) {
        const GlueVariable& cur_var = glue_variables[i];
        if (cur_var.dir == dir && cur_var.size == size && cur_var.msi == msi && cur_var.lsi == lsi) {
            return &glue_variables[i];
        }
    }

    return nullptr;
}

const GlueVariable* GlueVariablesBinding::find(const std::string& location) const {
    if (location.length() < 4 || location[0] != '%') {
        return nullptr;
    }

    IecLocationDirection direction;
    switch (location[1]) {
        case 'I':
            direction = IECLDT_IN;
            break;
        case 'Q':
            direction = IECLDT_OUT;
            break;
        case 'M':
            direction = IECLDT_MEM;
            break;
        default:
            return nullptr;
    }

    IecLocationSize size;
    switch (location[2]) {
        case 'X':
            size = IECLST_BIT;
            break;
        case 'B':
            size = IECLST_BYTE;
            break;
        case 'W':
            size = IECLST_WORD;
            break;
        case 'D':
            size = IECLST_DOUBLEWORD;
            break;
        case 'L':
            size = IECLST_LONGWORD;
            break;
        default:
            return nullptr;
    }

    char* end_msi;
    long msi = strtol(location.c_str() + 3, &end_msi, 10);

    // Do we have more characters left in the string to read for lsi?
    std::size_t start_lsi = end_msi + 1 - location.c_str();
    if (start_lsi >= location.length()) {
        find(direction, size, msi, 0);
    }

    char* end_lsi;
    long lsi = strtol(end_msi + 1, &end_lsi, 10);

    return find(direction, size, msi, lsi);
}
