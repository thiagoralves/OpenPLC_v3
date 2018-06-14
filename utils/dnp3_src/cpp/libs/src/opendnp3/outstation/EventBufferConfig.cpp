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
#include "opendnp3/outstation/EventBufferConfig.h"

namespace opendnp3
{

EventBufferConfig EventBufferConfig::AllTypes(uint16_t sizes)
{
	return EventBufferConfig(sizes, sizes, sizes, sizes, sizes, sizes, sizes, sizes);
}

uint16_t EventBufferConfig::GetMaxEventsForType(EventType type) const
{
	switch (type)
	{
	case(EventType::Binary) :
		return maxBinaryEvents;
	case(EventType::DoubleBitBinary) :
		return maxDoubleBinaryEvents;
	case(EventType::BinaryOutputStatus) :
		return maxBinaryOutputStatusEvents;
	case(EventType::Counter) :
		return maxCounterEvents;
	case(EventType::FrozenCounter) :
		return maxFrozenCounterEvents;
	case(EventType::Analog) :
		return maxAnalogEvents;
	case(EventType::AnalogOutputStatus) :
		return maxAnalogOutputStatusEvents;
	case(EventType::SecurityStat) :
		return maxSecurityStatisticEvents;
	default:
		return 0;
	}
}

EventBufferConfig::EventBufferConfig(
    uint16_t maxBinaryEvents_,
    uint16_t maxDoubleBinaryEvents_,
    uint16_t maxAnalogEvents_,
    uint16_t maxCounterEvents_,
    uint16_t maxFrozenCounterEvents_,
    uint16_t maxBinaryOutputStatusEvents_,
    uint16_t maxAnalogOutputStatusEvents_,
    uint16_t maxSecurityStatisticEvents_) :

	maxBinaryEvents(maxBinaryEvents_),
	maxDoubleBinaryEvents(maxDoubleBinaryEvents_),
	maxAnalogEvents(maxAnalogEvents_),
	maxCounterEvents(maxCounterEvents_),
	maxFrozenCounterEvents(maxFrozenCounterEvents_),
	maxBinaryOutputStatusEvents(maxBinaryOutputStatusEvents_),
	maxAnalogOutputStatusEvents(maxAnalogOutputStatusEvents_),
	maxSecurityStatisticEvents(maxSecurityStatisticEvents_)
{

}

uint32_t EventBufferConfig::TotalEvents() const
{
	return	maxBinaryEvents +
	        maxDoubleBinaryEvents +
	        maxAnalogEvents +
	        maxCounterEvents +
	        maxFrozenCounterEvents +
	        maxBinaryOutputStatusEvents +
	        maxAnalogOutputStatusEvents +
	        maxSecurityStatisticEvents;
}


}

