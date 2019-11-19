//-----------------------------------------------------------------------------
// Copyright 2019 Thiago Alves
// Copyright 2019 Smarter Grid Solutions
// This file is part of the OpenPLC Software Stack.
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
//
// This file is responsible for the persistent storage on the OpenPLC
// Thiago Alves, Jun 2019
//-----------------------------------------------------------------------------

#include <ini.h>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <istream>
#include <fstream>
#include <mutex>
#include <thread>
#include <type_traits>

#include "glue.h"
#include "ini_util.h"
#include "ladder.h"
#include "pstorage.h"
#include "lib/iec_types_all.h"

/** \addtogroup openplc_runtime
 *  @{
 */

using namespace std;

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

/// A file header that must be present for the persistence file. This header
// defines a few pieces of information as indicated below besides the obvious
// "OPLCPS" (OpenPLC Persistent Storage) header.
const char FILE_HEADER[] = { (char)137, 'O', 'P', 'L', 'C', 'P', 'S', '\n', 'v', 0, '\n' };
//                                 |                                             |
//                       non-printing char                       version # of file format
//                       to avoid accidentally matching          allows 255 versions
//                       a text file

#define OPLC_PERSISTENT_STORAGE_MAX_SIZE 1024

// The size of the file header
const size_t FILE_HEADER_SIZE(extent<decltype(FILE_HEADER)>::value);

/// Get the size of the particular location in bytes.
/// @param size the size type.
/// @return The required storage size in number of bytes.
inline uint8_t get_size_bytes(IecLocationSize size) {
    switch (size) {
        case IECLST_BIT:
            return 1;
        case IECLST_BYTE:
            return 1;
        case IECLST_WORD:
            return 2;
        case IECLST_DOUBLEWORD:
            return 4;
        case IECLST_LONGWORD:
            return 8;
    }

    return 0;
}

/// Get the total number of bytes required to store the bindings.
/// @param bindings The bindings that we want to store.
/// @return The total number of bytes required.
size_t get_size_bytes(const GlueVariablesBinding& bindings) {
    size_t size(0);

    for (uint16_t index(0); index < bindings.size; ++index) {
        const GlueVariable& glue = bindings.glue_variables[index];

        if (glue.dir != IECLDT_MEM) {
            // We only care about items that are stored in memory
            continue;
        }

        size += get_size_bytes(glue.size);
    }

    return size;
}

inline uint8_t mask_index(GlueBoolGroup* group, uint8_t i) {
    if (!group->values[i]) {
        return 0;
    }

    return (*group->values) ? (1 << i) : 0;
}

inline void set_index(GlueBoolGroup* group, uint8_t i, uint8_t v) {
    if (group->values[i]) {
        (*group->values[i]) = ((1 << i) & v) ? TRUE: FALSE;
    }
}

/// Copy the glue values into the buffer.
/// @param bindings The bindings that we want to copy from.
/// @param buffer The buffer that we are copying into.
/// @return The number of bytes that were written into the buffer.
size_t pstorage_copy_glue(const GlueVariablesBinding& bindings, char* buffer) {
    lock_guard<mutex> guard(*bindings.buffer_lock);

    size_t num_written(0);
    for (uint16_t index(0); index < bindings.size; ++index) {
        const GlueVariable& glue = bindings.glue_variables[index];

        if (glue.dir != IECLDT_MEM) {
            // We only care about items that are stored in memory
            continue;
        }

        uint8_t num_bytes = get_size_bytes(glue.size);

        if (glue.size == IECLST_BIT) {
            GlueBoolGroup* group = reinterpret_cast<GlueBoolGroup*>(glue.value);
            uint8_t bools_as_byte = mask_index(group, 0)
             | mask_index(group, 1)
             | mask_index(group, 2)
             | mask_index(group, 3)
             | mask_index(group, 4)
             | mask_index(group, 5)
             | mask_index(group, 6)
             | mask_index(group, 7);
            memcpy(buffer, &bools_as_byte, 1);
        } else {
            // Write the number of bytes to the buffer
            memcpy(buffer, glue.value, num_bytes);
        }

        // Advance the pointer to the next starting position
        num_written += num_bytes;
        buffer += num_bytes;
    }

    return num_written;
}

