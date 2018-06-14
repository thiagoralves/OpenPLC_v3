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
#ifndef OPENDNP3_MEASUREMENTTYPESPECS_H
#define OPENDNP3_MEASUREMENTTYPESPECS_H

#include "opendnp3/outstation/MeasurementConfig.h"
#include "opendnp3/app/EventCells.h"
#include "opendnp3/app/EventTriggers.h"

namespace opendnp3
{


struct BinarySpec : public BinaryInfo
{
	typedef BinaryConfig config_t;
	typedef SimpleEventCell<BinarySpec> event_cell_t;

	inline static bool IsQualityOnlineOnly(const Binary& binary)
	{
		return (binary.flags.value & 0b01111111) == static_cast<uint8_t>(BinaryQuality::ONLINE);
	}

	inline static bool IsEvent(const Binary& oldValue, const Binary& newValue)
	{
		return oldValue.flags.value != newValue.flags.value;
	}
};

struct DoubleBitBinarySpec : public DoubleBitBinaryInfo
{
	typedef DoubleBitBinaryConfig config_t;
	typedef SimpleEventCell<DoubleBitBinarySpec> event_cell_t;

	inline static bool IsEvent(const DoubleBitBinary& oldValue, const DoubleBitBinary& newValue)
	{
		return oldValue.flags.value != newValue.flags.value;
	}
};

struct BinaryOutputStatusSpec : public BinaryOutputStatusInfo
{
	typedef BOStatusConfig config_t;
	typedef SimpleEventCell<BinaryOutputStatusSpec> event_cell_t;

	inline static bool IsEvent(const BinaryOutputStatus& oldValue, const BinaryOutputStatus& newValue)
	{
		return oldValue.flags.value != newValue.flags.value;
	}
};


struct AnalogSpec : public AnalogInfo
{
	typedef AnalogConfig config_t;
	typedef DeadbandEventCell<AnalogSpec> event_cell_t;

	inline static bool IsEvent(const Analog& oldValue, const Analog& newValue, double deadband)
	{
		return measurements::IsEvent(newValue, oldValue, deadband);
	}
};

struct CounterSpec : public CounterInfo
{
	typedef CounterConfig config_t;
	typedef DeadbandEventCell<CounterSpec> event_cell_t;

	inline static bool IsEvent(const Counter& oldValue, const Counter& newValue, uint32_t deadband)
	{
		if (oldValue.flags.value != newValue.flags.value)
		{
			return true;
		}
		else
		{
			return measurements::IsEvent<uint32_t, uint64_t>(oldValue.value, newValue.value, deadband);
		}
	}
};

struct FrozenCounterSpec : public FrozenCounterInfo
{
	typedef FrozenCounterConfig config_t;
	typedef DeadbandEventCell<FrozenCounterSpec> event_cell_t;

	inline static bool IsEvent(const FrozenCounter& oldValue, const FrozenCounter& newValue, uint32_t deadband)
	{
		if (oldValue.flags.value != newValue.flags.value)
		{
			return true;
		}
		else
		{
			return measurements::IsEvent<uint32_t, uint64_t>(oldValue.value, newValue.value, deadband);
		}
	}
};

struct AnalogOutputStatusSpec : public AnalogOutputStatusInfo
{
	typedef AOStatusConfig config_t;
	typedef DeadbandEventCell<AnalogOutputStatusSpec> event_cell_t;

	inline static bool IsEvent(const AnalogOutputStatus& oldValue, const AnalogOutputStatus& newValue, double deadband)
	{
		return measurements::IsEvent(newValue, oldValue, deadband);
	}
};

struct TimeAndIntervalSpec : public TimeAndIntervalInfo
{
	typedef TimeAndIntervalConfig config_t;
	typedef EmptyEventCell event_cell_t;
};

struct SecurityStatSpec : public SecurityStatInfo
{
	typedef SecurityStatConfig config_t;
	typedef EmptyEventCell event_cell_t;

	inline static bool IsEvent(const SecurityStat& oldValue, const SecurityStat& newValue, uint32_t deadband)
	{
		if (oldValue.quality != newValue.quality)
		{
			return true;
		}
		else
		{
			return measurements::IsEvent<uint32_t, uint64_t>(oldValue.value.count, newValue.value.count, deadband);
		}
	}
};

}

#endif
