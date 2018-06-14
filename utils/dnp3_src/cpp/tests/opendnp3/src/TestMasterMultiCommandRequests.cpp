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
#include "mocks/MeasurementComparisons.h"

#include <testlib/HexConversions.h>

#include <dnp3mocks/CommandCallbackQueue.h>
#include <dnp3mocks/APDUHexBuilders.h>

#include <opendnp3/app/APDUResponse.h>
#include <opendnp3/app/APDUBuilders.h>

using namespace opendnp3;
using namespace openpal;

#define SUITE(name) "MasterMultiCommandRequestsTestSuite - " name

TEST_CASE(SUITE("command set ignores empty headers"))
{
	MasterTestObject t(NoStartupTasks());
	t.context->OnLowerLayerUp();

	ControlRelayOutputBlock crob(ControlCode::PULSE_ON);

	CommandSet commands;
	commands.Add<ControlRelayOutputBlock>({});
	commands.Add<ControlRelayOutputBlock>({ WithIndex(crob, 1) });

	CommandCallbackQueue queue;
	t.context->DirectOperate(std::move(commands), queue.Callback(), TaskConfig::Default());

	// writes just the 2nd call to Add()
	REQUIRE(t.lower->PopWriteAsHex() == "C0 05 0C 01 28 01 00 01 00 01 01 64 00 00 00 64 00 00 00 00");
}

TEST_CASE(SUITE("DirectOperateTwoCROB"))
{
	// Group 12 Var1, 1 byte count/index, index = 1, time on/off = 1000, CommandStatus::SUCCESS
	std::string crobstr = "0C 01 28 02 00 01 00 01 01 64 00 00 00 64 00 00 00 00 07 00 01 01 64 00 00 00 64 00 00 00 00";


	MasterTestObject t(NoStartupTasks());
	t.context->OnLowerLayerUp();

	ControlRelayOutputBlock crob(ControlCode::PULSE_ON);

	CommandSet commands;
	commands.Add<ControlRelayOutputBlock>({ WithIndex(crob, 1), WithIndex(crob, 7) });

	CommandCallbackQueue queue;
	t.context->DirectOperate(std::move(commands), queue.Callback(), TaskConfig::Default());

	REQUIRE(t.lower->PopWriteAsHex() ==  "C0 05 " + crobstr); // DO
	t.context->OnSendResult(true);
	t.SendToMaster("C0 81 00 00 " + crobstr);

	t.exe->RunMany();

	REQUIRE(t.lower->PopWriteAsHex() == ""); //nore more packets

	REQUIRE(queue.PopOnlyEqualValue(
	            TaskCompletion::SUCCESS,
	{
		CommandPointResult(0, 1, CommandPointState::SUCCESS, CommandStatus::SUCCESS),
		CommandPointResult(0, 7, CommandPointState::SUCCESS, CommandStatus::SUCCESS)
	}
	        ));
}

TEST_CASE(SUITE("SelectAndOperateTwoCROBSOneAO"))
{
	// Group 12 Var1, 1 byte count/index, index = 1, time on/off = 1000, CommandStatus::SUCCESS
	// Group 41 Var2 - index 8, value 0x1234
	std::string crobstr = "0C 01 28 02 00 01 00 01 01 64 00 00 00 64 00 00 00 00 07 00 01 01 64 00 00 00 64 00 00 00 00";
	std::string aostr = "29 02 28 01 00 08 00 34 12 00";
	std::string headers = crobstr + " " + aostr;


	MasterTestObject t(NoStartupTasks());
	t.context->OnLowerLayerUp();

	ControlRelayOutputBlock crob(ControlCode::PULSE_ON);
	AnalogOutputInt16 ao(0x1234);

	CommandSet commands;
	commands.Add<ControlRelayOutputBlock>({ WithIndex(crob, 1), WithIndex(crob, 7) });
	commands.Add<AnalogOutputInt16>({ WithIndex(ao, 8) });

	CommandCallbackQueue queue;
	t.context->SelectAndOperate(std::move(commands), queue.Callback(), TaskConfig::Default());

	REQUIRE(t.lower->PopWriteAsHex() == "C0 03 " + headers); // select
	t.context->OnSendResult(true);
	t.SendToMaster("C0 81 00 00 " + headers);

	t.exe->RunMany();

	REQUIRE(t.lower->PopWriteAsHex() == "C1 04 " + headers); // operate
	t.context->OnSendResult(true);
	t.SendToMaster("C1 81 00 00 " + headers);

	t.exe->RunMany();

	REQUIRE(t.lower->PopWriteAsHex() == ""); //nore more packets

	REQUIRE(queue.PopOnlyEqualValue(
	            TaskCompletion::SUCCESS,
	{
		CommandPointResult(0, 1, CommandPointState::SUCCESS, CommandStatus::SUCCESS),
		CommandPointResult(0, 7, CommandPointState::SUCCESS, CommandStatus::SUCCESS),
		CommandPointResult(1, 8, CommandPointState::SUCCESS, CommandStatus::SUCCESS)
	}
	        ));
}

