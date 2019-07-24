// Copyright 2015 Thiago Alves
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

// This file is the hardware layer for the OpenPLC. If you change the platform
// where it is running, you may only need to change this file. All the I/O
// related stuff is here. Basically it provides functions to read and write
// to the OpenPLC internal buffers in order to update I/O state.
// Thiago Alves, Dec 2015
//-----------------------------------------------------------------------------

#include <mutex>

#include "ladder.h"
#include "custom_layer.h"

//-----------------------------------------------------------------------------
// This function is called by the main OpenPLC routine when it is initializing.
// Hardware initialization procedures should be here.
//-----------------------------------------------------------------------------
void initializeHardware()
{
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Input state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersIn()
{
	std::lock_guard<std::mutex> lock(bufferLock); //lock mutex

	/*********READING AND WRITING TO I/O**************

	*bool_input[0][0] = read_digital_input(0);
	write_digital_output(0, *bool_output[0][0]);

	*int_input[0] = read_analog_input(0);
	write_analog_output(0, *int_output[0]);

	**************************************************/
}

//-----------------------------------------------------------------------------
// This function is called by the OpenPLC in a loop. Here the internal buffers
// must be updated to reflect the actual Output state. The mutex bufferLock
// must be used to protect access to the buffers on a threaded environment.
//-----------------------------------------------------------------------------
void updateBuffersOut()
{
	std::lock_guard<std::mutex> lock(bufferLock); //lock mutex

	/*********READING AND WRITING TO I/O**************

	*bool_input[0][0] = read_digital_input(0);
	write_digital_output(0, *bool_output[0][0]);

	*int_input[0] = read_analog_input(0);
	write_analog_output(0, *int_output[0]);

	**************************************************/
}

