/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */
#ifndef OPENDNP3_SHIFTABLEBUFFER_H
#define OPENDNP3_SHIFTABLEBUFFER_H

#include <openpal/container/WSlice.h>
#include <openpal/container/RSlice.h>

namespace opendnp3
{


/** @section DESCRIPTION
		Implements a buffer that can shift its contents as it is read */
class ShiftableBuffer
{
public:

	/**
	 * Construct the facade over the specified underlying buffer
	 */
	ShiftableBuffer(uint8_t* pBuffer_, uint32_t size);


	// ------- Functions related to reading -----------

	uint32_t NumBytesRead() const
	{
		return writePos - readPos;
	}

	/// @return Pointer to the next byte to be read in the buffer
	openpal::RSlice ReadBuffer() const
	{
		return openpal::RSlice(pBuffer + readPos, NumBytesRead());
	}

	/// Signal that some bytes don't have to be stored any longer. They'll be recovered during the next shift operation.
	void AdvanceRead(uint32_t aNumBytes);

	// ------- Functions related to writing -----------

	/// Shift the buffer back to front, writing over bytes that have already been read. The objective
	/// being to free space for further writing.
	void Shift();

	/// Reset the buffer to its initial state, empty
	void Reset();

	/// @return Bytes of available for writing
	uint32_t NumWriteBytes() const
	{
		return M_SIZE - writePos;
	}

	/// @return Pointer to the position in the buffer available for writing
	uint8_t* WriteBuff() const
	{
		return pBuffer + writePos;
	}

	/// Signal to the buffer bytes were written to the current write position
	void AdvanceWrite(uint32_t numBytes);

	////////////////////////////////////////////
	// Other functions
	////////////////////////////////////////////


	/// Searches the read subsequence for 0x0564 sync bytes
	/// @return true if both sync bytes were found in the buffer.
	bool Sync(uint32_t& skipCount);

private:



	uint8_t* pBuffer;
	const uint32_t M_SIZE;
	uint32_t writePos;
	uint32_t readPos;
};

}

#endif
