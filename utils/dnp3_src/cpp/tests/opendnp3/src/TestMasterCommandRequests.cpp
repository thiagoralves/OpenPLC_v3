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

// Group 12 Var1, 1 byte count/index, index = 1, time on/off = 1000, CommandStatus::SUCCESS
std::string crob = "0C 01 28 01 00 01 00 01 01 64 00 00 00 64 00 00 00 00";

#define SUITE(name) "MasterCommandRequestsTestSuite - " name

TEST_CASE(SUITE("ControlExecutionClosedState"))
{
	MasterParams params;
	MasterTestObject t(params);

	ControlRelayOutputBlock crob(ControlCode::PULSE_ON);
	CommandCallbackQueue queue;


	for (int i = 0; i < 10; ++i)
	{
		CommandSet commands({ WithIndex(crob, 7) });

		t.context->SelectAndOperate(std::move(commands), queue.Callback(), TaskConfig::Default());
		REQUIRE(1 == queue.values.size());

		auto& rsp = queue.values.front();
		REQUIRE(rsp.summary == TaskCompletion::FAILURE_NO_COMMS);

		// even though it has failed, there should be an entry for the command
		REQUIRE(rsp.results.size() == 1);
		REQUIRE(rsp.results[0].Equals(CommandPointResult(0, 7, CommandPointState::INIT, CommandStatus::UNDEFINED)));

		queue.values.pop_front();
	}
}

TEST_CASE(SUITE("ControlsTimeoutAfterStartPeriodElapses"))
{
	MasterParams params;
	params.taskStartTimeout = TimeDuration::Milliseconds(100); // set to an intentionally value so that is < response timeout
	MasterTestObject t(params);

	t.context->OnLowerLayerUp();

	REQUIRE(t.exe->RunMany() > 0);
	REQUIRE(t.lower->PopWriteAsHex() == hex::ClassTask(FunctionCode::DISABLE_UNSOLICITED, 0, ClassField::AllEventClasses()));

	// while we're waiting for a reponse, submit a control
	CommandCallbackQueue queue;

	for(int i = 0; i < 5; ++i)
	{
		CommandSet commands({ WithIndex(ControlRelayOutputBlock(ControlCode::PULSE_ON), 1) });
		t.context->SelectAndOperate(std::move(commands), queue.Callback(), TaskConfig::Default());
		t.exe->AdvanceTime(params.taskStartTimeout);
		REQUIRE(t.exe->RunMany() > 0);
		REQUIRE(1 == queue.values.size());
		REQUIRE(TaskCompletion::FAILURE_START_TIMEOUT == queue.values.front().summary);
		queue.values.pop_front();
	}
}

TEST_CASE(SUITE("SelectAndOperate"))
{
	MasterTestObject t(NoStartupTasks());
	t.context->OnLowerLayerUp();

	ControlRelayOutputBlock command(ControlCode::PULSE_ON);

	CommandCallbackQueue queue;
	t.context->SelectAndOperate(CommandSet({ WithIndex(command, 1) }), queue.Callback(), TaskConfig::Default());

	// Group 12 Var1, 1 byte count/index, index = 1, time on/off = 1000, CommandStatus::SUCCESS
	std::string crob = "0C 01 28 01 00 01 00 01 01 64 00 00 00 64 00 00 00 00";

	REQUIRE(t.lower->PopWriteAsHex() ==  "C0 03 " + crob); // SELECT
	t.context->OnSendResult(true);
	t.SendToMaster("C0 81 00 00 " + crob);

	t.exe->RunMany();

	REQUIRE(t.lower->PopWriteAsHex() == "C1 04 " + crob); // OPERATE
	t.context->OnSendResult(true);
	t.SendToMaster("C1 81 00 00 " + crob);

	t.exe->RunMany();

	REQUIRE(t.lower->PopWriteAsHex() == ""); //nore more packets

	REQUIRE(queue.PopOnlyEqualValue(
	            TaskCompletion::SUCCESS,
	            CommandPointResult(0, 1, CommandPointState::SUCCESS, CommandStatus::SUCCESS)
	        ));
}

