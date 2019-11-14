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

#ifndef CORE_INI_UTIL_H_
#define CORE_INI_UTIL_H_

/** \addtogroup openplc_runtime
 *  @{
 */

#include <cstring>
#include <istream>

/// Convert a boolean value in the INI file to a boolean.
/// The value must be "true", otherwise it is interpreted as false.
/// @param value the value to convert.
/// @return The converted value.
inline bool ini_atob(const char* value) {
    return strcmp("true", value) == 0;
}

/// Is the section and value equal to the expected section and value?
/// @param section_expected The expected section.
/// @param value_expected The expected value.
/// @param section The current section.
/// @param value The current value.
/// @return true if both the section and value match, otherwise false.
inline bool ini_matches(const char* section_expected,
                        const char* value_expected,
                        const char* section,
                        const char* value) {
    return strcmp(section_expected, section) == 0
        && strcmp(value_expected, value) == 0;
}

/// Implementation for fgets based on istream.
/// @param str pointer to an array of chars where the string read is copied.
/// @param num Maximum number of characters to be copied into str.
/// @param stream The stream object. The string must be null terminated.
/// @return the string or null if cannot read more.
static char* istream_fgets(char* str, int num, void* stream) {
    auto st = reinterpret_cast<std::istream*>(stream);
    if (!st->good() || st->eof()) {
        // We previously reached the end of the file, so return the end signal.
        return nullptr;
    }

    st->getline(str, num);

    return str;
}

/** @}*/

#endif  // CORE_INI_UTIL_H_
