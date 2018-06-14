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
#ifndef OPENDNP3_EVENTTYPE_H
#define OPENDNP3_EVENTTYPE_H

#include <cstdint>

namespace opendnp3
{

static const int NUM_OUTSTATION_EVENT_TYPES = 8;

enum class EventType : uint16_t
{
	Binary = 0,
	Analog = 1,
	Counter = 2,
	FrozenCounter = 3,
	DoubleBitBinary = 4,
	BinaryOutputStatus = 5,
	AnalogOutputStatus = 6,
	SecurityStat = 7
};

enum class EventClass : uint8_t
{
	EC1 = 0,
	EC2 = 1,
	EC3 = 2
};

}

#endif
