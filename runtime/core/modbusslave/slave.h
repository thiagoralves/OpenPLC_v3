// Copyright 2015 Thiago Alves
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

#ifndef CORE_MODBUSSLAVE_SLAVE_H_
#define CORE_MODBUSSLAVE_SLAVE_H_

/** \addtogroup openplc_runtime
 *  @{
 */

class GlueVariablesBinding;

/// @brief Process an individual modbus message.
int process_modbus_message(unsigned char *buffer, int buffer_size,
                           void* user_data);

/// @brief Start the modbus slave server.
///
/// @param glue_variables The glue variables that may be bound into this
///                       server.
/// @param run A signal for running this server. This server terminates when
///            this signal is false.
/// @param config The custom configuration for this service.
void modbus_slave_service_run(const GlueVariablesBinding& binding,
                              volatile bool& run, const char* config);

/** @}*/

#endif  // CORE_MODBUSSLAVE_SLAVE_H_
