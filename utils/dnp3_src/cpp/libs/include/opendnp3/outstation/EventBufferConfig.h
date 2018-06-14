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
#ifndef OPENDNP3_EVENTBUFFERCONFIG_H
#define OPENDNP3_EVENTBUFFERCONFIG_H

#include <cstdint>

#include "opendnp3/app/EventType.h"

namespace opendnp3
{

/**

  Configuration of maximum event counts per event type.

  The underlying implementation uses a *preallocated* heap buffer to store events
  until they are transmitted to the master. The size of this buffer is proportional
  to the TotalEvents() method, i.e. the sum of the maximum events for each type.

  Implementations should configure the buffers to store a reasonable number events
  given the polling frequency and memory restrictions of the target platform.

*/
struct EventBufferConfig
{
	/**
		Construct the class using the same maximum for all types. This is mainly used for demo purposes.
		You probably don't want to use this method unless your implementation actually reports every type.
	*/
	static EventBufferConfig AllTypes(uint16_t sizes);

	/// Retrieve the number of events using a type enumeration
	uint16_t GetMaxEventsForType(EventType type) const;

	/**
		Construct the class specifying the maximum number of events for each type individually.
	*/
	EventBufferConfig(
	    uint16_t maxBinaryEvents = 0,
	    uint16_t maxDoubleBinaryEvents = 0,
	    uint16_t maxAnalogEvents = 0,
	    uint16_t maxCounterEvents = 0,
	    uint16_t maxFrozenCounterEvents = 0,
	    uint16_t maxBinaryOutputStatusEvents = 0,
	    uint16_t maxAnalogOutputStatusEvents = 0,
	    uint16_t maxSecurityStatisticEvents = 0
	);

	/// Returns the sum of all event count maximums (number of elements in preallocated buffer)
	uint32_t TotalEvents() const;

	/// The number of binary events the outstation will buffer before overflowing
	uint16_t maxBinaryEvents;

	/// The number of double bit binary events the outstation will buffer before overflowing
	uint16_t maxDoubleBinaryEvents;

	/// The number of analog events the outstation will buffer before overflowing
	uint16_t maxAnalogEvents;

	/// The number of counter events the outstation will buffer before overflowing
	uint16_t maxCounterEvents;

	/// The number of frozen counter events the outstation will buffer before overflowing
	uint16_t maxFrozenCounterEvents;

	/// The number of binary output status events the outstation will buffer before overflowing
	uint16_t maxBinaryOutputStatusEvents;

	/// The number of analog output status events the outstation will buffer before overflowing
	uint16_t maxAnalogOutputStatusEvents;

	/// The number of security statistic events the outstation will buffer before overflowing
	uint16_t maxSecurityStatisticEvents;
};

}

#endif
