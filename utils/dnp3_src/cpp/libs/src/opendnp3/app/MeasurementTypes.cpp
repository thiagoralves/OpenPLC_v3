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
#include "opendnp3/app/MeasurementTypes.h"

#include "opendnp3/app/QualityFlags.h"
#include "opendnp3/gen/BinaryQuality.h"

namespace opendnp3
{

// ------------ Binary ---------------

Binary::Binary() : TypedMeasurement(false, flags::RESTART)
{}

Binary::Binary(bool value) : TypedMeasurement(value, flags::GetBinaryFlags(flags::ONLINE, value))
{}

Binary::Binary(Flags flags) : TypedMeasurement(flags::GetBinaryValue(flags), flags)
{}

Binary::Binary(Flags flags, DNPTime time) : TypedMeasurement(flags::GetBinaryValue(flags), flags, time)
{}

Binary::Binary(bool value, Flags flags) : TypedMeasurement(value, flags::GetBinaryFlags(flags, value))
{}

Binary::Binary(bool value, Flags flags, DNPTime time) : TypedMeasurement(value, flags::GetBinaryFlags(flags, value), time)
{}

// ------------ Double Bit Binary ---------------

DoubleBitBinary::DoubleBitBinary() : TypedMeasurement(DoubleBit::INDETERMINATE, flags::RESTART)
{}

DoubleBitBinary::DoubleBitBinary(DoubleBit value) : TypedMeasurement(value, GetFlags(flags::ONLINE, value))
{}

DoubleBitBinary::DoubleBitBinary(Flags flags) : TypedMeasurement(GetValue(flags), flags)
{}

DoubleBitBinary::DoubleBitBinary(Flags flags, DNPTime time) : TypedMeasurement(GetValue(flags), flags, time)
{}

DoubleBitBinary::DoubleBitBinary(DoubleBit value, Flags flags) : TypedMeasurement(value, GetFlags(flags, value))
{}

DoubleBitBinary::DoubleBitBinary(DoubleBit value, Flags flags, DNPTime time) : TypedMeasurement(value, GetFlags(flags, value), time)
{}

DoubleBit DoubleBitBinary::GetValue(Flags flags)
{
	// the value is the top 2 bits, so down-shift 6
	uint8_t value = flags.value >> 6;
	return DoubleBitFromType(value);
}

Flags DoubleBitBinary::GetFlags(Flags flags, DoubleBit state)
{
	uint8_t value = DoubleBitToType(state) << 6;
	return (QualityMask & flags.value) | value;
}

// ------------ Binary Output Status ---------------


BinaryOutputStatus::BinaryOutputStatus() : TypedMeasurement(false, flags::RESTART)
{}

BinaryOutputStatus::BinaryOutputStatus(bool value) : TypedMeasurement(value, flags::GetBinaryFlags(flags::ONLINE, value))
{}

BinaryOutputStatus::BinaryOutputStatus(Flags flags) : TypedMeasurement(flags::GetBinaryValue(flags), flags)
{}

BinaryOutputStatus::BinaryOutputStatus(Flags flags, DNPTime time) : TypedMeasurement(flags::GetBinaryValue(flags), flags, time)
{}

BinaryOutputStatus::BinaryOutputStatus(bool value, Flags flags) : TypedMeasurement(value, flags::GetBinaryFlags(flags, value))
{}

BinaryOutputStatus::BinaryOutputStatus(bool value, Flags flags, DNPTime time) : TypedMeasurement(value, flags::GetBinaryFlags(flags, value), time)
{}

// ------------ Analog ---------------

Analog::Analog() : TypedMeasurement(flags::RESTART)
{}

Analog::Analog(double value) : TypedMeasurement(value, flags::ONLINE)
{}

Analog::Analog(double value, Flags flags) : TypedMeasurement(value, flags)
{}

Analog::Analog(double value, Flags flags, DNPTime time) : TypedMeasurement<double>(value, flags, time)
{}


// ------------ Counter ---------------



Counter::Counter() : TypedMeasurement(0, flags::RESTART) {}

Counter::Counter(uint32_t value) : TypedMeasurement<uint32_t>(value, flags::ONLINE)
{}

Counter::Counter(uint32_t value, Flags flags) : TypedMeasurement<uint32_t>(value, flags)
{}

Counter::Counter(uint32_t value, Flags flags, DNPTime time) : TypedMeasurement<uint32_t>(value, flags, time)
{}

// ------------ Frozen Counter ---------------


FrozenCounter::FrozenCounter() : TypedMeasurement(0, flags::RESTART) {}

FrozenCounter::FrozenCounter(uint32_t value) : TypedMeasurement<uint32_t>(value, flags::ONLINE)
{}

FrozenCounter::FrozenCounter(uint32_t value, Flags flags) : TypedMeasurement<uint32_t>(value, flags)
{}

FrozenCounter::FrozenCounter(uint32_t value, Flags flags, DNPTime time) : TypedMeasurement<uint32_t>(value, flags, time)
{}

// ------------ Analog Output Status ---------------

AnalogOutputStatus::AnalogOutputStatus() : TypedMeasurement<double>(flags::RESTART) {}

AnalogOutputStatus::AnalogOutputStatus(double value) : TypedMeasurement<double>(value, flags::ONLINE)
{}

AnalogOutputStatus::AnalogOutputStatus(double value, Flags flags) : TypedMeasurement<double>(value, flags)
{}

AnalogOutputStatus::AnalogOutputStatus(double value, Flags flags, DNPTime time) : TypedMeasurement<double>(value, flags, time)
{}

// ------------ TimeAndInterval ---------------

TimeAndInterval::TimeAndInterval() : time(0), interval(0), units(0)
{}

TimeAndInterval::TimeAndInterval(DNPTime time_, uint32_t interval_, uint8_t units_) :
	time(time_),
	interval(interval_),
	units(units_)
{}

TimeAndInterval::TimeAndInterval(DNPTime time_, uint32_t interval_, IntervalUnits units_) :
	time(time_),
	interval(interval_),
	units(static_cast<uint8_t>(units_))
{}

IntervalUnits TimeAndInterval::GetUnitsEnum() const
{
	return IntervalUnitsFromType(units);
}

} // end ns

