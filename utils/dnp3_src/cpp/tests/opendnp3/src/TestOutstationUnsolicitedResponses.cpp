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

using namespace std;
using namespace opendnp3;
using namespace openpal;

#define SUITE(name) "OutstationUnsolicitedTestSuite - " name

TEST_CASE(SUITE("sends null unsol on startup"))
{
	OutstationConfig cfg;  cfg.params.allowUnsolicited = true;
	OutstationTestObject t(cfg);
	t.LowerLayerUp();

	REQUIRE(t.lower->PopWriteAsHex() == hex::NullUnsolicited(0, IINField(IINBit::DEVICE_RESTART)));
	REQUIRE(t.NumPendingTimers() == 1);
}

TEST_CASE(SUITE("Non-read during null unsol"))
{
	OutstationConfig cfg;  cfg.params.allowUnsolicited = true;
	OutstationTestObject t(cfg);
	t.LowerLayerUp();

	REQUIRE(t.lower->PopWriteAsHex() == hex::NullUnsolicited(0, IINField(IINBit::DEVICE_RESTART)));
	t.OnSendResult(true);
	REQUIRE(t.lower->NumWrites() == 0);

	// send any non-read message
	t.SendToOutstation(hex::ClearRestartIIN(0));
	REQUIRE(t.lower->PopWriteAsHex() == hex::EmptyResponse(0));
	REQUIRE(t.NumPendingTimers() == 1);
}

TEST_CASE(SUITE("UnsolRetryDelay"))
{
	OutstationConfig cfg; cfg.params.allowUnsolicited = true;
	OutstationTestObject t(cfg);

	t.LowerLayerUp();


	// check for the startup null unsol packet, but fail the transaction
	REQUIRE(t.lower->PopWriteAsHex() == hex::NullUnsolicited(0, IINField(IINBit::DEVICE_RESTART)));
	t.OnSendResult(true);

	REQUIRE(t.NumPendingTimers() ==  1); // confirm timer
	REQUIRE(t.AdvanceToNextTimer());

	// immediately retries with new sequence #
	REQUIRE(t.lower->PopWriteAsHex() == hex::NullUnsolicited(1, IINField(IINBit::DEVICE_RESTART)));
}

TEST_CASE(SUITE("UnsolData"))
{
	OutstationConfig cfg; cfg.params.allowUnsolicited = true;
	cfg.params.unsolClassMask = ClassField::AllEventClasses(); // allows us to skip the "enable unsol" step
	cfg.eventBufferConfig = EventBufferConfig::AllTypes(5);
	OutstationTestObject t(cfg, DatabaseSizes::BinaryOnly(3));

	auto view = t.context.GetConfigView();
	view.binaries[0].config.clazz = PointClass::Class1;
	view.binaries[1].config.clazz = PointClass::Class2;
	view.binaries[2].config.clazz = PointClass::Class3;

	t.LowerLayerUp();

	REQUIRE(t.lower->PopWriteAsHex() ==  hex::NullUnsolicited(0, IINField(IINBit::DEVICE_RESTART)));
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(0));

	// do a transaction before the layer comes online to prove that the null transaction
	// is occuring before unsol data is sent
	t.Transaction([](IUpdateHandler & db)
	{
		db.Update(Binary(false, 0x01), 2);
	});


	// should immediately try to send another unsol packet,
	// Grp2Var1, qual 0x17, count 1, index 2, quality+val == 0x01
	REQUIRE(t.lower->PopWriteAsHex() ==  "F1 82 80 00 02 01 28 01 00 02 00 01");
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(1));
	REQUIRE(t.lower->PopWriteAsHex() == "");
}

TEST_CASE(SUITE("UnsolEventBufferOverflow"))
{
	OutstationConfig cfg;
	cfg.params.allowUnsolicited = true;
	cfg.params.unsolClassMask = ClassField(PointClass::Class1);
	cfg.eventBufferConfig = EventBufferConfig(2);
	OutstationTestObject t(cfg, DatabaseSizes::BinaryOnly(1));

	t.LowerLayerUp();
	REQUIRE(t.lower->PopWriteAsHex() == hex::NullUnsolicited(0, IINField(IINBit::DEVICE_RESTART)));
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(0));

	t.Transaction([](IUpdateHandler & db)
	{
		db.Update(Binary(true, 0x01), 0);
		db.Update(Binary(false, 0x01), 0);
		db.Update(Binary(true, 0x01), 0);
	});

	// should immediately try to send 2 unsol events
	// Grp2Var1, qual 0x17, count 2, index 0
	// The last two values should be published, 0x01 and 0x81 (false and true)
	// the first value is lost off the front of the buffer
	REQUIRE(t.lower->PopWriteAsHex() ==  "F1 82 80 08 02 01 28 02 00 00 00 01 00 00 81");
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(1));

	REQUIRE(t.lower->PopWriteAsHex() == "");
}

