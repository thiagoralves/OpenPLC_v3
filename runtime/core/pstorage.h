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

#ifndef CORE_PSTORAGE_H_
#define CORE_PSTORAGE_H_

#include <cstdint>
#include <chrono>
#include <functional>
#include <memory>

/** \addtogroup openplc_runtime
 *  @{
 */

class GlueVariablesBinding;

/// @brief Initialize persistent storage when the runtime starts.
///
/// @param glue_variables The glue variables that may be bound into this
///                       server.
void pstorage_service_init(const GlueVariablesBinding& binding);

/// @brief Finalize persistent storage after the runtime completes.
///
/// @param glue_variables The glue variables that may be bound into this
///                       server.
void pstorage_service_finalize(const GlueVariablesBinding& binding);

/// @brief Start the persistent storage server.
///
/// @param glue_variables The glue variables that may be bound into this
///                       server.
/// @param run A signal for running this server. This server terminates when
///            this signal is false.
/// @param config The custom configuration for this service.
void pstorage_service_run(const GlueVariablesBinding& binding,
                          volatile bool& run, const char* config);

/// @brief Reads the contents from the input stream into OpenPLC internal
/// buffers. The input stream must represent a file that was previously
/// written to persistent storage for the same project.
///
/// This function does not lock buffer access and therefore it is only safe
/// to call before the PLC loop begins (during bootstrap).
///
/// @note This is defined here so that we can access it from the unit tests.
///
/// @param input_stream The stream to read from. This must be opened for
/// binary read.
/// @param bindings The glue bindings to populate.
/// @return Zero on success, otherwise non-zero. This function may fail
/// part way through. Failure does not mean no variables have been set.
std::int8_t pstorage_read(std::istream& input_stream,
                          const GlueVariablesBinding& bindings);

/// @brief Run the persistent storage loop.
///
/// @note This is defined here so that we can access it from the unit tests.
///
/// @param cfg_stream An input stream to read configuration information
///                   from. This will be reset once use of the stream has
///                   been completed.
/// @param custom_config Additional configuration information that we process.
/// @param bindings The glue bindings to use.
/// @param run A flag that indicates if we should terminate the process.
/// @return Zero on success, otherwise non-zero. This function may fail
/// part way through. Failure does not mean no variables have been set.
std::int8_t pstorage_run(std::unique_ptr<std::istream, std::function<void(std::istream*)>>& cfg_stream,
                         const char* custom_config,
                         const GlueVariablesBinding& bindings,
                         volatile bool& run,
                         std::function<std::ostream*(void)> stream_fn);

/** @}*/

#endif  // CORE_PSTORAGE_H_
