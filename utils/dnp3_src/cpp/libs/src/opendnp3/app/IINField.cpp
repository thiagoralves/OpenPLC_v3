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
#include "opendnp3/app/IINField.h"

#include <openpal/util/ToHex.h>

using namespace openpal;
using namespace std;

namespace opendnp3
{

bool IINField::IsSet(IINBit bit) const
{
	switch(bit)
	{
	case(IINBit::ALL_STATIONS):
		return Get(LSBMask::ALL_STATIONS);
	case(IINBit::CLASS1_EVENTS):
		return Get(LSBMask::CLASS1_EVENTS);
	case(IINBit::CLASS2_EVENTS):
		return Get(LSBMask::CLASS2_EVENTS);
	case(IINBit::CLASS3_EVENTS):
		return Get(LSBMask::CLASS3_EVENTS);
	case(IINBit::NEED_TIME):
		return Get(LSBMask::NEED_TIME);
	case(IINBit::LOCAL_CONTROL):
		return Get(LSBMask::LOCAL_CONTROL);
	case(IINBit::DEVICE_TROUBLE):
		return Get(LSBMask::DEVICE_TROUBLE);
	case(IINBit::DEVICE_RESTART):
		return Get(LSBMask::DEVICE_RESTART);
	case(IINBit::FUNC_NOT_SUPPORTED):
		return Get(MSBMask::FUNC_NOT_SUPPORTED);
	case(IINBit::OBJECT_UNKNOWN):
		return Get(MSBMask::OBJECT_UNKNOWN);
	case(IINBit::PARAM_ERROR):
		return Get(MSBMask::PARAM_ERROR);
	case(IINBit::EVENT_BUFFER_OVERFLOW):
		return Get(MSBMask::EVENT_BUFFER_OVERFLOW);
	case(IINBit::ALREADY_EXECUTING):
		return Get(MSBMask::ALREADY_EXECUTING);
	case(IINBit::CONFIG_CORRUPT):
		return Get(MSBMask::CONFIG_CORRUPT);
	case(IINBit::RESERVED1):
		return Get(MSBMask::RESERVED1);
	case(IINBit::RESERVED2):
		return Get(MSBMask::RESERVED2);
	default:
		return false;
	};
}

void IINField::SetBitToValue(IINBit bit, bool value)
{
	if (value)
	{
		SetBit(bit);
	}
	else
	{
		ClearBit(bit);
	}
}

void IINField::SetBit(IINBit bit)
{
	switch(bit)
	{
	case(IINBit::ALL_STATIONS):
		Set(LSBMask::ALL_STATIONS);
		break;
	case(IINBit::CLASS1_EVENTS):
		Set(LSBMask::CLASS1_EVENTS);
		break;
	case(IINBit::CLASS2_EVENTS):
		Set(LSBMask::CLASS2_EVENTS);
		break;
	case(IINBit::CLASS3_EVENTS):
		Set(LSBMask::CLASS3_EVENTS);
		break;
	case(IINBit::NEED_TIME):
		Set(LSBMask::NEED_TIME);
		break;
	case(IINBit::LOCAL_CONTROL):
		Set(LSBMask::LOCAL_CONTROL);
		break;
	case(IINBit::DEVICE_TROUBLE):
		Set(LSBMask::DEVICE_TROUBLE);
		break;
	case(IINBit::DEVICE_RESTART):
		Set(LSBMask::DEVICE_RESTART);
		break;
	case(IINBit::FUNC_NOT_SUPPORTED):
		Set(MSBMask::FUNC_NOT_SUPPORTED);
		break;
	case(IINBit::OBJECT_UNKNOWN):
		Set(MSBMask::OBJECT_UNKNOWN);
		break;
	case(IINBit::PARAM_ERROR):
		Set(MSBMask::PARAM_ERROR);
		break;
	case(IINBit::EVENT_BUFFER_OVERFLOW):
		Set(MSBMask::EVENT_BUFFER_OVERFLOW);
		break;
	case(IINBit::ALREADY_EXECUTING):
		Set(MSBMask::ALREADY_EXECUTING);
		break;
	case(IINBit::CONFIG_CORRUPT):
		Set(MSBMask::CONFIG_CORRUPT);
		break;
	case(IINBit::RESERVED1):
		Set(MSBMask::RESERVED1);
		break;
	case(IINBit::RESERVED2):
		Set(MSBMask::RESERVED2);
		break;
	default:
		break;
	};
}

void IINField::ClearBit(IINBit bit)
{
	switch(bit)
	{
	case(IINBit::ALL_STATIONS):
		Clear(LSBMask::ALL_STATIONS);
		break;
	case(IINBit::CLASS1_EVENTS):
		Clear(LSBMask::CLASS1_EVENTS);
		break;
	case(IINBit::CLASS2_EVENTS):
		Clear(LSBMask::CLASS2_EVENTS);
		break;
	case(IINBit::CLASS3_EVENTS):
		Clear(LSBMask::CLASS3_EVENTS);
		break;
	case(IINBit::NEED_TIME):
		Clear(LSBMask::NEED_TIME);
		break;
	case(IINBit::LOCAL_CONTROL):
		Clear(LSBMask::LOCAL_CONTROL);
		break;
	case(IINBit::DEVICE_TROUBLE):
		Clear(LSBMask::DEVICE_TROUBLE);
		break;
	case(IINBit::DEVICE_RESTART):
		Clear(LSBMask::DEVICE_RESTART);
		break;
	case(IINBit::FUNC_NOT_SUPPORTED):
		Clear(MSBMask::FUNC_NOT_SUPPORTED);
		break;
	case(IINBit::OBJECT_UNKNOWN):
		Clear(MSBMask::OBJECT_UNKNOWN);
		break;
	case(IINBit::PARAM_ERROR):
		Clear(MSBMask::PARAM_ERROR);
		break;
	case(IINBit::EVENT_BUFFER_OVERFLOW):
		Clear(MSBMask::EVENT_BUFFER_OVERFLOW);
		break;
	case(IINBit::ALREADY_EXECUTING):
		Clear(MSBMask::ALREADY_EXECUTING);
		break;
	case(IINBit::CONFIG_CORRUPT):
		Clear(MSBMask::CONFIG_CORRUPT);
		break;
	case(IINBit::RESERVED1):
		Clear(MSBMask::RESERVED1);
		break;
	case(IINBit::RESERVED2):
		Clear(MSBMask::RESERVED2);
		break;
	default:
		break;
	};
}

bool IINField::operator==(const IINField& aRHS) const
{
	return (LSB == aRHS.LSB) && (MSB == aRHS.MSB);
}

}

