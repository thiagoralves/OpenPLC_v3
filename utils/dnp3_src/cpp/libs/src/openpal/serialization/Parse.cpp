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
#include "openpal/serialization/Parse.h"

#include "openpal/serialization/Serialization.h"

namespace openpal
{
template <class Serializer>
bool ParseType(RSlice& input, typename Serializer::Type& output)
{
	if (input.Size() < Serializer::SIZE)
	{
		return false;
	}
	else
	{
		output = Serializer::ReadBuffer(input);
		return true;
	}
}

bool Parse::Read(RSlice& input, uint8_t& output)
{
	return ParseType<UInt8>(input, output);
}

bool Parse::Read(RSlice& input, uint16_t& output)
{
	return ParseType<UInt16>(input, output);
}

bool Parse::Read(RSlice& input, uint32_t& output)
{
	return ParseType<UInt32>(input, output);
}

bool Parse::Read(RSlice& input, UInt48Type& output)
{
	return ParseType<UInt48>(input, output);
}

bool Parse::Read(RSlice& input, int16_t& output)
{
	return ParseType<Int16>(input, output);
}

bool Parse::Read(RSlice& input, int32_t& output)
{
	return ParseType<Int32>(input, output);
}

bool Parse::Read(RSlice& input, double& output)
{
	return ParseType<DoubleFloat>(input, output);
}

bool Parse::Read(RSlice& input, float& output)
{
	return ParseType<SingleFloat>(input, output);
}
}