TEST_CASE(SUITE("UnsolMultiFragments"))
{
	OutstationConfig cfg;
	cfg.params.allowUnsolicited = true;
	cfg.params.maxTxFragSize = 20; //this will cause the unsol response to get fragmented
	cfg.params.unsolClassMask = ClassField::AllEventClasses(); // this allows the EnableUnsol sequence to be skipped
	cfg.eventBufferConfig = EventBufferConfig(0, 0, 5);
	OutstationTestObject t(cfg, DatabaseSizes::AnalogOnly(5));

	t.LowerLayerUp();

	REQUIRE(t.lower->PopWriteAsHex() ==  hex::NullUnsolicited(0));
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(0));
	REQUIRE(t.lower->PopWriteAsHex() ==  "");

	t.Transaction([](IUpdateHandler & db)
	{
		db.Update(Analog(7, 0x01), 1);
		db.Update(Analog(13, 0x01), 3);
	});


	// Only enough room to in the APDU to carry a single value
	REQUIRE(t.lower->PopWriteAsHex() ==  "F1 82 82 00 20 01 28 01 00 01 00 01 07 00 00 00");
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(1));

	// should immediately try to send another unsol packet
	REQUIRE(t.lower->PopWriteAsHex() ==  "F2 82 80 00 20 01 28 01 00 03 00 01 0D 00 00 00");
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(2));

	REQUIRE(t.lower->PopWriteAsHex() == "");
}

void WriteDuringUnsol(bool beforeTx)
{
	OutstationConfig cfg;
	cfg.params.allowUnsolicited = true;
	cfg.params.unsolClassMask = ClassField::AllEventClasses();
	cfg.eventBufferConfig = EventBufferConfig(5);
	OutstationTestObject t(cfg, DatabaseSizes::BinaryOnly(5));

	t.LowerLayerUp();

	REQUIRE(t.lower->PopWriteAsHex() == hex::NullUnsolicited(0));
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(0));

	t.Transaction([](IUpdateHandler & db)
	{
		db.Update(Binary(true, 0x01), 0);
	});

	REQUIRE(t.lower->PopWriteAsHex() == "F1 82 80 00 02 01 28 01 00 00 00 81");

	if (beforeTx)
	{
		t.SendToOutstation(hex::ClearRestartIIN(0));
		t.OnSendResult(true);
	}
	else
	{
		t.OnSendResult(true);
		t.SendToOutstation(hex::ClearRestartIIN(0));
	}

	// check that we get a response to this immediately without the confirm
	REQUIRE(t.lower->PopWriteAsHex() == hex::EmptyResponse(0));

	// now send the confirm to the outstation
	t.SendToOutstation(hex::UnsolConfirm(1));
	REQUIRE(t.lower->PopWriteAsHex() == "");
}

// Test that non-read fragments are immediately responded to while
// waiting for a response to unsolicited data
TEST_CASE(SUITE("WriteDuringUnsolBeforeTx"))
{
	WriteDuringUnsol(true);
}

TEST_CASE(SUITE("WriteDuringUnsolAfterTx"))
{
	WriteDuringUnsol(false);
}


TEST_CASE(SUITE("ReadDuringUnsol"))
{
	OutstationConfig cfg;
	cfg.params.allowUnsolicited = true;
	cfg.params.unsolClassMask = ClassField::AllEventClasses();
	cfg.eventBufferConfig = EventBufferConfig(5);
	OutstationTestObject t(cfg, DatabaseSizes::BinaryOnly(5));

	t.LowerLayerUp();

	REQUIRE(t.lower->PopWriteAsHex() == hex::NullUnsolicited(0));
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(0));

	t.Transaction([](IUpdateHandler & db)
	{
		db.Update(Binary(true, 0x01), 0);
	});

	REQUIRE(t.lower->PopWriteAsHex() ==  "F1 82 80 00 02 01 28 01 00 00 00 81");

	auto readClass1 = "C0 01 3C 02 06";

	if (true)
	{
		t.OnSendResult(true);
		t.SendToOutstation(readClass1);
	}
	else
	{
		t.SendToOutstation(readClass1);
		t.OnSendResult(true);
	}

	t.SendToOutstation(hex::UnsolConfirm(1));

	REQUIRE(t.lower->PopWriteAsHex() ==  "C0 81 80 00");
}


