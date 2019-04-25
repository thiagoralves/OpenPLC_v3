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

// A simple application that defines the items normally that are provided
// by the glue generator and MATIEC. This allows us to do a simple check
// of the ability to compile everything together, which is useful since
// much of this is only determined when we have a structured text input file.

#include <cstdint>

#include "iec_std_lib.h"

TIME __CURRENT_TIME;
extern unsigned long long common_ticktime__;

#ifndef OPLC_IEC_GLUE_VALUE_TYPE
#define OPLC_IEC_GLUE_VALUE_TYPE
enum IecGlueValueType {
IECVT_BOOL,
IECVT_BYTE,
IECVT_SINT,
IECVT_USINT,
IECVT_INT,
IECVT_UINT,
IECVT_WORD,
IECVT_DINT,
IECVT_UDINT,
IECVT_DWORD,
IECVT_REAL,
IECVT_LREAL,
IECVT_LWORD,
IECVT_LINT,
IECVT_ULINT,
IECVT_UNASSIGNED,
};
#endif // OPLC_IEC_GLUE_VALUE_TYPE

#ifndef OPLC_GLUE_VARIABLE
#define OPLC_GLUE_VARIABLE
/// Defines the mapping for a glued variable.
struct GlueVariable {
    /// The type of the glue variable.
    IecGlueValueType type;
    /// A pointer to the memory address for reading/writing the value.
    void* value;
};
#endif // OPLC_GLUE_VARIABLE

//Internal buffers for I/O and memory. These buffers are defined in the
//auto-generated glueVars.cpp file
#define BUFFER_SIZE		1024

//Booleans
IEC_BOOL* bool_input[BUFFER_SIZE][8];
IEC_BOOL* bool_output[BUFFER_SIZE][8];

//Bytes
IEC_BYTE* byte_input[BUFFER_SIZE];
IEC_BYTE* byte_output[BUFFER_SIZE];

//Analog I/O
IEC_UINT* int_input[BUFFER_SIZE];
IEC_UINT* int_output[BUFFER_SIZE];

//Memory
IEC_UINT* int_memory[BUFFER_SIZE];
IEC_DINT* dint_memory[BUFFER_SIZE];
IEC_LINT* lint_memory[BUFFER_SIZE];

//Special Functions
IEC_LINT* special_functions[BUFFER_SIZE];

#define __LOCATED_VAR(type, name, ...) type __##name;
#include "LOCATED_VARIABLES.h"
#undef __LOCATED_VAR
#define __LOCATED_VAR(type, name, ...) type* name = &__##name;
#include "LOCATED_VARIABLES.h"
#undef __LOCATED_VAR

void glueVars()
{
}

/// The size of the array of input variables
extern std::uint16_t const OPLCGLUE_INPUT_SIZE(0);
GlueVariable oplc_input_vars[] = {
	{ IECVT_UNASSIGNED, nullptr },
};

/// The size of the array of output variables
extern std::uint16_t const OPLCGLUE_OUTPUT_SIZE(0);
GlueVariable oplc_output_vars[] = {
	{ IECVT_UNASSIGNED, nullptr },
};

void updateTime()
{
	__CURRENT_TIME.tv_nsec += common_ticktime__;

	if (__CURRENT_TIME.tv_nsec >= 1000000000)
	{
		__CURRENT_TIME.tv_nsec -= 1000000000;
		__CURRENT_TIME.tv_sec += 1;
	}
}