/// Container for reading in configuration from the config.ini
/// This is populated with values from the config file.
struct PstorageConfig {
    PstorageConfig() :
        poll_interval(std::chrono::seconds(10))
    {}
    std::chrono::seconds poll_interval;
};

int pstorage_cfg_handler(void* user_data, const char* section,
                         const char* name, const char* value) {
    if (strcmp("pstorage", section) != 0) {
        return 0;
    }

    auto config = reinterpret_cast<PstorageConfig*>(user_data);

    if (strcmp(name, "poll_interval") == 0) {
        // We do not allow a poll period of less than 1 second as that
        // might cause lock contention problems.
        config->poll_interval = std::chrono::seconds(max(1, atoi(value)));
    } else if (strcmp(name, "enabled") == 0) {
        // Nothing to do here - we already know this is enabled
    } else {
        spdlog::warn("Unknown configuration item {}", name);
        return -1;
    }

    return 0;
}

int8_t pstorage_run(oplc::config_stream& cfg_stream,
                    const char* cfg_overrides,
                    const GlueVariablesBinding& bindings,
                    volatile bool& run,
                    function<std::ostream*(void)> stream_fn) {
    PstorageConfig config;
    ini_parse_stream(oplc::istream_fgets, cfg_stream.get(),
                     pstorage_cfg_handler, &config);

    // We are done with the file, so release the unique ptr. Normally this
    // will close the reference to the file
    cfg_stream.reset(nullptr);

    if (strlen(cfg_overrides) > 0) {
        config.poll_interval = std::chrono::seconds(max(1, atoi(cfg_overrides)));
    }

    const char endianness_header[2] = { IS_BIG_ENDIAN, '\n'};

    // This isn't ideal because we really only need enough space for
    // the located variables that are memory items, and this is all, but
    // it does ensure we have enough space.
    char buffer[OPLC_PERSISTENT_STORAGE_MAX_SIZE] = {0};
    // We keep a second block of memory so that we can detect if the values
    // have changed and not write if they are the same.
    char buffer_old[OPLC_PERSISTENT_STORAGE_MAX_SIZE] = {0};

    // If the required size from bindings is greater than the configured
    // size, then just exit
    if (get_size_bytes(bindings) > extent<decltype(buffer)>::value) {
        spdlog::error("Stored variables too large for persistent storage");
        return -1;
    }

    while (run) {
        size_t num_written = pstorage_copy_glue(bindings, buffer);

        if (memcmp(buffer, buffer_old, num_written) != 0) {
            // Try to open the file to do the initial write
            unique_ptr<ostream> out_stream(stream_fn());
            if (!out_stream) {
                spdlog::error("Unable to open persistent storage file for writing");
                return -2;
            }
            out_stream->write(FILE_HEADER, FILE_HEADER_SIZE);
            out_stream->write(endianness_header, 2);
            out_stream->write(bindings.checksum, strlen(bindings.checksum));
            out_stream->put('\n');
            out_stream->write(buffer, num_written);

            spdlog::info("Persistent storage updated");

            // We should be able to avoid this memory copy entirely
            memcpy(buffer_old, buffer, num_written);
        } else {
            spdlog::debug("Skip persistent write because unchanged values");
        }

        // Since we just wrote, we sleep.
        // TODO this needs a new mechanism for sleeping because this can
        // delay shutdown/stop if polling is long.
        this_thread::sleep_for(config.poll_interval);
    }

    spdlog::debug("Persistent storage ending normally");

    return 0;
}

inline int8_t read_and_check(istream& input_stream, const char header[],
                             char buffer[], size_t count) {
    if (!input_stream.read(buffer, count)) {
        spdlog::warn("Unable to read header from persistence file stream");
        return -1;
    }

    if (memcmp(header, buffer, count) != 0) {
        spdlog::warn("Header does not match expected in persistence file ");
        return -2;
    }

    return 0;
}

