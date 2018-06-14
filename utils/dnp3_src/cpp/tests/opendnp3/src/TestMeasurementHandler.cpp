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

#include <opendnp3/master/MeasurementHandler.h>

#include <testlib/BufferHelpers.h>

#include <testlib/MockLogHandler.h>
#include <dnp3mocks/MockSOEHandler.h>

#include <functional>

using namespace openpal;
using namespace opendnp3;
using namespace testlib;

/**
* Test that the measurement handler correctly interprets measurement values in response data
*/
#define SUITE(name) "MeasurementHandlerTestSuite - " name

// Parse some input and verify that the ISOEHandler is invoked with expected results
ParseResult TestObjectHeaders(const std::string& objects, ParseResult expectedResult, const std::function<void(MockSOEHandler&)>& verify);

TEST_CASE(SUITE("accepts empty response"))
{
	auto verify = [](MockSOEHandler & soe)
	{
		REQUIRE(soe.TotalReceived() == 0);
	};

	TestObjectHeaders("", ParseResult::OK, verify);
}

TEST_CASE(SUITE("parses g121v1 correctly"))
{
	auto verify = [](MockSOEHandler & soe)
	{
		REQUIRE(soe.TotalReceived() == 1);

		auto& stat = soe.securityStatSOE[2];

		REQUIRE(stat.info.tsmode == TimestampMode::INVALID);
		REQUIRE(stat.info.gv == GroupVariation::Group121Var1);
		REQUIRE_FALSE(stat.info.isEventVariation);
		REQUIRE(stat.meas.value.count == 8);
		REQUIRE(stat.meas.value.assocId == 7);
	};

	// g121v1 - 1 byte start/stop - 2->2 - flags: 0x01, assoc = 0x0007, count = 0x00000008
	auto header = "79 01 00 02 02 01 07 00 08 00 00 00";

	TestObjectHeaders(header, ParseResult::OK, verify);
}

TEST_CASE(SUITE("parses g122v1 correctly"))
{
	auto verify = [](MockSOEHandler & soe)
	{
		REQUIRE(soe.TotalReceived() == 1);

		auto& stat = soe.securityStatSOE[3];

		REQUIRE(stat.info.tsmode == TimestampMode::INVALID);
		REQUIRE(stat.info.gv == GroupVariation::Group122Var1);
		REQUIRE(stat.meas.value.count == 8);
		REQUIRE(stat.meas.value.assocId == 7);
	};

	// g122v1 - 1 byte count and prefix - 1 count - index: 3, flags: 0x01, assoc = 7, count = 8
	auto header = "7A 01 17 01 03 01 07 00 08 00 00 00";

	TestObjectHeaders(header, ParseResult::OK, verify);
}

TEST_CASE(SUITE("parses g122v2 correctly"))
{
	auto verify = [](MockSOEHandler & soe)
	{
		REQUIRE(soe.TotalReceived() == 1);

		auto& stat = soe.securityStatSOE[3];

		REQUIRE(stat.info.tsmode == TimestampMode::SYNCHRONIZED);
		REQUIRE(stat.info.gv == GroupVariation::Group122Var2);
		REQUIRE(stat.info.isEventVariation);
		REQUIRE(stat.meas.value.count == 8);
		REQUIRE(stat.meas.value.assocId == 7);
		REQUIRE(stat.meas.time == 9);
	};

	// g122v1 - 1 byte count and prefix - 1 count - index: 3, flags: 0x01, assoc = 7, count = 8, time = 9
	auto header = "7A 02 17 01 03 01 07 00 08 00 00 00 09 00 00 00 00 00";

	TestObjectHeaders(header, ParseResult::OK, verify);
}

TEST_CASE(SUITE("parses g50v1 correctly"))
{
	auto verify = [](MockSOEHandler & soe)
	{
		REQUIRE(soe.TotalReceived() == 2);
		REQUIRE(soe.timeSOE.size() == 2);
		REQUIRE(soe.timeSOE[0].value == 0xABABABABABAB);
		REQUIRE(soe.timeSOE[1].value == 0xBCBCBCBCBCBC);
	};

	// g50v1 count of 2
	auto objects = "32 01 07 02 AB AB AB AB AB AB BC BC BC BC BC BC";

	TestObjectHeaders(objects, ParseResult::OK, verify);
}

ParseResult TestObjectHeaders(const std::string& objects, ParseResult expectedResult, const std::function<void(MockSOEHandler&)>& verify)
{
	MockSOEHandler soe;
	testlib::MockLogHandler log;

	HexSequence hex(objects);

	auto result = MeasurementHandler::ProcessMeasurements(hex.ToRSlice(), log.logger, &soe);
	REQUIRE(result == expectedResult);
	verify(soe);
	return result;
}