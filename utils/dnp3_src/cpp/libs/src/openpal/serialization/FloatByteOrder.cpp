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
#include "openpal/serialization/FloatByteOrder.h"


namespace openpal
{

const FloatByteOrder::Value FloatByteOrder::ORDER(GetByteOrder());

union FloatUnion
{
	uint8_t bytes[4];
	float f;
};

static_assert(sizeof(FloatUnion) == 4, "Bad float union size");

bool FloatByteOrder::IsNormalByteOrder()
{
	FloatUnion value = {{ 0x00, 0x00, 0xA0, 0xC1 }};
	return (value.f == -20.0f);
}

bool FloatByteOrder::IsReverseByteOrder()
{
	FloatUnion value = {{ 0xC1, 0xA0, 0x00, 0x00 }};
	return (value.f == -20.0f);
}

FloatByteOrder::Value FloatByteOrder::GetByteOrder()
{
	if (IsNormalByteOrder())
	{
		return FloatByteOrder::Value::NORMAL;
	}
	else if (IsReverseByteOrder())
	{
		return FloatByteOrder::Value::REVERSE;
	}
	else
	{
		return FloatByteOrder::Value::UNSUPPORTED;
	}
}


}


