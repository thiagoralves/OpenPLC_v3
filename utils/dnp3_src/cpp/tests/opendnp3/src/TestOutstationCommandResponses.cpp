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

using namespace std;
using namespace opendnp3;
using namespace openpal;

#define SUITE(name) "OutstationCommandResponsesTestSuite - " name

TEST_CASE(SUITE("SelectCROBNotSupported"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	t.cmdHandler->SetResponse(CommandStatus::NOT_SUPPORTED);

	// Select group 12 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");

	// conformance requires IIN 2.2 to be set whenever the command status is not supported
	REQUIRE(t.lower->PopWriteAsHex() ==  "C0 81 80 04 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 04"); // 0x04 status == CommandStatus::NOT_SUPPORTED

	REQUIRE(t.cmdHandler->numStart == 1);
	REQUIRE(t.cmdHandler->numEnd == 1);
}

TEST_CASE(SUITE("UnknownCodeIsEchoed"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	t.cmdHandler->SetResponse(CommandStatus::NOT_SUPPORTED);

	// Select group 12 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 0C 01 17 01 03 AA 01 01 00 00 00 01 00 00 00 00");

	// conformance requires IIN 2.2 to be set whenever the command status is not supported
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 04 0C 01 17 01 03 AA 01 01 00 00 00 01 00 00 00 04"); // 0x04 status == CommandStatus::NOT_SUPPORTED
}

TEST_CASE(SUITE("SelectCROBTooMany"))
{
	OutstationConfig config;
	config.params.maxControlsPerRequest = 1;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 12 Var 1, count = 2, index = 3 & 4
	t.SendToOutstation("C0 03 0C 01 17 02 03 01 01 01 00 00 00 01 00 00 00 00 04 01 01 01 00 00 00 01 00 00 00 00");
	auto expected = "C0 81 80 00 0C 01 17 02 03 01 01 01 00 00 00 01 00 00 00 00 04 01 01 01 00 00 00 01 00 00 00 08";
	REQUIRE(t.lower->PopWriteAsHex() == expected); // 0x08 status == CommandStatus::TOO_MANY_OBJS
}

TEST_CASE(SUITE("SelectOperateCROB"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 12 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() ==  "C0 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);

	// operate
	t.SendToOutstation("C1 04 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	t.OnSendResult(true);
}

TEST_CASE(SUITE("SelectRetryAndOperateCROB"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 12 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);

	// Select group 12 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);

	// operate
	t.SendToOutstation("C1 04 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	t.OnSendResult(true);
}

TEST_CASE(SUITE("SelectOperateTimeout"))
{
	OutstationConfig config;
	config.params.selectTimeout = TimeDuration::Seconds(5);
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 12 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);

	t.AdvanceTime(TimeDuration::Milliseconds(5001));

	// operate
	t.SendToOutstation("C1 04 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 01"); // 0x01 timeout
	t.OnSendResult(true);
}

TEST_CASE(SUITE("SelectOperateGapInSequenceNumber"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 12 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);

	// operate
	t.SendToOutstation("C2 04 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C2 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 02"); // 0x02 no select
	t.OnSendResult(true);
}

TEST_CASE(SUITE("SelectOperateSameSequenceNumber"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 12 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);

	// operate
	t.SendToOutstation("C0 04 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 02"); // 0x02 no select
	t.OnSendResult(true);
}

TEST_CASE(SUITE("SelectOperateNonMatchingRequests"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 12 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);

	// operate
	t.SendToOutstation("C1 04 0C 01 17 01 04 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 0C 01 17 01 04 01 01 01 00 00 00 01 00 00 00 02"); // 0x02 no select
	t.OnSendResult(true);
}

TEST_CASE(SUITE("SelectOperateCROBSameSequenceNumber"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	REQUIRE(0 ==  t.cmdHandler->NumInvocations());

	// Select group 12 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() ==  "C0 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	REQUIRE(1 == t.cmdHandler->NumInvocations());
	t.OnSendResult(true);

	// operate the first time with correct sequence #
	t.SendToOutstation("C1 04 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(2 == t.cmdHandler->NumInvocations());
	t.OnSendResult(true);

	// operate again with same sequence number, should respond success but not really do an operation
	t.SendToOutstation("C1 04 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00");
	REQUIRE(2 == t.cmdHandler->NumInvocations());
	t.OnSendResult(true);
}

TEST_CASE(SUITE("SelectGroup41Var1"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 41 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 29 01 17 01 03 00 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 29 01 17 01 03 00 00 00 00 00");
}

TEST_CASE(SUITE("SelectGroup41Var2"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 41 Var 2, count = 1, index = 3
	t.SendToOutstation("C0 03 29 02 17 01 03 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 29 02 17 01 03 00 00 00");
}

TEST_CASE(SUITE("SelectGroup41Var3"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 41 Var 3, count = 1, index = 1, value = 100.0
	t.SendToOutstation("C0 03 29 03 17 01 01 00 00 C8 42 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 29 03 17 01 01 00 00 C8 42 00");
}

TEST_CASE(SUITE("SelectGroup41Var4"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 41 Var 4, count = 1, index = 1, value = 100.0
	t.SendToOutstation("C0 03 29 04 17 01 01 00 00 00 00 00 00 59 40 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 29 04 17 01 01 00 00 00 00 00 00 59 40 00");
}

TEST_CASE(SUITE("SelectOperateGroup41Var1"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 41 Var 1, count = 1, index = 3
	t.SendToOutstation("C0 03 29 01 17 01 03 00 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 29 01 17 01 03 00 00 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);

	// Select group 41 Var 1, count = 1, index = 3
	t.SendToOutstation("C1 04 29 01 17 01 03 00 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 29 01 17 01 03 00 00 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);

}

TEST_CASE(SUITE("SelectOperateGroup41Var2"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 41 Var 2, count = 1, index = 3
	t.SendToOutstation("C0 03 29 02 17 01 03 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 29 02 17 01 03 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);


	// Select group 41 Var 1, count = 1, index = 3
	t.SendToOutstation("C1 04 29 02 17 01 03 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 29 02 17 01 03 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);

}

TEST_CASE(SUITE("SelectOperateGroup41Var3"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 41 Var 3, count = 1, index = 1
	t.SendToOutstation("C0 03 29 03 17 01 01 00 00 C8 42 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 29 03 17 01 01 00 00 C8 42 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);

	// operate group 41 Var 3, count = 1, index = 1
	t.SendToOutstation("C1 04 29 03 17 01 01 00 00 C8 42 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 29 03 17 01 01 00 00 C8 42 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);
}

TEST_CASE(SUITE("SelectOperateGroup41Var4"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Select group 41 Var 4, count = 1, index = 1
	t.SendToOutstation("C0 03 29 04 17 01 01 00 00 00 00 00 00 59 40 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C0 81 80 00 29 04 17 01 01 00 00 00 00 00 00 59 40 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);


	// operate group 41 Var 4, count = 1, index = 1
	t.SendToOutstation("C1 04 29 04 17 01 01 00 00 00 00 00 00 59 40 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 29 04 17 01 01 00 00 00 00 00 00 59 40 00"); // 0x00 status == CommandStatus::SUCCESS
	t.OnSendResult(true);
}

TEST_CASE(SUITE("DirectOperateNoResponseGroup12Var1"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Direct operate (no response) group 12 Var 1, count = 1, index = 3
	auto directOperateNoACK = "C1 06 0C 01 17 01 03 01 01 01 00 00 00 01 00 00 00 00";

	/// no-ack codes ignore sequence numbers and repeat requests
	for (uint32_t i = 1; i < 5; ++i)
	{
		t.SendToOutstation(directOperateNoACK);
		REQUIRE(t.lower->PopWriteAsHex() == "");
		REQUIRE(t.cmdHandler->NumInvocations() == i);
	}
}

TEST_CASE(SUITE("DirectOperateGroup41Var1"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Direct operate group 41 Var 1, count = 1, index = 3
	t.SendToOutstation("C1 05 29 01 17 01 03 00 00 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 29 01 17 01 03 00 00 00 00 00"); // 0x00 status == CommandStatus::SUCCESS
}

TEST_CASE(SUITE("DirectOperateGroup41Var2"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Direct operate group 41 Var 1, count = 1, index = 3
	t.SendToOutstation("C1 05 29 02 17 01 03 00 00 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 29 02 17 01 03 00 00 00"); // 0x00 status == CommandStatus::SUCCESS

}

TEST_CASE(SUITE("DirectOperateGroup41Var3"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Direct operate group 41 Var 3, count = 1, index = 1
	t.SendToOutstation("C1 05 29 03 17 01 01 00 00 C7 42 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 29 03 17 01 01 00 00 C7 42 00"); // 0x00 status == CommandStatus::SUCCESS

	REQUIRE(t.cmdHandler->aoFloat32Ops.size() == 1);
	auto op = t.cmdHandler->aoFloat32Ops[0];

	REQUIRE(op.value.value == Approx(99.5).epsilon(1e-6));
	REQUIRE(op.index == 1);
	REQUIRE(op.opType == OperateType::DirectOperate);

}

TEST_CASE(SUITE("DirectOperateGroup41Var4"))
{
	OutstationConfig config;
	OutstationTestObject t(config);
	t.LowerLayerUp();

	// Direct operate group 41 Var 4, count = 1, index = 1
	t.SendToOutstation("C1 05 29 04 17 01 01 00 00 00 00 00 00 58 40 00");
	REQUIRE(t.lower->PopWriteAsHex() == "C1 81 80 00 29 04 17 01 01 00 00 00 00 00 00 58 40 00"); // 0x00 status == CommandStatus::SUCCESS


	REQUIRE(t.cmdHandler->aoDouble64Ops.size() == 1);
	auto op = t.cmdHandler->aoDouble64Ops[0];

	REQUIRE(op.value.value == Approx(96.0).epsilon(1e-6));
	REQUIRE(op.index == 1);
	REQUIRE(op.opType == OperateType::DirectOperate);
}

