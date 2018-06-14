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

#include "mocks/MasterTestObject.h"

#include <dnp3mocks/APDUHexBuilders.h>
#include <testlib/HexConversions.h>

using namespace std;
using namespace opendnp3;
using namespace openpal;

#define SUITE(name) "MasterAssignClassTestSuite - " name

TEST_CASE(SUITE("AssignsClassAfterConnect"))
{
	MasterTestObject t(NoStartupTasks());

	// configure the mock application to do assign class on startup
	t.application->assignClassHeaders.push_back(Header::AllObjects(60, 2));
	t.application->assignClassHeaders.push_back(Header::AllObjects(3, 0));

	t.context->OnLowerLayerUp();

	REQUIRE(t.exe->RunMany() > 0);

	REQUIRE(t.lower->PopWriteAsHex() == "C0 16 3C 02 06 03 00 06");
	t.context->OnSendResult(true);
	t.SendToMaster("C0 81 00 00");

	t.exe->RunMany();
	REQUIRE(t.exe->NumPendingTimers() == 1);
	REQUIRE(t.lower->PopWriteAsHex() == "");

	REQUIRE(t.application->taskStartEvents.size() == 1);
	REQUIRE(t.application->taskStartEvents[0] == MasterTaskType::ASSIGN_CLASS);

	REQUIRE(t.application->taskCompletionEvents.size() == 1);
	REQUIRE(t.application->taskCompletionEvents[0].result == TaskCompletion::SUCCESS);
}

TEST_CASE(SUITE("DisableUnsolBeforeAssignClass"))
{
	auto params = NoStartupTasks();
	params.disableUnsolOnStartup = true;
	MasterTestObject t(params);

	// configure the mock application to do assign class on startup
	t.application->assignClassHeaders.push_back(Header::AllObjects(60, 2));
	t.application->assignClassHeaders.push_back(Header::AllObjects(3, 0));

	t.context->OnLowerLayerUp();

	REQUIRE(t.exe->RunMany() > 0);

	REQUIRE(t.lower->PopWriteAsHex() == hex::ClassTask(FunctionCode::DISABLE_UNSOLICITED, 0, ClassField::AllEventClasses()));

}