TEST_CASE(SUITE("SelectAndOperateWithConfirmResponse"))
{
	MasterTestObject t(NoStartupTasks());
	t.context->OnLowerLayerUp();

	ControlRelayOutputBlock bo(ControlCode::PULSE_ON);

	CommandCallbackQueue queue;
	t.context->SelectAndOperate(CommandSet({ WithIndex(bo, 1) }), queue.Callback(), TaskConfig::Default());

	// Group 12 Var1, 1 byte count/index, index = 1, time on/off = 1000, CommandStatus::SUCCESS
	std::string crob = "0C 01 28 01 00 01 00 01 01 64 00 00 00 64 00 00 00 00";

	REQUIRE(t.lower->PopWriteAsHex() == "C0 03 " + crob); // SELECT
	t.context->OnSendResult(true);
	t.SendToMaster("E0 81 00 00 " + crob); // confirm bit set
	t.exe->RunMany();

	REQUIRE(t.lower->PopWriteAsHex() == "C0 00"); // confirm
	t.context->OnSendResult(true);
	t.exe->RunMany();

	REQUIRE(t.lower->PopWriteAsHex() == "C1 04 " + crob); // OPERATE
	t.context->OnSendResult(true);
	t.SendToMaster("C1 81 00 00 " + crob);
	t.exe->RunMany();

	REQUIRE(t.lower->PopWriteAsHex() == ""); //nore more packets
	REQUIRE(
	    queue.PopOnlyEqualValue(
	        TaskCompletion::SUCCESS,
	        CommandPointResult(0, 1, CommandPointState::SUCCESS, CommandStatus::SUCCESS)
	    )
	);
}

TEST_CASE(SUITE("ControlExecutionSelectResponseTimeout"))
{
	auto config = NoStartupTasks();
	MasterTestObject t(config);
	t.context->OnLowerLayerUp();

	CommandCallbackQueue queue;
	t.context->SelectAndOperate(
	    CommandSet({ WithIndex(ControlRelayOutputBlock(ControlCode::PULSE_ON), 1) }),
	    queue.Callback(), TaskConfig::Default()
	);

	REQUIRE(t.lower->PopWriteAsHex() == "C0 03 " + crob); // SELECT
	t.context->OnSendResult(true);

	t.exe->AdvanceTime(config.responseTimeout);
	t.exe->RunMany();

	REQUIRE(
	    queue.PopOnlyEqualValue(
	        TaskCompletion::FAILURE_RESPONSE_TIMEOUT,
	        CommandPointResult(0, 1, CommandPointState::INIT, CommandStatus::UNDEFINED)
	    )
	);
}

TEST_CASE(SUITE("ControlExecutionSelectLayerDown"))
{
	auto config = NoStartupTasks();
	MasterTestObject t(config);
	t.context->OnLowerLayerUp();

	CommandCallbackQueue queue;
	t.context->SelectAndOperate(
	    CommandSet({ WithIndex(ControlRelayOutputBlock(ControlCode::PULSE_ON), 1) }),
	    queue.Callback(), TaskConfig::Default()
	);

	REQUIRE(t.lower->PopWriteAsHex() == "C0 03 " + crob); // SELECT
	t.context->OnSendResult(true);

	t.context->OnLowerLayerDown();

	REQUIRE(queue.PopOnlyEqualValue(
	            TaskCompletion::FAILURE_NO_COMMS,
	            CommandPointResult(0, 1, CommandPointState::INIT, CommandStatus::UNDEFINED)
	        ));
}

TEST_CASE(SUITE("ControlExecutionSelectErrorResponse"))
{
	auto config = NoStartupTasks();
	MasterTestObject t(config);
	t.context->OnLowerLayerUp();

	CommandCallbackQueue queue;
	t.context->SelectAndOperate(
	    CommandSet({ WithIndex(ControlRelayOutputBlock(ControlCode::PULSE_ON), 1) }),
	    queue.Callback(), TaskConfig::Default()
	);

	t.context->OnSendResult(true);
	t.SendToMaster("C0 81 00 00 0C 01 28 01 00 01 00 01 01 64 00 00 00 64 00 00 00 04"); // not supported

	t.exe->RunMany();

	REQUIRE(queue.PopOnlyEqualValue(
	            TaskCompletion::SUCCESS,
	            CommandPointResult(0, 1, CommandPointState::SELECT_FAIL, CommandStatus::NOT_SUPPORTED)
	        ));
}

