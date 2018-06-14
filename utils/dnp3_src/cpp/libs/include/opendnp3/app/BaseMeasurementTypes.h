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
#ifndef OPENDNP3_BASEMEASUREMENTTYPES_H
#define OPENDNP3_BASEMEASUREMENTTYPES_H

#include <cstdint>

#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/Flags.h"

namespace opendnp3
{

/**
  Base class shared by all of the DataPoint types.
*/
class Measurement
{
public:

	Flags flags;		//	bitfield that stores type specific quality information
	DNPTime time;		//	timestamp associated with the measurement

protected:

	Measurement()
	{}

	Measurement(Flags flags) : flags(flags)
	{}

	Measurement(Flags flags, DNPTime time) : flags(flags), time(time)
	{}

};


/// Common subclass to analogs and counters
template <class T>
class TypedMeasurement : public Measurement
{
public:

	T value;

	typedef T Type;

protected:

	TypedMeasurement(): Measurement(), value(0) {}
	TypedMeasurement(Flags flags) : Measurement(flags), value(0) {}
	TypedMeasurement(T value, Flags flags) : Measurement(flags), value(value) {}
	TypedMeasurement(T value, Flags flags, DNPTime time) : Measurement(flags, time), value(value) {}
};

}


#endif
