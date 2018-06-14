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
#include "BitReader.h"

#include <openpal/Configure.h>
#include <assert.h>


namespace opendnp3
{

uint32_t NumBytesInBits(uint32_t numBits)
{
	uint32_t numBytes = numBits / 8;
	return ((numBits % 8) == 0) ? numBytes : numBytes + 1;
}

bool GetBit(const openpal::RSlice& buffer, uint32_t position)
{
	uint32_t byte = position / 8;
	uint32_t bit = position % 8;
	assert(byte < buffer.Size());
	return (buffer[byte] & (1 << bit)) != 0;
}

uint32_t NumBytesInDoubleBits(uint32_t numBits)
{
	uint32_t numBytes = numBits / 4;
	return ((numBits % 4) == 0) ? numBytes : numBytes + 1;
}

DoubleBit GetDoubleBit(const openpal::RSlice& buffer, uint32_t index)
{
	uint32_t byteNumber = index / 4;
	assert(byteNumber < buffer.Size());
	uint8_t byte = buffer[byteNumber];
	uint32_t bitshift = 2 * (index % 4);
	return DoubleBitFromType((byte >> bitshift) & 0x03);
}

}
