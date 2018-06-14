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

#include "mocks/OutstationTestObject.h"

#include <dnp3mocks/APDUHexBuilders.h>
#include <testlib/HexConversions.h>

using namespace std;
using namespace opendnp3;
using namespace openpal;

#define SUITE(name) "OutstationAssignClassTestSuite - " name

TEST_CASE(SUITE("RejectsWithFuncNotSupportedIfAppDoesNotSupport"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// assign binaries to class 2
	t.SendToOutstation("C0 16 3C 03 06 01 00 06");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 01");

	REQUIRE(t.application->classAssignments.empty());
}

TEST_CASE(SUITE("RejectsWithParamErrorIfNoType"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.application->supportsAssignClass = true;
	t.LowerLayerUp();

	// assign binaries to class 2
	t.SendToOutstation("C0 16 3C 03 06 01 00 06");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 04");
	REQUIRE(t.application->classAssignments.empty());
}

TEST_CASE(SUITE("AcceptsAssignClassViaAllObjects"))
{
	const uint16_t NUM_BINARY = 5;

	OutstationConfig config;
	OutstationTestObject t(config, DatabaseSizes::BinaryOnly(NUM_BINARY));
	t.application->supportsAssignClass = true;
	t.LowerLayerUp();

	// assign binaries to class 2
	t.SendToOutstation("C0 16 3C 03 06 01 00 06");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00");

	auto view = t.context.GetConfigView();
	for (uint16_t i = 0; i < NUM_BINARY; ++i)
	{
		REQUIRE(view.binaries[0].config.clazz == PointClass::Class2);
	}

	REQUIRE(t.application->classAssignments.size() == 1);
	auto assignment = t.application->classAssignments.front();
	REQUIRE(assignment == std::make_tuple(AssignClassType::BinaryInput, PointClass::Class2, 0, 4));
}

TEST_CASE(SUITE("RejectsAssignClassWithParamErrorIfRangeIsInvalid"))
{
	const uint16_t NUM_BINARY = 5;

	OutstationConfig config;
	OutstationTestObject t(config, DatabaseSizes::BinaryOnly(NUM_BINARY));
	t.application->supportsAssignClass = true;
	t.LowerLayerUp();

	// assign binaries 0 -> 5 (invalid range) to class 2
	t.SendToOutstation("C0 16 3C 03 06 01 00 01 00 00 05 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 04");

	//despite the invalid range, the outstation should assign the values that it does have
	auto view = t.context.GetConfigView();
	for (uint16_t i = 0; i < NUM_BINARY; ++i)
	{
		REQUIRE(view.binaries[0].config.clazz == PointClass::Class2);
	}

	REQUIRE(t.application->classAssignments.size() == 1);
}

TEST_CASE(SUITE("AcceptsAssignClassViaStartStop"))
{
	const uint16_t NUM_BINARY = 5;

	OutstationConfig config;
	OutstationTestObject t(config, DatabaseSizes::BinaryOnly(NUM_BINARY));
	t.application->supportsAssignClass = true;
	t.LowerLayerUp();

	// assign binaries 2 - 3 to class 2
	t.SendToOutstation("C0 16 3C 03 06 01 00 01 02 00 03 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00");

	auto view = t.context.GetConfigView();

	REQUIRE(view.binaries[0].config.clazz == PointClass::Class1);
	for (uint16_t i = 2; i < 3; ++i)
	{
		REQUIRE(view.binaries[i].config.clazz == PointClass::Class2);
	}
	REQUIRE(view.binaries[4].config.clazz == PointClass::Class1);

	REQUIRE(t.application->classAssignments.size() == 1);
	auto assignment = t.application->classAssignments.front();
	REQUIRE(assignment == std::make_tuple(AssignClassType::BinaryInput, PointClass::Class2, 2, 3));
}

TEST_CASE(SUITE("AcceptsMultipleAssignsmentPerMessage"))
{
	const uint16_t NUM_BINARY = 5;
	const uint16_t NUM_ANALOG = 10;

	OutstationConfig config;
	OutstationTestObject t(config, DatabaseSizes(NUM_BINARY, 0, NUM_ANALOG, 0, 0, 0, 0, 0));
	t.application->supportsAssignClass = true;
	t.LowerLayerUp();

	// assign binaries 2 - 3 to class 2 - assign all analogs to class 3
	t.SendToOutstation("C0 16 3C 03 06 01 00 01 02 00 03 00 3C 04 06 1E 00 06");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00");

	REQUIRE(t.application->classAssignments.size() == 2);
	auto assignment = t.application->classAssignments.front();
	REQUIRE(assignment == std::make_tuple(AssignClassType::BinaryInput, PointClass::Class2, 2, 3));
	t.application->classAssignments.pop_front();
	assignment = t.application->classAssignments.front();
	REQUIRE(assignment == std::make_tuple(AssignClassType::AnalogInput, PointClass::Class3, 0, 9));

}