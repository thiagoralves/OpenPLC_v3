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
#ifndef OPENDNP3_SOERECORD_H
#define OPENDNP3_SOERECORD_H

#include "opendnp3/app/EventType.h"
#include "opendnp3/app/MeasurementTypeSpecs.h"
#include "opendnp3/app/SecurityStat.h"

#include <openpal/serialization/UInt48Type.h>


namespace opendnp3
{

template <class Spec>
struct ValueAndVariation
{
	typename Spec::value_t value;
	typename Spec::event_variation_t defaultVariation;
	typename Spec::event_variation_t selectedVariation;

	void SelectDefaultVariation()
	{
		selectedVariation = defaultVariation;
	}
};

template <class ValueType>
struct EventInstance
{
	ValueType value;
	uint16_t index;
};

union EventValue
{
	ValueAndVariation<BinarySpec> binary;
	ValueAndVariation<DoubleBitBinarySpec> doubleBinary;
	ValueAndVariation<BinaryOutputStatusSpec> binaryOutputStatus;
	ValueAndVariation<CounterSpec> counter;
	ValueAndVariation<FrozenCounterSpec> frozenCounter;
	ValueAndVariation<AnalogSpec> analog;
	ValueAndVariation<AnalogOutputStatusSpec> analogOutputStatus;
	ValueAndVariation<SecurityStatSpec> securityStat;
};

class SOERecord
{
public:

	SOERecord();

	SOERecord(const Binary& meas, uint16_t index, EventClass clazz, EventBinaryVariation var);
	SOERecord(const DoubleBitBinary& meas, uint16_t index, EventClass clazz, EventDoubleBinaryVariation var);
	SOERecord(const BinaryOutputStatus& meas, uint16_t index, EventClass clazz, EventBinaryOutputStatusVariation var);
	SOERecord(const Counter& meas, uint16_t index, EventClass clazz, EventCounterVariation var);
	SOERecord(const FrozenCounter& meas, uint16_t index, EventClass clazz, EventFrozenCounterVariation var);
	SOERecord(const Analog& meas, uint16_t index, EventClass clazz, EventAnalogVariation var);
	SOERecord(const AnalogOutputStatus& meas, uint16_t index, EventClass clazz, EventAnalogOutputStatusVariation var);
	SOERecord(const SecurityStat& meas, uint16_t index, EventClass clazz, EventSecurityStatVariation var);

	template <class Spec>
	EventInstance<typename Spec::meas_t> ReadEvent()
	{
		return EventInstance <typename Spec::meas_t>
		{
			typename Spec::meas_t(GetValue<Spec>().value, flags, time), index
		};
	}

	template <class Spec>
	const ValueAndVariation<Spec>& GetValue();

	void SelectDefault();

	void Select(EventBinaryVariation var);
	void Select(EventDoubleBinaryVariation var);
	void Select(EventBinaryOutputStatusVariation var);
	void Select(EventCounterVariation var);
	void Select(EventFrozenCounterVariation var);
	void Select(EventAnalogVariation var);
	void Select(EventAnalogOutputStatusVariation var);
	void Select(EventSecurityStatVariation var);

	EventType type;
	EventClass clazz;
	bool selected;
	bool written;
	void Reset();

	DNPTime GetTime() const
	{
		return time;
	}

private:

	SOERecord(EventType type, EventClass clazz, uint16_t index, DNPTime time, uint8_t flags);


	// the actual value;
	EventValue value;
	uint16_t index;
	DNPTime time;
	uint8_t flags;

};

}

#endif
