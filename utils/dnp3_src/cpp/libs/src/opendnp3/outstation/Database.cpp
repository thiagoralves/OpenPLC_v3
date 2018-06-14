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
#include "Database.h"

#include <openpal/logging/LogMacros.h>

#include <assert.h>

using namespace openpal;

namespace opendnp3
{

Database::Database(const DatabaseSizes& dbSizes, IEventReceiver& eventReceiver, IndexMode indexMode, StaticTypeBitField allowedClass0Types) :
	eventReceiver(&eventReceiver),
	indexMode(indexMode),
	buffers(dbSizes, allowedClass0Types, indexMode)
{

}

bool Database::Update(const Binary& value, uint16_t index, EventMode mode)
{
	return this->UpdateEvent<BinarySpec>(value, index, mode);
}

bool Database::Update(const DoubleBitBinary& value, uint16_t index, EventMode mode)
{
	return this->UpdateEvent<DoubleBitBinarySpec>(value, index, mode);
}

bool Database::Update(const Analog& value, uint16_t index, EventMode mode)
{
	return this->UpdateEvent<AnalogSpec>(value, index, mode);
}

bool Database::Update(const Counter& value, uint16_t index, EventMode mode)
{
	return this->UpdateEvent<CounterSpec>(value, index, mode);
}

bool Database::Update(const FrozenCounter& value, uint16_t index, EventMode mode)
{
	return this->UpdateEvent<FrozenCounterSpec>(value, index, mode);
}

bool Database::Update(const BinaryOutputStatus& value, uint16_t index, EventMode mode)
{
	return this->UpdateEvent<BinaryOutputStatusSpec>(value, index, mode);
}

bool Database::Update(const AnalogOutputStatus& value, uint16_t index, EventMode mode)
{
	return this->UpdateEvent<AnalogOutputStatusSpec>(value, index, mode);
}

bool Database::Update(const TimeAndInterval& value, uint16_t index)
{
	auto rawIndex = GetRawIndex<TimeAndIntervalSpec>(index);
	auto view = buffers.buffers.GetArrayView<TimeAndIntervalSpec>();

	if (view.Contains(rawIndex))
	{
		view[rawIndex].value = value;
		return true;
	}
	else
	{
		return false;
	}
}

bool Database::Modify(FlagsType type, uint16_t start, uint16_t stop, uint8_t flags)
{
	switch (type)
	{
	case(FlagsType::BinaryInput):
		return Modify<BinarySpec>(start, stop, flags);
	case(FlagsType::DoubleBinaryInput):
		return Modify<DoubleBitBinarySpec>(start, stop, flags);
	case(FlagsType::AnalogInput):
		return Modify<AnalogSpec>(start, stop, flags);
	case(FlagsType::Counter):
		return Modify<CounterSpec>(start, stop, flags);
	case(FlagsType::FrozenCounter):
		return Modify<FrozenCounterSpec>(start, stop, flags);
	case(FlagsType::BinaryOutputStatus):
		return Modify<BinaryOutputStatusSpec>(start, stop, flags);
	case(FlagsType::AnalogOutputStatus):
		return Modify<AnalogOutputStatusSpec>(start, stop, flags);
	}

	return false;
}

bool Database::ConvertToEventClass(PointClass pc, EventClass& ec)
{
	switch (pc)
	{
	case(PointClass::Class1) :
		ec = EventClass::EC1;
		return true;
	case(PointClass::Class2) :
		ec = EventClass::EC2;
		return true;
	case(PointClass::Class3) :
		ec = EventClass::EC3;
		return true;
	default:
		return false;
	}
}

template <class Spec>
uint16_t Database::GetRawIndex(uint16_t index)
{
	if (indexMode == IndexMode::Contiguous)
	{
		return index;
	}
	else
	{
		auto view = buffers.buffers.GetArrayView<Spec>();
		auto result = IndexSearch::FindClosestRawIndex(view, index);
		return result.match ? result.index : openpal::MaxValue<uint16_t>();
	}
}

template <class Spec>
bool Database::UpdateEvent(const typename Spec::meas_t& value, uint16_t index, EventMode mode)
{
	auto rawIndex = GetRawIndex<Spec>(index);
	auto view = buffers.buffers.GetArrayView<Spec>();

	if (view.Contains(rawIndex))
	{
		this->UpdateAny(view[rawIndex], value, mode);
		return true;
	}
	else
	{
		return false;
	}
}

template <class Spec>
bool Database::UpdateAny(Cell<Spec>& cell, const typename Spec::meas_t& value, EventMode mode)
{
	EventClass ec;
	if (ConvertToEventClass(cell.config.clazz, ec))
	{
		bool createEvent = false;

		switch (mode)
		{
		case(EventMode::Force):
			createEvent = true;
			break;
		case(EventMode::Detect):
			createEvent = cell.event.IsEvent(cell.config, value);
			break;
		default:
			break;
		}

		if (createEvent)
		{
			cell.event.lastEvent = value;
			eventReceiver->Update(Event<Spec>(value, cell.config.vIndex, ec, cell.config.evariation));
		}
	}

	cell.value = value;
	return true;
}

template <class Spec>
bool Database::Modify(uint16_t start, uint16_t stop, uint8_t flags)
{
	auto rawStart = GetRawIndex<Spec>(start);
	auto rawStop = GetRawIndex<Spec>(stop);

	auto view = buffers.buffers.GetArrayView<Spec>();

	if (view.Contains(rawStart) && view.Contains(rawStop) && (rawStart <= rawStop))
	{
		for (uint16_t i = rawStart; i <= rawStop; ++i)
		{
			auto copy = view[i].value;
			copy.flags = flags;
			this->UpdateAny(view[i], copy, EventMode::Detect);
		}

		return true;
	}
	else
	{
		return false;
	}
}

}

