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
#include "openpal/serialization/SingleFloat.h"

#include "openpal/util/Limits.h"
#include "openpal/serialization/FloatByteOrder.h"

#include <cstring>

namespace openpal
{
const float SingleFloat::Max(openpal::MaxValue<float>());
const float SingleFloat::Min(openpal::MinValue<float>());

union SingleFloatUnion
{
	uint8_t bytes[4];
	float value;
};


float SingleFloat::ReadBuffer(RSlice& buffer)
{
	auto ret = Read(buffer);
	buffer.Advance(SIZE);
	return ret;
}

void SingleFloat::WriteBuffer(WSlice& buffer, float value)
{
	Write(buffer, value);
	buffer.Advance(SIZE);
}

float SingleFloat::Read(const uint8_t* data)
{
	if (FloatByteOrder::ORDER == FloatByteOrder::Value::NORMAL)
	{
		SingleFloatUnion x = {{ data[0], data[1], data[2], data[3] }};
		return x.value;
	}
	else
	{
		SingleFloatUnion x = {{ data[3], data[2], data[1], data[0] }};
		return x.value;
	}
}

void SingleFloat::Write(uint8_t* dest, float value)
{
	if (FloatByteOrder::ORDER == FloatByteOrder::Value::NORMAL)
	{
		memcpy(dest, &value, SIZE);
	}
	else
	{
		auto data = reinterpret_cast<uint8_t*>(&value);
		uint8_t bytes[4] = { data[3], data[2], data[1], data[0] };
		memcpy(dest, bytes, SIZE);
	}
}
}

