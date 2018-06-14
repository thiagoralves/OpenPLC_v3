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
#include "BufferTestObject.h"

#include <testlib/BufferHelpers.h>

#include <memory>
#include <stdexcept>

#include <testlib/HexConversions.h>

#include <openpal/util/ToHex.h>

using namespace openpal;

namespace testlib
{

BufferTestObject::BufferTestObject() :
	mNumWrites(0)
{

}

BufferTestObject::~BufferTestObject()
{

}

void BufferTestObject::ClearBuffer()
{
	mNumWrites = 0;
	mBuffer.clear();
}

bool BufferTestObject::BufferEquals(const openpal::RSlice& arBuffer) const
{
	return BufferEquals(arBuffer, arBuffer.Size());
}

bool BufferTestObject::BufferEquals(const uint8_t* apData, size_t aNumBytes) const
{

	if(aNumBytes != mBuffer.size()) return false;
	for(size_t i = 0; i < aNumBytes; i++)
		if(apData[i] != mBuffer[i])
		{
			return false;
		}
	return true;
}

bool BufferTestObject::BufferContains(const std::string& arPattern) const
{
	std::string s;
	for(size_t i = 0; i < mBuffer.size(); ++i)
	{
		std::string c(1, static_cast<char>(mBuffer[i]));
		s.append(c);
	}
	return s.find(arPattern) != std::string::npos;
}

std::string BufferTestObject::GetBufferAsHexString(bool spaced) const
{
	CopyableBuffer buffer(static_cast<uint32_t>(mBuffer.size()));
	for(size_t i = 0; i < mBuffer.size(); ++i) buffer[i] = mBuffer[i];
	return ToHex(buffer.ToRSlice(), spaced);
}


bool BufferTestObject::BufferEqualsHex(const std::string& arData) const
{
	HexSequence hs(arData);
	return BufferEquals(hs, hs.Size());
}

bool BufferTestObject::BufferEqualsString(const std::string& arData) const
{
	if(arData.size() != mBuffer.size()) return false;
	for(size_t i = 0; i < mBuffer.size(); i++)
		if(arData[i] != mBuffer[i])
		{
			return false;
		}
	return true;
}

void BufferTestObject::WriteToBuffer(const RSlice& input)
{
	if((mBuffer.size() + input.Size()) > MAX_SIZE )
	{
		throw std::invalid_argument("Max size exceeded");
	}
	else
	{
		++mNumWrites;

		for(size_t i = 0; i < input.Size(); ++i)
		{
			mBuffer.push_back(input[i]);
		}
	}
}

} //end namespace

