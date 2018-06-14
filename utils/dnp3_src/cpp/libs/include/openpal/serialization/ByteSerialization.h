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
#ifndef OPENPAL_BYTESERIALIZATION_H
#define OPENPAL_BYTESERIALIZATION_H

#include <cstdint>
#include <cstring>

#include "openpal/container/WSlice.h"
#include "openpal/container/RSlice.h"

namespace openpal
{

class UInt8Simple
{
public:

	inline static uint8_t Read(const uint8_t* pStart)
	{
		return (*pStart);
	}

	inline static uint8_t ReadBuffer(RSlice& buffer)
	{
		auto ret = Read(buffer);
		buffer.Advance(SIZE);
		return ret;
	}

	static void WriteBuffer(WSlice& buffer, uint8_t value)
	{
		Write(buffer, value);
		buffer.Advance(SIZE);
	}

	inline static void Write(uint8_t* pStart, uint8_t value)
	{
		*(pStart) = value;
	}

	const static size_t SIZE = 1;
	const static uint8_t Max;
	const static uint8_t Min;

	typedef uint8_t Type;
};

}

#endif
