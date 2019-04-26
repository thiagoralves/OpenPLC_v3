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

#ifndef TEST_GLUE_TEST_HELPERS_H
#define TEST_GLUE_TEST_HELPERS_H

#include <cstdint>
#include <mutex>

#define TEST_BUFFER_SIZE 1024

/// Allocate a pointer to a new glue structure for the specific data type.
/// Initializes the values in the structure to the specified initial value.
/// @param buffer_size The size of the buffer to allocate.
/// @param start The first index to assign a default value
/// @param end One past the index to assign a default value.
/// @param default_value the default value to assign to members.
static inline GlueVariable* glue_alloc(std::uint16_t buffer_size, std::uint16_t start = 0, std::uint16_t stop = 0) {
    GlueVariable* buffer = new GlueVariable[buffer_size];

    for (auto i = 0; i < buffer_size; ++i) {
        buffer[i].type = IECVT_UNASSIGNED;
        buffer[i].value = nullptr;
    }

    return buffer;
}

/// Allocate a pointer to a new glue structure for the boolean type - the boolean type has
/// an additional level of nesting in comparision to other types.
/// Initializes the values in the structure to the specified initial value.
/// @param buffer_size The size of the buffer to allocate.
/// @param start The first index to assign a default value
/// @param end One past the index to assign a default value.
/// @param default_value the default value to assign to members.
static inline IEC_BOOL** glue_boolean_alloc(std::uint16_t buffer_size, std::uint16_t start = 0, std::uint16_t stop = 0) {
    IEC_BOOL** buffer = new IEC_BOOL*[buffer_size * 8];

    for (auto i = 0; i < buffer_size * 8; ++i) {
        if (i >= start && i < stop) {
            buffer[i] = new IEC_BOOL;
        }
        else {
            buffer[i] = nullptr;
        }
    }

    return buffer;
}

/// Free a glue structure that was previously allocated by glue_alloc.
/// @param buffer The buffer to free.
/// @param buffer_size The size of the buffer to free.
static inline void glue_free(GlueVariable* buffer, std::uint16_t buffer_size) {
    for (auto i = 0; i < buffer_size; ++i) {
		void* value_ptr = buffer[i].value;
		switch (buffer[i].type) {
			case IECVT_BOOL:
				delete reinterpret_cast<IEC_BOOL*>(value_ptr);
				break;
			case IECVT_BYTE:
				delete reinterpret_cast<IEC_BYTE*>(value_ptr);
				break;
			case IECVT_SINT:
				delete reinterpret_cast<IEC_SINT*>(value_ptr);
				break;
			case IECVT_USINT:
				delete reinterpret_cast<IEC_USINT*>(value_ptr);
				break;
			case IECVT_INT:
				delete reinterpret_cast<IEC_INT*>(value_ptr);
				break;
			case IECVT_UINT:
				delete reinterpret_cast<IEC_UINT*>(value_ptr);
				break;
			case IECVT_WORD:
				delete reinterpret_cast<IEC_WORD*>(value_ptr);
				break;
			case IECVT_DINT:
				delete reinterpret_cast<IEC_DINT*>(value_ptr);
				break;
			case IECVT_UDINT:
				delete reinterpret_cast<IEC_UDINT*>(value_ptr);
				break;
			case IECVT_DWORD:
				delete reinterpret_cast<IEC_DWORD*>(value_ptr);
				break;
			case IECVT_REAL:
				delete reinterpret_cast<IEC_REAL*>(value_ptr);
				break;
			case IECVT_LREAL:
				delete reinterpret_cast<IEC_LREAL*>(value_ptr);
				break;
			case IECVT_LWORD:
				delete reinterpret_cast<IEC_LWORD*>(value_ptr);
				break;
			case IECVT_LINT:
				delete reinterpret_cast<IEC_LINT*>(value_ptr);
				break;
			case IECVT_ULINT:
				delete reinterpret_cast<IEC_ULINT*>(value_ptr);
				break;
			case IECVT_UNASSIGNED:
				break;
		}
    }
    delete[] buffer;
}

/// Free a glue structure that was previously allocated by glue_boolean_alloc.
/// @param buffer The buffer to free.
/// @param buffer_size The size of the buffer to free.
static inline void glue_boolean_free(IEC_BOOL** buffer, std::uint16_t buffer_size) {
    for (auto i = 0; i < buffer_size * 8; ++i) {
        if (buffer[i] != nullptr) {
            delete buffer[i];
        }
    }
    delete[] buffer;
}

/// Creates an instance of the GlueVariables structure suitable for testing. The all children
/// are appropriately initialized with a buffer of a default size.
/// @return the allocated glue variables.
static inline std::shared_ptr<GlueVariables> make_vars() {
    //Booleans
    IEC_BOOL** bool_input = glue_boolean_alloc(TEST_BUFFER_SIZE, 0, TEST_BUFFER_SIZE);
    IEC_BOOL** bool_output = glue_boolean_alloc(TEST_BUFFER_SIZE, 0, TEST_BUFFER_SIZE);

    //Bytes
    GlueVariable* inputs = glue_alloc(TEST_BUFFER_SIZE, 0, TEST_BUFFER_SIZE);
    GlueVariable* outputs = glue_alloc(TEST_BUFFER_SIZE, 0, TEST_BUFFER_SIZE);

	auto mutex = new std::mutex;

    return std::shared_ptr<GlueVariables>(new GlueVariables{
        mutex,

        bool_input,
        bool_output,

        TEST_BUFFER_SIZE,
        inputs,
        TEST_BUFFER_SIZE,
        outputs,
        }, [](GlueVariables* vars) {
        // Specialize the destructor for the shared pointer so that this will
        // clean up everything on destruction.
			delete vars->buffer_lock;
			glue_boolean_free(vars->bool_inputs, TEST_BUFFER_SIZE);
			glue_boolean_free(vars->bool_outputs, TEST_BUFFER_SIZE);
			glue_free(vars->inputs, TEST_BUFFER_SIZE);
			glue_free(vars->outputs, TEST_BUFFER_SIZE);
			delete vars;
    });
}

#endif // TEST_GLUE_TEST_HELPERS_H
