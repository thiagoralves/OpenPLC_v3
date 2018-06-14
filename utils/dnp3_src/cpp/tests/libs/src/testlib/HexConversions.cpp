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
#include "HexConversions.h"

#include "BufferHelpers.h"

#include <openpal/util/ToHex.h>
#include <openpal/container/Buffer.h>

#include <sstream>

using namespace std;
using namespace openpal;

namespace testlib
{

std::string ToHex(const uint8_t* apBuff, size_t aLength, bool spaced)
{
	std::ostringstream oss;
	size_t last = aLength - 1;
	for (size_t i = 0; i < aLength; i++)
	{
		char c = apBuff[i];
		oss << openpal::ToHexChar((c & 0xf0) >> 4) << openpal::ToHexChar(c & 0xf);
		if (spaced && i != last)oss << " ";
	}
	return oss.str();
}

std::string ToHex(const RSlice& buffer, bool spaced)
{
	return ToHex(buffer, buffer.Size(), spaced);
}

std::string ByteToHex(uint8_t b)
{
	std::ostringstream oss;
	oss << openpal::ToHexChar((b & 0xf0) >> 4) << openpal::ToHexChar(b & 0xf);
	return oss.str();
}

std::string AppendHex(std::initializer_list<std::string> segments)
{
	ostringstream oss;
	for (auto& str : segments)
	{
		oss << str;
	}
	HexSequence output(oss.str());
	return ToHex(output.ToRSlice());
}

std::string SkipBytesHex(const std::string& input, uint32_t bytes)
{
	HexSequence buffer(input);
	return ToHex(buffer.ToRSlice().Skip(bytes));
}

std::string RepeatHex(uint8_t byte, uint16_t count, bool spaced)
{
	Buffer buffer(count);
	buffer.GetWSlice().SetAllTo(byte);
	return ToHex(buffer.ToRSlice(), spaced);
}

std::string IncrementHex(uint8_t start, uint16_t count, bool spaced)
{
	Buffer buffer(count);

	for (uint16_t i = 0; i < count; ++i)
	{
		buffer[i] = start;
		++start;
	}

	return ToHex(buffer.ToRSlice(), spaced);
}

}