TEST_CASE(SUITE("ControlExecutionSelectBadFIRFIN"))
{
	auto config = NoStartupTasks();
	MasterTestObject t(config);
	t.context->OnLowerLayerUp();

	CommandCallbackQueue queue;

	t.context->SelectAndOperate(
	    CommandSet({ WithIndex(ControlRelayOutputBlock(ControlCode::PULSE_ON), 1) }),
	    queue.Callback(), TaskConfig::Default()
	);
	t.context->OnSendResult(true);

	t.SendToMaster("80 81 00 00 0C 01 28 01 00 01 00 01 01 64 00 00 00 64 00 00 00 00");

	t.exe->RunMany();

	REQUIRE(queue.PopOnlyEqualValue(
	            TaskCompletion::FAILURE_BAD_RESPONSE,
	            CommandPointResult(0, 1, CommandPointState::INIT, CommandStatus::UNDEFINED)
	        ));
}

TEST_CASE(SUITE("DeferredControlExecution"))
{
	MasterParams params;
	params.disableUnsolOnStartup = false;
	params.unsolClassMask = 0;
	MasterTestObject t(params);
	t.context->OnLowerLayerUp();

	t.exe->RunMany();

	// check that a read request was made on startup
	REQUIRE(t.lower->PopWriteAsHex() == hex::IntegrityPoll(0));
	t.context->OnSendResult(true);

	//issue a command while the master is waiting for a response from the outstation

	CommandCallbackQueue queue;
	t.context->SelectAndOperate(
	    CommandSet({ WithIndex(ControlRelayOutputBlock(ControlCode::PULSE_ON), 1) }),
	    queue.Callback(), TaskConfig::Default()
	);

	t.SendToMaster("C0 81 00 00"); //now master gets response to integrity

	t.exe->RunMany();

	REQUIRE(t.lower->PopWriteAsHex() == "C1 03 " + crob); //select
}

TEST_CASE(SUITE("CloseWhileWaitingForCommandResponse"))
{
	auto config = NoStartupTasks();
	MasterTestObject t(config);
	t.context->OnLowerLayerUp();

	AnalogOutputInt16 ao(100);
	CommandCallbackQueue queue;

	t.context->DirectOperate(
	    CommandSet({ WithIndex(ao, 1) }),
	    queue.Callback(), TaskConfig::Default()
	);

	REQUIRE(t.exe->RunMany() > 0);

	REQUIRE(t.lower->PopWriteAsHex() == "C0 05 29 02 28 01 00 01 00 64 00 00"); // DIRECT OPERATE
	REQUIRE(t.lower->NumWrites() == 0); //nore more packets
	REQUIRE(queue.values.empty());
	t.context->OnLowerLayerDown();

	REQUIRE(queue.PopOnlyEqualValue(
	            TaskCompletion::FAILURE_NO_COMMS,
	            CommandPointResult(0, 1, CommandPointState::INIT, CommandStatus::UNDEFINED)
	        ));
}

TEST_CASE(SUITE("ResponseTimeout"))
{
	MasterTestObject t(NoStartupTasks());
	t.context->OnLowerLayerUp();

	AnalogOutputInt16 ao(100);
	CommandCallbackQueue queue;

	t.context->DirectOperate(
	    CommandSet({ WithIndex(ao, 1) }),
	    queue.Callback(), TaskConfig::Default()
	);
	REQUIRE(t.exe->RunMany() > 0);

	REQUIRE(t.lower->PopWriteAsHex() == "C0 05 29 02 28 01 00 01 00 64 00 00"); // DIRECT OPERATE
	REQUIRE(t.lower->NumWrites() == 0); //nore more packets
	REQUIRE(queue.values.empty());

	REQUIRE(t.exe->AdvanceToNextTimer());

	REQUIRE(t.exe->RunMany() > 0);

	REQUIRE(queue.PopOnlyEqualValue(
	            TaskCompletion::FAILURE_RESPONSE_TIMEOUT,
	            CommandPointResult(0, 1, CommandPointState::INIT, CommandStatus::UNDEFINED)
	        ));
}

