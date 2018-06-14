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
#include "EventCount.h"

#include "opendnp3/gen/PointClass.h"

namespace opendnp3
{

EventCount::EventCount() : total(0)
{
	this->Clear();
}

EventCount::EventCount(const EventCount& ec) : total(ec.total)
{
	for (uint16_t clazz = 0; clazz < NUM_CLASSES; ++clazz)
	{
		numOfClass[clazz] = ec.numOfClass[clazz];
	}


	for (uint16_t type = 0; type < NUM_TYPES; ++type)
	{
		numOfType[type] = ec.numOfType[type];
	}
}

EventCount& EventCount::operator=(const EventCount& ec)
{
	if (this != &ec)
	{
		this->total = ec.total;

		for (uint16_t clazz = 0; clazz < NUM_CLASSES; ++clazz)
		{
			numOfClass[clazz] = ec.numOfClass[clazz];
		}


		for (uint16_t type = 0; type < NUM_TYPES; ++type)
		{
			numOfType[type] = ec.numOfType[type];
		}
	}

	return *this;
}

void EventCount::Clear()
{
	total = 0;

	for (uint16_t clazz = 0; clazz < NUM_CLASSES; ++clazz)
	{
		numOfClass[clazz] = 0;
	}


	for (uint16_t type = 0; type < NUM_TYPES; ++type)
	{
		numOfType[type] = 0;
	}
}

ClassField EventCount::ToClassField() const
{
	bool class1 = this->NumOfClass(EventClass::EC1) > 0;
	bool class2 = this->NumOfClass(EventClass::EC2) > 0;
	bool class3 = this->NumOfClass(EventClass::EC3) > 0;

	return ClassField(false, class1, class2, class3);
}

uint32_t EventCount::NumOfClass(ClassField field) const
{
	uint32_t ret = 0;

	if (field.HasClass1()) ret += NumOfClass(EventClass::EC1);
	if (field.HasClass2()) ret += NumOfClass(EventClass::EC2);
	if (field.HasClass3()) ret += NumOfClass(EventClass::EC3);

	return ret;
}

uint32_t EventCount::NumOfClass(EventClass clazz) const
{
	return numOfClass[static_cast<uint8_t>(clazz)];
}

uint32_t EventCount::NumOfType(EventType type) const
{
	return numOfType[static_cast<uint8_t>(type)];
}

bool EventCount::IsEmpty() const
{
	return ToClassField().IsEmpty();
}

void EventCount::Increment(EventClass clazz, EventType type)
{
	++total;
	++numOfClass[static_cast<int>(clazz)];
	++numOfType[static_cast<int>(type)];
}

void EventCount::Decrement(EventClass clazz, EventType type)
{
	--total;
	--numOfType[static_cast<int>(type)];
	--numOfClass[static_cast<int>(clazz)];
}

}
