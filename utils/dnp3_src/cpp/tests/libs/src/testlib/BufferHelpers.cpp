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
#include "BufferHelpers.h"

#include <openpal/util/ToHex.h>

#include "HexConversions.h"

#include <memory.h>
#include <sstream>
#include <assert.h>
#include <algorithm>
#include <exception>
#include <stdexcept>

using namespace std;
using namespace openpal;

namespace testlib
{

ByteStr::ByteStr(uint32_t aLength, uint8_t aSeed) : CopyableBuffer(aLength)
{
	for(size_t i = 0; i < aLength; ++i) mpBuff[i] = static_cast<uint8_t>((i + aSeed) % 256);
}

ByteStr::ByteStr(const uint8_t* apData, uint32_t aLength) : CopyableBuffer(aLength)
{
	memcpy(mpBuff, apData, aLength);
}

ByteStr::ByteStr(const std::string& aChars) : CopyableBuffer(static_cast<uint32_t>(aChars.size()))
{
	memcpy(mpBuff, aChars.c_str(), aChars.size());
}

bool ByteStr::operator==(const ByteStr& arRHS) const
{
	if(Size() != arRHS.Size()) return false;

	for(size_t i = 0; i < Size(); ++i)
		if(mpBuff[i] != arRHS[i]) return false;

	return true;
}

std::string ByteStr::ToHex() const
{
	return testlib::ToHex(ToRSlice());
}

HexSequence::HexSequence( const std::string& aSequence) :
	ByteStr(Validate(RemoveSpaces(aSequence)))
{
	std::string s = RemoveSpaces(aSequence);

	size_t size = s.size();
	for(size_t index = 0, pos = 0; pos < size; ++index, pos += 2)
	{
		uint32_t val;
		std::stringstream ss;
		ss << std::hex << s.substr(pos, 2);
		if((ss >> val).fail())
		{
			throw std::invalid_argument(aSequence);
		}
		mpBuff[index] = static_cast<uint8_t>(val);
	}
}

std::string HexSequence::RemoveSpaces(const std::string& aSequence)
{
	std::string copy(aSequence);
	RemoveSpacesInPlace(copy);
	return copy;
}

void HexSequence::RemoveSpacesInPlace(std::string& s)
{
	size_t pos = s.find_first_of(' ');
	if(pos != string::npos)
	{
		s.replace(pos, 1, "");
		RemoveSpacesInPlace(s);
	}
}

uint32_t HexSequence::Validate(const std::string& s)
{
	//annoying when you accidentally put an 'O' instead of zero '0'
	if(s.find_first_of( "oO") != string::npos)
	{
		throw std::invalid_argument("Sequence contains 'o' or 'O'");
	}

	if(s.size() % 2 != 0)
	{
		throw std::invalid_argument(s);
	}

	return static_cast<uint32_t>(s.size() / 2);
}

}


