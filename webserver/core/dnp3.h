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

#ifndef CORE_DNP3_H
#define CORE_DNP3_H

#include <cstdint>
#include <istream>
#include <sstream>

/// Defines an offest mapping for DNP3 to glue variables.
/// This structure allows you to specify valid ranges for
/// the glue mapping. This could allow you to divide up
/// the glue into multiple outstations.
struct Dnp3Range {
    std::uint16_t inputs_offset;
    std::uint16_t inputs_start;
    std::uint16_t inputs_end;

    std::uint16_t outputs_offset;
    std::uint16_t outputs_start;
    std::uint16_t outputs_end;

    std::uint16_t bool_inputs_offset;
    std::uint16_t bool_inputs_start;
    std::uint16_t bool_inputs_end;

    std::uint16_t bool_outputs_offset;
    std::uint16_t bool_outputs_start;
    std::uint16_t bool_outputs_end;

    std::string ToString() {
        std::stringstream ss;
        ss << "DNP3 range (S,E,O) " << "I: " << this->inputs_start << ' ' << this->inputs_end << ' ' << this->inputs_offset;
        ss << "; O: " << this->outputs_start << ' ' << this->outputs_end << ' ' << this->outputs_offset;
        ss << "; BI: " << this->bool_inputs_start << ' ' << this->bool_inputs_end << ' ' << this->bool_inputs_offset;
        ss << "; BO: " << this->bool_outputs_start << ' ' << this->bool_outputs_end << ' ' << this->bool_outputs_offset;
        return ss.str();
    }
};

#endif // CORE_DNP3_H
