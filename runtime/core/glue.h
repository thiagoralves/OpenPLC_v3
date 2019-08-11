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

#ifndef CORE_GLUE_H
#define CORE_GLUE_H

#include <cstdint>
#include <memory>
#include <mutex>

#include "iec_types.h"

/** \addtogroup openplc_runtime
 *  @{
 */

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif


#ifndef OPLC_IEC_GLUE_VALUE_TYPE
#define OPLC_IEC_GLUE_VALUE_TYPE

/// @brief Defines the type of a glue variable (so that we can read and
/// write). This definition must be consistent with what is produced
/// by the /// Defines the type of a glue variable (so that we can read and
/// write). This definition must be consistent with what is produced
/// by the @ref glue_generator.
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
    IECVT_UNASSIGNED
};
#endif // OPLC_IEC_GLUE_VALUE_TYPE

#ifndef OPLC_GLUE_VARIABLE
#define OPLC_GLUE_VARIABLE

//////////////////////////////////////////////////////////////////////////////////
/// @brief Defines the mapping for a glued variable.
///
/// This definition must be consistent with what is produced by the @ref glue_generator.
//////////////////////////////////////////////////////////////////////////////////
struct GlueVariable {
    /// The type of the glue variable.
    IecGlueValueType type;
    /// A pointer to the memory address for reading/writing the value.
    void* value;
};
#endif // OPLC_GLUE_VARIABLE

//////////////////////////////////////////////////////////////////////////////////
/// @brief Defines a collection of buffers by which we exchange data
/// between the runtime and peripherals/middleware.
///
/// Except for the boolean buffers, each is an array of the same length
/// where each value points to a memory location for data
/// exchange. That is, the values of the arrays only indirectly
/// indicate the held value.
///
/// In the case of booleans, there is a further level of indirection
/// in that each location has as many as 8 child bits.
///
/// Access to the buffers is protected by a common mutex. You need
/// to acquire the lock prior to reading or writing from the buffers.
///
/// Inputs are normally only populated by hardwired devices. They
/// are inputs to the runtime. Outputs are normally populated by
/// the runtime and are available as outputs from the runtime. The
/// precise use of these conventions ultimately depends on the
/// implementation of the communcation driver.
///
/// A value that is not mapped is marked with NULL and must not be used.
//////////////////////////////////////////////////////////////////////////////////
struct GlueVariables {

	GlueVariables(
		std::mutex* buffer_lock,
		IEC_BOOL** bool_inputs,
		IEC_BOOL** bool_outputs,
		std::uint16_t inputs_size,
		GlueVariable* inputs,
		std::uint16_t outputs_size,
		GlueVariable* outputs) :

		buffer_lock(buffer_lock),
		bool_inputs(bool_inputs),
		bool_outputs(bool_outputs),
		inputs_size(inputs_size),
		inputs(inputs),
		outputs_size(outputs_size),
		outputs(outputs)
	{}

	GlueVariables(const GlueVariables& copy) :

		buffer_lock(copy.buffer_lock),
		bool_inputs(copy.bool_inputs),
		bool_outputs(copy.bool_outputs),
		inputs_size(copy.inputs_size),
		inputs(copy.inputs),
		outputs_size(copy.outputs_size),
		outputs(copy.outputs)
	{}

    /// @brief Mutex for this structure
    std::mutex* buffer_lock;

    // Booleans - these are mapped separately because they have an additional
    // level of nesting.

    /// Mapped to IXx_x locations. This is a a 2D array where the second index
	/// must have a length of 8.
    IEC_BOOL** bool_inputs;

    /// Mapped to QXx_x locations. This is a a 2D array where the second index
	/// must have a length of 8.
    IEC_BOOL** bool_outputs;

	/// @brief Gets the boolean input at the specified primary and secondary index.
	/// @param prim The primary (first) index in the 2-D array
	/// @param sec The secondary index in the 2-D array.
	/// @return A pointer to the boolean value.
	inline IEC_BOOL* BoolInputAt(std::uint16_t prim, std::uint16_t sec) {
		std::uint16_t idx = prim * 8 + sec;
		return this->bool_inputs[idx];
	}

	/// @brief Gets the boolean output at the specified primary and secondary index.
	/// @param prim The primary (first) index in the 2-D array
	/// @param sec The secondary index in the 2-D array.
	/// @return A pointer to the boolean value.
	inline IEC_BOOL* BoolOutputAt(std::uint16_t prim, std::uint16_t sec) {
		std::uint16_t idx = prim * 8 + sec;
		return this->bool_outputs[idx];
	}

    /// @brief The size of the inputs array.  You can index up to inputs_size - 1
    /// in the array.
    const std::uint16_t inputs_size;

    /// @brief The input glue variables array. The number of items in the array is
    /// given by inputs_size.
    GlueVariable* const inputs;

    /// @brief The size of the outputs array. You can index up to outputs_size - 1
    /// in the array.
    const std::uint16_t outputs_size;

    /// @brief The output glue variables array. The number of items in the array is
    /// given by outputs_size.
    GlueVariable* const outputs;
};

#endif // CORE_GLUE_H

/** @}*/