TEST_CASE(SUITE("ReadWriteDuringUnsol"))
{
	OutstationConfig cfg;
	cfg.params.allowUnsolicited = true;
	cfg.params.unsolClassMask = ClassField::AllEventClasses();
	cfg.eventBufferConfig = EventBufferConfig(5);
	OutstationTestObject t(cfg, DatabaseSizes::BinaryOnly(5));

	t.LowerLayerUp();

	REQUIRE(t.lower->PopWriteAsHex() ==  "F0 82 80 00");
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(0));

	t.Transaction([](IUpdateHandler & db)
	{
		db.Update(Binary(true, 0x01), 0);
	});

	REQUIRE(t.lower->PopWriteAsHex() ==  "F1 82 80 00 02 01 28 01 00 00 00 81");
	t.OnSendResult(true);

	// send a read request that will be overwritten
	t.SendToOutstation("C0 01 3C 01 06");

	//now send a write IIN request, and test that the outstation answers immediately
	t.SendToOutstation("C0 02 50 01 00 07 07 00");

	REQUIRE(t.lower->PopWriteAsHex() ==  "C0 81 00 00");
}

TEST_CASE(SUITE("RepeatRequestDuringUnsol"))
{
	OutstationConfig cfg;
	cfg.params.allowUnsolicited = true;
	cfg.params.unsolClassMask = ClassField::AllEventClasses();
	cfg.eventBufferConfig = EventBufferConfig(5);
	OutstationTestObject t(cfg, DatabaseSizes::BinaryOnly(5));

	t.LowerLayerUp();

	REQUIRE(t.lower->PopWriteAsHex() == "F0 82 80 00");
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(0));

	t.SendToOutstation(hex::ClearRestartIIN(0));
	REQUIRE(t.lower->PopWriteAsHex() == hex::EmptyResponse(0));
	t.OnSendResult(true);

	t.Transaction([](IUpdateHandler & db)
	{
		db.Update(Binary(true, 0x01), 1);
	});
	REQUIRE(t.lower->PopWriteAsHex() == "F1 82 00 00 02 01 28 01 00 01 00 81");
	t.OnSendResult(true);

	// while waiting for a confirm, repeat the previous request, byte-for-byte
	t.SendToOutstation(hex::ClearRestartIIN(0));
	REQUIRE(t.lower->PopWriteAsHex() == hex::EmptyResponse(0));
	t.OnSendResult(true);

}


TEST_CASE(SUITE("UnsolEnable"))
{
	OutstationConfig cfg;
	cfg.params.allowUnsolicited = true;
	cfg.eventBufferConfig = EventBufferConfig(5);
	OutstationTestObject t(cfg, DatabaseSizes::BinaryOnly(5));

	t.LowerLayerUp();

	REQUIRE(t.lower->PopWriteAsHex() ==  hex::NullUnsolicited(0));
	t.OnSendResult(true);
	t.SendToOutstation(hex::UnsolConfirm(0));

	// do a transaction to show that unsol data is not being reported yet
	t.Transaction([](IUpdateHandler & db)
	{
		db.Update(Binary(false, 0x01), 0);
	});

	REQUIRE(t.lower->PopWriteAsHex() == "");

	// Enable unsol class 1
	t.SendToOutstation("C0 14 3C 02 06");
	REQUIRE(t.lower->PopWriteAsHex() ==  "C0 81 82 00");
	t.OnSendResult(true);

	// should automatically send the previous data as unsol
	REQUIRE(t.lower->PopWriteAsHex() ==  "F1 82 80 00 02 01 28 01 00 00 00 01");
}


TEST_CASE(SUITE("UnsolEnableDisableFailure"))
{
	OutstationConfig cfg;
	cfg.eventBufferConfig = EventBufferConfig(5);
	OutstationTestObject t(cfg, DatabaseSizes::BinaryOnly(5));

	t.LowerLayerUp();
	t.SendToOutstation("C0 14 3C 02 06");
	REQUIRE(t.lower->PopWriteAsHex() ==  "C0 81 80 01"); //FUNC_NOT_SUPPORTED
}



