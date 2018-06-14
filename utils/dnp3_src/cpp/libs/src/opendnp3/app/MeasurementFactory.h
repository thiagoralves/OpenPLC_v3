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
#ifndef OPENDNP3_MEASUREMENTFACTORY_H
#define OPENDNP3_MEASUREMENTFACTORY_H

#include <openpal/util/Uncopyable.h>
#include "opendnp3/app/MeasurementTypes.h"
#include "opendnp3/app/BinaryCommandEvent.h"
#include "opendnp3/app/AnalogCommandEvent.h"
#include "opendnp3/app/ControlRelayOutputBlock.h"
#include "opendnp3/app/AnalogOutput.h"
#include "opendnp3/app/SecurityStat.h"

namespace opendnp3
{

struct BinaryFactory: private openpal::StaticOnly
{
	inline static Binary From(Flags flags)
	{
		return Binary(flags);
	}

	inline static Binary From(uint8_t flags, uint16_t time)
	{
		return From(flags, DNPTime(time));
	}

	inline static Binary From(uint8_t flags, DNPTime time)
	{
		return Binary(flags, time);
	}
};

struct DoubleBitBinaryFactory : private openpal::StaticOnly
{
	inline static DoubleBitBinary From(Flags flags)
	{
		return DoubleBitBinary(flags);
	}

	inline static DoubleBitBinary From(Flags flags, uint16_t time)
	{
		return From(flags, DNPTime(time));
	}

	inline static DoubleBitBinary From(Flags flags, DNPTime time)
	{
		return DoubleBitBinary(flags, time);
	}
};

struct AnalogFactory: private openpal::StaticOnly
{
	static Analog From(int32_t count)
	{
		return Analog(count);
	}
	static Analog From(Flags flags, double value)
	{
		return Analog(value, flags);
	}
	static Analog From(Flags flags, double value, DNPTime time)
	{
		return Analog(value, flags, time);
	}
};

struct CounterFactory: private openpal::StaticOnly
{
	inline static Counter From(uint32_t count)
	{
		return Counter(count);
	}
	inline static Counter From(Flags flags, uint32_t count)
	{
		return Counter(count, flags);
	}
	inline static Counter From(Flags flags, uint32_t count, DNPTime time)
	{
		return Counter(count, flags, time);
	}
};

struct FrozenCounterFactory: private openpal::StaticOnly
{
	inline static FrozenCounter From(uint32_t count)
	{
		return FrozenCounter(count);
	}
	inline static FrozenCounter From(Flags flags, uint32_t count)
	{
		return FrozenCounter(count, flags);
	}
	inline static FrozenCounter From(Flags flags, uint32_t count, DNPTime time)
	{
		return FrozenCounter(count, flags, time);
	}
};

struct TimeAndIntervalFactory : private openpal::StaticOnly
{
	inline static TimeAndInterval From(DNPTime time, uint32_t interval, uint8_t units)
	{
		return TimeAndInterval(time, interval, units);
	}
};

struct ControlRelayOutputBlockFactory: private openpal::StaticOnly
{
	inline static ControlRelayOutputBlock From(
	    uint8_t code,
	    uint8_t count,
	    uint32_t onTime,
	    uint32_t offTime,
	    uint8_t status)
	{
		return ControlRelayOutputBlock(code, count, onTime, offTime, CommandStatusFromType(status));
	}
};

struct BinaryOutputStatusFactory: private openpal::StaticOnly
{
	inline static BinaryOutputStatus From(Flags flags)
	{
		return BinaryOutputStatus(flags);
	}

	inline static BinaryOutputStatus From(Flags flags, DNPTime time)
	{
		return BinaryOutputStatus(flags, time);
	}
};

struct BinaryCommandEventFactory : private openpal::StaticOnly
{
	inline static BinaryCommandEvent From(Flags flags)
	{
		return BinaryCommandEvent(flags);
	}

	inline static BinaryCommandEvent From(uint8_t flags, DNPTime time)
	{
		return BinaryCommandEvent(flags, time);
	}
};

struct AnalogOutputStatusFactory: private openpal::StaticOnly
{
	inline static AnalogOutputStatus From(uint8_t flags, double value)
	{
		return AnalogOutputStatus(value, flags);
	}

	inline static AnalogOutputStatus From(uint8_t flags, double value, DNPTime time)
	{
		return AnalogOutputStatus(value, flags, time);
	}
};

struct AnalogCommandEventFactory : private openpal::StaticOnly
{
	inline static AnalogCommandEvent From(uint8_t status, double value)
	{
		return AnalogCommandEvent(value, CommandStatusFromType(status));
	}

	inline static AnalogCommandEvent From(uint8_t status, double value, DNPTime time)
	{
		return AnalogCommandEvent(value, CommandStatusFromType(status), time);
	}
};

struct SecurityStatFactory : private openpal::StaticOnly
{
	inline static SecurityStat From(uint8_t flags, uint16_t assocId, uint32_t value)
	{
		return SecurityStat(flags, assocId, value);
	}

	inline static SecurityStat From(uint8_t flags, uint16_t assocId, uint32_t value, DNPTime time)
	{
		return SecurityStat(flags, assocId, value, time);
	}
};

template <class Target, class ValueType>
struct AnalogOutputFactory: private openpal::StaticOnly
{
	inline static Target From(ValueType value, uint8_t status)
	{
		return Target(value, CommandStatusFromType(status));
	}
};

typedef AnalogOutputFactory<AnalogOutputInt32, int32_t> AnalogOutputInt32Factory;
typedef AnalogOutputFactory<AnalogOutputInt16, int16_t> AnalogOutputInt16Factory;
typedef AnalogOutputFactory<AnalogOutputFloat32, float> AnalogOutputFloat32Factory;
typedef AnalogOutputFactory<AnalogOutputDouble64, double> AnalogOutputDouble64Factory;



}

#endif
