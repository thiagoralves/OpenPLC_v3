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
#ifndef __DATABASE_TEST_OBJECT_H_
#define __DATABASE_TEST_OBJECT_H_

#include <queue>

#include <opendnp3/outstation/Event.h>
#include <opendnp3/outstation/Database.h>

namespace opendnp3
{

class MockEventBuffer final : public IEventReceiver
{
public:

	void Update(const Event<BinarySpec>& evt) override
	{
		binaryEvents.push_back(evt);
	}

	void Update(const Event<DoubleBitBinarySpec>& evt) override
	{
		doubleBinaryEvents.push_back(evt);
	}

	void Update(const Event<AnalogSpec>& evt) override
	{
		analogEvents.push_back(evt);
	}

	void Update(const Event<CounterSpec>& evt) override
	{
		counterEvents.push_back(evt);
	}

	void Update(const Event<FrozenCounterSpec>& evt) override
	{
		frozenCounterEvents.push_back(evt);
	}

	void Update(const Event<BinaryOutputStatusSpec>& evt) override
	{
		binaryOutputStatusEvents.push_back(evt);
	}

	void Update(const Event<AnalogOutputStatusSpec>& evt) override
	{
		analogOutputStatusEvents.push_back(evt);
	}

	std::deque<Event<BinarySpec>> binaryEvents;
	std::deque<Event<DoubleBitBinarySpec>> doubleBinaryEvents;
	std::deque<Event<AnalogSpec>> analogEvents;
	std::deque<Event<CounterSpec>> counterEvents;
	std::deque<Event<FrozenCounterSpec>> frozenCounterEvents;
	std::deque<Event<BinaryOutputStatusSpec>> binaryOutputStatusEvents;
	std::deque<Event<AnalogOutputStatusSpec>> analogOutputStatusEvents;
	std::deque<Event<SecurityStatSpec>> securityStatEvents;
};

class DatabaseTestObject
{
public:

	DatabaseTestObject(const DatabaseSizes& dbSizes, IndexMode mode = IndexMode::Contiguous, StaticTypeBitField allowedClass0 = StaticTypeBitField::AllTypes()) :
		buffer(),
		db(dbSizes, buffer, mode, allowedClass0)
	{

	}


	MockEventBuffer buffer;
	Database db;
};

}

#endif

