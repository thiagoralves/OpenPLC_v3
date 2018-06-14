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

#ifndef OPENDNP3_EXPECTEDVALUE_H
#define OPENDNP3_EXPECTEDVALUE_H

#include <cstdint>

#include "opendnp3/app/MeasurementTypes.h"
#include "opendnp3/gen/StaticTypeBitmask.h"

#include <sstream>

namespace opendnp3
{

enum class ValueType
{
	Binary,
	DoubleBitBinary,
	Analog,
	Counter,
	FrozenCounter,
	AnalogOutputStatus,
	BinaryOutputStatus
};

class ExpectedValue final
{
	ExpectedValue(int64_t value, uint16_t index, ValueType type) :
		value(value), index(index), type(type)
	{}

public:

	static const char* ToString(ValueType vt)
	{
		switch (vt)
		{
		case(ValueType::Binary):
			return "Binary";
		case(ValueType::DoubleBitBinary):
			return "DoubleBitBinary";
		case(ValueType::Analog):
			return "Analog";
		case(ValueType::Counter):
			return "Counter";
		case(ValueType::FrozenCounter):
			return "FrozenCounter";
		case(ValueType::AnalogOutputStatus):
		default:
			return "BinaryOutputStatus";
		}
	}

	bool Equals(const ExpectedValue& other) const
	{
		return (type == other.type) && (index == other.index) && (value == other.value);
	}

	int64_t value;
	uint16_t index;
	ValueType type;

	ExpectedValue(const Binary& value, uint16_t index) : ExpectedValue(value.value, index, ValueType::Binary) {}
	ExpectedValue(const DoubleBitBinary& value, uint16_t index) : ExpectedValue(static_cast<int64_t>(value.value), index, ValueType::DoubleBitBinary) {}
	ExpectedValue(const Analog& value, uint16_t index) : ExpectedValue(static_cast<int64_t>(value.value), index, ValueType::Analog) {}
	ExpectedValue(const Counter& value, uint16_t index) : ExpectedValue(value.value, index, ValueType::Counter) {}
	ExpectedValue(const FrozenCounter& value, uint16_t index) : ExpectedValue(value.value, index, ValueType::FrozenCounter) {}
	ExpectedValue(const AnalogOutputStatus& value, uint16_t index) : ExpectedValue(static_cast<int64_t>(value.value), index, ValueType::AnalogOutputStatus) {}
	ExpectedValue(const BinaryOutputStatus& value, uint16_t index) : ExpectedValue(value.value, index, ValueType::BinaryOutputStatus) {}
};

inline std::ostream& operator<<(std::ostream& os, const ExpectedValue& v)
{
	os << v.index << " - " << ExpectedValue::ToString(v.type) << "(" << v.value << ")";
	return os;
}

}

#endif