TEST_CASE(SUITE("SendCommandDuringFailedStartup"))
{
	auto params = NoStartupTasks();
	params.disableUnsolOnStartup = true;
	MasterTestObject t(params);
	t.context->OnLowerLayerUp();

	REQUIRE(t.exe->RunMany() > 0);

	REQUIRE(t.lower->PopWriteAsHex() == hex::ClassTask(FunctionCode::DISABLE_UNSOLICITED, 0, ClassField::AllEventClasses()));
	t.context->OnSendResult(true);
	REQUIRE(t.exe->AdvanceToNextTimer());
	REQUIRE(t.exe->RunMany() > 0);

	// while we're waiting for a response to the disable unsol, initiate a command seqeunce
	CommandCallbackQueue queue;
	AnalogOutputInt16 ao(100);
	t.context->DirectOperate(
	    CommandSet({ WithIndex(ao, 1) }),
	    queue.Callback(), TaskConfig::Default()
	);


	REQUIRE(t.lower->PopWriteAsHex() == "C1 05 29 02 28 01 00 01 00 64 00 00"); // DIRECT OPERATE
	REQUIRE(t.lower->NumWrites() == 0); //nore more packets
	REQUIRE(queue.values.empty());

	REQUIRE(t.exe->AdvanceToNextTimer());

	REQUIRE(t.exe->RunMany() > 0);

	REQUIRE(queue.PopOnlyEqualValue(
	            TaskCompletion::FAILURE_RESPONSE_TIMEOUT,
	            CommandPointResult(0, 1, CommandPointState::INIT, CommandStatus::UNDEFINED)
	        ));
}

template <class T>
void TestAnalogOutputExecution(const std::string& hex, const T& command)
{
	auto config = NoStartupTasks();
	MasterTestObject t(config);
	t.context->OnLowerLayerUp();

	CommandCallbackQueue queue;

	t.context->SelectAndOperate(CommandSet({WithIndex(command, 1)}), queue.Callback(), TaskConfig::Default());
	REQUIRE(t.exe->RunMany() > 0);

	REQUIRE((t.lower->PopWriteAsHex() == ("C0 03 " + hex)));
	t.context->OnSendResult(true);
	REQUIRE(queue.values.empty());
	t.SendToMaster("C0 81 00 00 " + hex);

	t.exe->RunMany();

	REQUIRE((t.lower->PopWriteAsHex() == ("C1 04 " + hex)));
	t.context->OnSendResult(true);
	REQUIRE(queue.values.empty());
	t.SendToMaster("C1 81 00 00 " + hex);

	t.exe->RunMany();

	REQUIRE(queue.PopOnlyEqualValue(
	            TaskCompletion::SUCCESS,
	            CommandPointResult(0, 1, CommandPointState::SUCCESS, CommandStatus::SUCCESS)
	        ));
}

TEST_CASE(SUITE("SingleSetpointExecution"))// Group 41 Var4
{
	// 100.0
	TestAnalogOutputExecution("29 03 28 01 00 01 00 00 00 C8 42 00", AnalogOutputFloat32(100.0f));

	// 95.6
	TestAnalogOutputExecution("29 03 28 01 00 01 00 33 33 BF 42 00", AnalogOutputFloat32(95.6f));
}

TEST_CASE(SUITE("DoubleSetpointExecution"))
{
	TestAnalogOutputExecution("29 04 28 01 00 01 00 00 00 00 E7 FF FF 58 48 00", AnalogOutputDouble64(SingleFloat::Max * 100.0));
}

TEST_CASE(SUITE("Int32SetpointExecution"))
{
	// Group 41 Var1, Int32, 65536
	TestAnalogOutputExecution("29 01 28 01 00 01 00 00 00 01 00 00", AnalogOutputInt32(65536));
}

TEST_CASE(SUITE("Int16SetpointExecution"))
{
	// Group 41 Var2, Int16, 100
	TestAnalogOutputExecution("29 02 28 01 00 01 00 64 00 00", AnalogOutputInt16(100));
}



