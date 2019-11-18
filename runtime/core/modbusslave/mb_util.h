// Copyright 2015 Thiago Alves
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

#ifndef CORE_MODBUSSLAVE_MB_UTIL_H_
#define CORE_MODBUSSLAVE_MB_UTIL_H_

#include <cstdint>

/** \addtogroup openplc_runtime
 *  @{
 */

inline std::int16_t mb_to_word(std::uint8_t byte1, std::uint8_t byte2) {
	std::int16_t returnValue = (std::int16_t)(byte1 << 8) | (std::int16_t)byte2;
	return returnValue;
}

inline std::uint8_t mb_low_byte(std::uint16_t w) {
    return (std::uint8_t) ((w) & 0xff);
}

inline std::uint8_t mb_high_byte(std::uint16_t w) {
    return (std::uint8_t) ((w) >> 8);
}

/** @}*/

#endif  // CORE_MODBUSSLAVE_MB_UTIL_H_
