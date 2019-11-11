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
#include <iostream>
#include <memory>
#include <mutex>

#include "iec_types.h"

/** \addtogroup openplc_runtime
 *  @{
 */

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#ifndef OPLC_IEC_GLUE_DIRECTION
#define OPLC_IEC_GLUE_DIRECTION
enum IecLocationDirection {
    IECLDT_IN,
    IECLDT_OUT,
    IECLDT_MEM,
};
#endif  // OPLC_IEC_GLUE_DIRECTION

#ifndef OPLC_IEC_GLUE_SIZE
#define OPLC_IEC_GLUE_SIZE
enum IecLocationSize {
    /// Variables that are a single bit
    IECLST_BIT,
    /// Variables that are 1 byte
    IECLST_BYTE,
    /// Variables that are 2 bytes
    IECLST_WORD,
    /// Variables that are 4 bytes, including REAL
    IECLST_DOUBLEWORD,
    /// Variables that are 8 bytes, including LREAL
    IECLST_LONGWORD,
};
#endif  // OPLC_IEC_GLUE_SIZE

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
    /// This is not a normal type and won't appear in the glue variables
    /// here. But it does allow you to create your own indexed mapping
    /// and have a way to indicate a value that is not assigned a type.
    IECVT_UNASSIGNED
};
#endif // OPLC_IEC_GLUE_VALUE_TYPE

#ifndef OPLC_GLUE_BOOL_GROUP
#define OPLC_GLUE_BOOL_GROUP

//////////////////////////////////////////////////////////////////////////////////
/// @brief Defines the mapping for a group of glued boolean variable.
///
/// This definition must be consistent with what is produced by the @ref glue_generator.
//////////////////////////////////////////////////////////////////////////////////
struct GlueBoolGroup {
    /// The first index for this array. If we are iterating over the glue
    /// variables, then this index is superfluous, but it is very helpful
    /// for debugging.
    std::uint16_t index;
    /// The values in this group. If the value is not assigned, then the
    /// value at the index points to nullptr.
    IEC_BOOL* values[8];
};
#endif // OPLC_GLUE_BOOL_GROUP

#ifndef OPLC_GLUE_VARIABLE
#define OPLC_GLUE_VARIABLE


/// @brief Defines the mapping for a glued variable. This defines a simple,
/// space efficient lookup table. It has all of the mapping information that 
/// you need to find the variable based on the location name (e.g. %IB1.1).
/// While this is space efficient, this should be searched once to construct a
/// fast lookup into this table used for the remainder of the application
/// lifecycle.
///
/// This definition must be consistent with what is produced by the
/// @ref glue_generator.
struct GlueVariable {
    /// The direction of the variable - this is determined by I/Q/M.
    IecLocationDirection dir;
    /// The size of the variable - this is determined by X/B/W/D/L.
    IecLocationSize size;
    /// The most significant index for the variable. This is the part of the
    /// name, converted to an integer, before the period.
    std::uint16_t msi;
    /// The least significant index (sub-index) for the variable. This is the
    /// part of the name, converted to an integer, after the period. It is
    /// only relevant for boolean (bit) values.
    std::uint8_t lsi;
    /// The type of the glue variable. This is used so that we correctly
    /// write into the value type.
    IecGlueValueType type;
    /// A pointer to the memory address for reading/writing the value.
    void* value;
};
#endif  // OPLC_GLUE_VARIABLE

/// @brief Defines accessors for glue variables.
/// This structure wraps up items that are available as globals, but this
/// allows a straighforward way to inject definitions into tests, so it is
/// preferred to use this structure rather than globals.
struct GlueVariablesBinding {

    GlueVariablesBinding(std::mutex* buffer_lock, const std::uint16_t size,
                         const GlueVariable* glue_variables) :
        buffer_lock(buffer_lock),
        size(size),
        glue_variables(glue_variables)

    {}

    /// @brief Mutex for the glue variables
    std::mutex* buffer_lock;

    /// @brief The size of the glue variables array
    std::uint16_t size;

    /// @brief The glue variables array
    const GlueVariable* glue_variables;

    /// @brief Find a glue varia&glue_mutexble based on the specification of
    /// the variable.
    /// @return the variable or null if there is no variable that matches all
    /// criteria in the specification.
    const GlueVariable* find(IecLocationDirection dir,
                             IecLocationSize size,
                             std::uint16_t msi,
                             std::uint8_t lsi) const;

    /// @brief Find a glue variable based on the location of the variable, for
    /// example %IX0.1
    /// @return the variable or null if there is no variable that matches all
    /// criteria in the specification.
    const GlueVariable* find(const std::string& location) const;
};

#endif // CORE_GLUE_H

/** @}*/
