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
#include <catch.hpp>

#include "mocks/MeasurementComparisons.h"
#include "mocks/DatabaseTestObject.h"

#include <opendnp3/app/QualityMasks.h>

#include <limits>

using namespace std;
using namespace openpal;
using namespace opendnp3;

template <class Spec>
void TestBufferForEvent(bool isEvent, const typename Spec::meas_t& newVal, DatabaseTestObject& test, std::deque< Event <Spec> >& queue)
{
	test.db.Update(newVal, 0);

	if(isEvent)
	{
		REQUIRE((queue.size() ==  1));
		REQUIRE((newVal == queue.front().value));
		REQUIRE((0 ==  queue.front().index));
		queue.pop_front();
	}
	else
	{
		REQUIRE((queue.size() ==  0));
	}
}

bool IsAnalogEvent(double val1, double val2, double deadband)
{
	Analog a1(val1);
	Analog a2(val2);
	return AnalogSpec::IsEvent(a1, a2, deadband);
}

bool IsCounterEvent(uint32_t val1, uint32_t val2, uint32_t deadband)
{
	Counter c1(val1);
	Counter c2(val2);
	return CounterSpec::IsEvent(c1, c2, deadband);
}

#define SUITE(name) "DatabaseTestSuite - " name

// tests for the various analog event conditions
TEST_CASE(SUITE("AnalogEventZeroDeadband"))
{
	REQUIRE_FALSE(IsAnalogEvent(0, 0, 0));
}

TEST_CASE(SUITE("AnalogEventOnDeadband"))
{
	REQUIRE_FALSE(IsAnalogEvent(0, 1, 1));
}

TEST_CASE(SUITE("AnalogEventNegative"))
{
	REQUIRE(IsAnalogEvent(-34, -36, 1));
}

TEST_CASE(SUITE("AnalogInfiniteChange"))
{
	auto maxAnalog = std::numeric_limits<double>::max();
	auto minAnalog = -maxAnalog;

	REQUIRE(IsAnalogEvent(minAnalog, maxAnalog, 1));
}

TEST_CASE(SUITE("CounterMaxChange"))
{
	auto max = std::numeric_limits<uint32_t>::max();
	REQUIRE(IsCounterEvent(0, max, max - 1));
}

TEST_CASE(SUITE("AnalogNoEventNegative"))
{
	REQUIRE_FALSE(IsAnalogEvent(-34, -36, 2));
}

// Next 3 tests prove that "no change" doesn't get forwared to IEventBuffer
TEST_CASE(SUITE("BinaryNoChange"))
{
	DatabaseTestObject t(DatabaseSizes::BinaryOnly(1));
	auto view = t.db.GetConfigView();
	view.binaries[0].config.clazz = PointClass::Class1;
	TestBufferForEvent<BinarySpec>(false, Binary(false, ToUnderlying(BinaryQuality::RESTART)), t, t.buffer.binaryEvents);
}

TEST_CASE(SUITE("AnalogNoChange"))
{
	DatabaseTestObject t(DatabaseSizes::AnalogOnly(1));
	auto view = t.db.GetConfigView();
	view.analogs[0].config.clazz = PointClass::Class1;
	TestBufferForEvent(false, Analog(0, ToUnderlying(AnalogQuality::RESTART)), t, t.buffer.analogEvents);
}

TEST_CASE(SUITE("CounterNoChange"))
{
	DatabaseTestObject t(DatabaseSizes::CounterOnly(1));
	auto view = t.db.GetConfigView();
	view.counters[0].config.clazz = PointClass::Class1;
	TestBufferForEvent(false, Counter(0, ToUnderlying(CounterQuality::RESTART)), t, t.buffer.counterEvents);
}

// Next 3 tests prove that a change detection will forward to the buffer
TEST_CASE(SUITE("BinaryChange"))
{
	DatabaseTestObject t(DatabaseSizes::BinaryOnly(1));
	auto view = t.db.GetConfigView();
	view.binaries[0].config.clazz = PointClass::Class1;
	TestBufferForEvent(true, Binary(false, ToUnderlying(BinaryQuality::ONLINE)), t, t.buffer.binaryEvents);
}

TEST_CASE(SUITE("AnalogChange"))
{
	DatabaseTestObject t(DatabaseSizes::AnalogOnly(1));
	auto view = t.db.GetConfigView();
	view.analogs[0].config.clazz = PointClass::Class1;
	TestBufferForEvent(true, Analog(0, ToUnderlying(AnalogQuality::ONLINE)), t, t.buffer.analogEvents);
}

TEST_CASE(SUITE("CounterChange"))
{
	DatabaseTestObject t(DatabaseSizes::CounterOnly(1));
	auto view = t.db.GetConfigView();
	view.counters[0].config.clazz = PointClass::Class1;
	TestBufferForEvent(true, Counter(0, ToUnderlying(CounterQuality::ONLINE)), t, t.buffer.counterEvents);
}

//show that the last reported change gets recorded correctly and applied correctly for each type
TEST_CASE(SUITE("AnalogLastReportedChange"))
{
	DatabaseTestObject t(DatabaseSizes::AnalogOnly(1));
	auto view = t.db.GetConfigView();
	view.analogs[0].config.clazz = PointClass::Class1;
	view.analogs[0].config.deadband = 5; //value must change by more than 5 before being reported

	TestBufferForEvent(false, Analog(-2, ToUnderlying(AnalogQuality::RESTART)), t, t.buffer.analogEvents);
	TestBufferForEvent(false, Analog(5, ToUnderlying(AnalogQuality::RESTART)), t, t.buffer.analogEvents);
	TestBufferForEvent(true, Analog(6, ToUnderlying(AnalogQuality::RESTART)), t, t.buffer.analogEvents); //change by 6, so 6 should get recorded
	TestBufferForEvent(false, Analog(1, ToUnderlying(AnalogQuality::RESTART)), t, t.buffer.analogEvents);
	TestBufferForEvent(true, Analog(-1, ToUnderlying(AnalogQuality::RESTART)), t, t.buffer.analogEvents);
}

TEST_CASE(SUITE("CounterLastReportedChange"))
{
	DatabaseTestObject t(DatabaseSizes::CounterOnly(1));
	auto view = t.db.GetConfigView();
	view.counters[0].config.clazz = PointClass::Class1;
	view.counters[0].config.deadband = 5;

	TestBufferForEvent(false, Counter(1, ToUnderlying(CounterQuality::RESTART)), t, t.buffer.counterEvents);
	TestBufferForEvent(false, Counter(5, ToUnderlying(CounterQuality::RESTART)), t, t.buffer.counterEvents);
	TestBufferForEvent(true, Counter(6, ToUnderlying(CounterQuality::RESTART)), t, t.buffer.counterEvents); //change by 6, so 6 should get recorded
	TestBufferForEvent(false, Counter(1, ToUnderlying(CounterQuality::RESTART)), t, t.buffer.counterEvents);
	TestBufferForEvent(true, Counter(0, ToUnderlying(CounterQuality::RESTART)), t, t.buffer.counterEvents);
}