int8_t pstorage_read(istream& input_stream,
                     const GlueVariablesBinding& bindings) {
    // Read the file header - we define the file header as a constant that
    // must be present as the header. We don't allow UTF BOMs here.
    char header_check[FILE_HEADER_SIZE];
    if (read_and_check(input_stream, FILE_HEADER, header_check, FILE_HEADER_SIZE) != 0) {
        return -1;
    }

    // Check endianness of the written file
    char endianness_expected[2] = { IS_BIG_ENDIAN, '\n'};
    char endianness_check[2];
    if (read_and_check(input_stream, endianness_expected, header_check, 2) != 0) {
        return -2;
    }

    // We have a digest in the header to try to prevent accidentally using
    // the wrong persistence file for a particular runtime.
    char checksum_check[32];
    if (read_and_check(input_stream, bindings.checksum, checksum_check, 32) != 0) {
        return -3;
    }

    // Just add one newline character
    char padding_expected[1] = { '\n' };
    char padding_check[1];
    if (read_and_check(input_stream, padding_expected, padding_check, 1) != 0) {
        return -4;
    }

    // Now we know that the format is right, so read in the rest. We read
    // variable by variable so that we can assign into the right value.
    for (uint16_t index(0); index < bindings.size; ++index) {
        const GlueVariable& glue = bindings.glue_variables[index];

        if (glue.dir != IECLDT_MEM) {
            // We only care about items that are stored in memory
            continue;
        }

        uint8_t num_bytes;
        switch (glue.size) {
            case IECLST_BIT:
                num_bytes = 1;
                break;
            case IECLST_BYTE:
                num_bytes = 1;
                break;
            case IECLST_WORD:
                num_bytes = 2;
                break;
            case IECLST_DOUBLEWORD:
                num_bytes = 4;
                break;
            case IECLST_LONGWORD:
                num_bytes = 8;
                break;
            default:
                spdlog::error("Unexpected glue variable type {}", glue.size);
                return -5;
        }

        // Read the required number of bytes from the stream
        // 8 here is the maximum buffer size that we need based on
        // the types that we support.
        char buffer[8];
        if (!input_stream.read(buffer, num_bytes)) {
            spdlog::error("Persistent storage file too short; partially read");
            return -6;
        }

        // Assign the value into the glue value. The value is ether a simple
        // value or a group of booleans.
        // We don't actually care what the contents are - we just populate as
        // though they are raw bytes
        if (glue.size == IECLST_BIT) {
            GlueBoolGroup* group = reinterpret_cast<GlueBoolGroup*>(glue.value);
            uint8_t value = static_cast<uint8_t>(buffer[0]);
            set_index(group, 0, value);
            set_index(group, 1, value);
            set_index(group, 2, value);
            set_index(group, 3, value);
            set_index(group, 4, value);
            set_index(group, 5, value);
            set_index(group, 6, value);
            set_index(group, 7, value);
        } else {
            memcpy(glue.value, buffer, num_bytes);
        }
    }

    spdlog::info("Initialized from persistent storage");

    return 0;
}

void pstorage_service_init(const GlueVariablesBinding& binding) {
    ifstream stream("persistent.file", ios::binary);
    if (!stream) {
        spdlog::info("Skipped load persistence because file cannot be read.");
        return;
    }

    auto result = pstorage_read(stream, binding);
    spdlog::info("Storage read completed with result {}", result);
}

void pstorage_service_finalize(const GlueVariablesBinding& binding) {
    // We don't current do anything on finalize (although we probably should)
}

void pstorage_service_run(const GlueVariablesBinding& binding,
                          volatile bool& run, const char* config) {
    // We don't allow a poll duration of less than one second otherwise
    // that can have detrimental effects on performance
    auto create_stream = []() {
        return new ofstream("persistent.file", ios::binary);
    };

    auto cfg_stream = oplc::open_config();
    pstorage_run(cfg_stream, config, binding, run, create_stream);
}

/** @}*/
