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
#ifndef TESTLIB_COPYABLEBUFFER_H_
#define TESTLIB_COPYABLEBUFFER_H_

#include <stddef.h>
#include <memory>
#include <sstream>

#include <openpal/container/RSlice.h>

namespace testlib
{

/** Implements a dynamic buffer with a safe
	copy constructor. This makes it easier to compose with
	classes without requiring an explicit copy constructor
*/
class CopyableBuffer
{
	friend std::ostream& operator<<(std::ostream& output, const CopyableBuffer& arBuff);


public:
	// Construct null buffer
	CopyableBuffer();
	// Construct based on starting size of buffer
	CopyableBuffer(uint32_t aSize);
	CopyableBuffer(const openpal::RSlice&);
	CopyableBuffer(const uint8_t* apBuff, uint32_t aSize);
	CopyableBuffer(const CopyableBuffer&);
	CopyableBuffer& operator=(const CopyableBuffer&);
	~CopyableBuffer();

	bool operator==( const CopyableBuffer& other) const;
	bool operator!=( const CopyableBuffer& other) const
	{
		return ! (*this == other);
	}

	openpal::RSlice ToRSlice() const
	{
		return openpal::RSlice(mpBuff, mSize);
	}

	operator const uint8_t* () const
	{
		return mpBuff;
	}

	operator uint8_t* ()
	{
		return mpBuff;
	}

	uint32_t Size() const
	{
		return mSize;
	}

	void Zero();

protected:
	uint8_t* mpBuff;

private:
	uint32_t mSize;
};

}

#endif


