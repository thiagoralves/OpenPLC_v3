//-----------------------------------------------------------------------------
// Copyright 2019 Thiago Alves
// Copyright 2019 Garret Fick
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

// This file contains the structured used by enip.cpp to process
// EtherNet/IP requests.
// UAH, Sep 2019
//-----------------------------------------------------------------------------

#ifndef RUNTIME_CORE_ENIP_ENIP_H_
#define RUNTIME_CORE_ENIP_ENIP_H_

#include <cstdint>

/** \addtogroup openplc_runtime
 *  @{
 */

class GlueVariablesBinding;

/// @brief Start the enip server.
///
/// @param glue_variables The glue variables that may be bound into this
///                       server.
/// @param run A signal for running this server. This server terminates when
///            this signal is false.
/// @param config The custom configuration for this service.
void enip_service_run(const GlueVariablesBinding& binding,
                              volatile bool& run, const char* config);

std::int16_t enip_process_message(unsigned char *buffer, std::int16_t buffer_size, void* user_data);
std::uint16_t processPCCCMessage(unsigned char *buffer, int buffer_size);

/** @} */

#endif  // RUNTIME_CORE_ENIP_ENIP_H_
