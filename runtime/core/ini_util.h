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

#ifndef RUNTIME_CORE_INI_UTIL_H_
#define RUNTIME_CORE_INI_UTIL_H_

/** \addtogroup openplc_runtime
 *  @{
 */

#include <cstring>
#include <functional>
#include <fstream>
#include <istream>
#include <memory>

namespace oplc
{

/// Convert a boolean value in the INI file to a boolean.
/// The value must be "true", otherwise it is interpreted as false.
/// @param value the value to convert.
/// @return The converted value.
inline bool ini_atob(const char* value)
{
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
                        const char* value)
{
    return strcmp(section_expected, section) == 0
        && strcmp(value_expected, value) == 0;
}

/// @brief Compare a INI declaration name that has a postfix with an index,
/// for example "example.1". This returns 0 if the provided name matches
/// the expected value and there is a postfix. Returns the postfix
/// number to the caller.
///
/// The intention of this function is to create a standard mechanism to
/// declare configuration items that are indexed. The index is normally
/// used to lookup in an array for the slot where to populate the value.
///
/// @param name The name to test. This is the value read from the INI file.
/// @param expected The name to test against, without the separating period or
/// a marker for the index.
/// @param index If the return value of this function is 0, then this will
/// contain the index that was read as the postfix.
/// @param max_index The maximum index this will return. If the read index
/// is greater than this value, then this will return non-zero.
/// This maximum is designed to prevent unintentionally allocating far more
/// space than would be expected under normal use.
/// @return 0 if there is a match, otherwise non-zero.
inline int strcmp_with_id(const char* name, const char* expected,
                          std::uint8_t max_index,
                          std::uint8_t* index) {
    size_t expected_len = strlen(expected);
    int ret = strncmp(name, expected, expected_len);
    if (ret != 0) {
        return ret;
    }

    size_t name_len = strlen(name);
    if (name_len >= expected_len + 2 && name[expected_len] == '.') {
        auto read_index = atoi(name + (expected_len + 1));
        if (read_index <= max_index) {
            *index = read_index;
            return 0;
        }
    }

    return -1;
}

/// @brief Implementation for fgets based on istream.
/// @param str pointer to an array of chars where the string read is copied.
/// @param num Maximum number of characters to be copied into str.
/// @param stream The stream object. The string must be null terminated.
/// @return the string or null if cannot read more.
static char* istream_fgets(char* str, int num, void* stream)
{
    auto st = reinterpret_cast<std::istream*>(stream);
    if (!st->good() || st->eof())
    {
        // We previously reached the end of the file, so return the end signal.
        return nullptr;
    }

    st->getline(str, num);

    return str;
}

typedef std::unique_ptr<std::istream, std::function<void(std::istream*)>> config_stream;

/// Open the standard configuration file as an closable stream.
/// @return A stream for the configuration file.
inline config_stream open_config()
{
    return config_stream(
            new std::ifstream("../etc/config.ini"),
            [] (std::istream* s)
            {
                reinterpret_cast<std::ifstream*>(s)->close();
                delete s;
            }
        );
}

}  // namespace oplc

/** @}*/

#endif  // RUNTIME_CORE_INI_UTIL_H_
