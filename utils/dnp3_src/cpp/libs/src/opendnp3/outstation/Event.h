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
#ifndef OPENDNP3_EVENT_H
#define OPENDNP3_EVENT_H

#include "opendnp3/app/EventType.h"

namespace opendnp3
{

struct Evented
{
	Evented(uint16_t index_, EventClass clazz_) : index(index_), clazz(clazz_)
	{}

	Evented() : clazz(EventClass::EC1)
	{}

	uint16_t index;
	EventClass clazz;	// class of the event (CLASS<1-3>)
};

/**
 * Record of an event that includes value, index, and class
 */
template <typename Spec>
struct Event : public Evented
{
	typedef typename Spec::meas_t meas_type_t;

	Event(const meas_type_t& value, uint16_t index, EventClass clazz, typename Spec::event_variation_t variation) :
		Evented(index, clazz),
		value(value),
		variation(variation)
	{}

	Event() : Evented(), value(), variation()
	{}

	meas_type_t value;
	typename Spec::event_variation_t variation;
};

} //end namespace



#endif

