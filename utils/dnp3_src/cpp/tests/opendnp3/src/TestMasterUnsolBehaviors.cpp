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

#include <dnp3mocks/CallbackQueue.h>
#include <dnp3mocks/APDUHexBuilders.h>

#include <opendnp3/app/APDUResponse.h>
#include <opendnp3/app/APDUBuilders.h>

using namespace openpal;
using namespace opendnp3;


#define SUITE(name) "MasterUnsolTestSuite - " name

TEST_CASE(SUITE("ReceiveUnsolBeforeTransmit"))
{
	MasterParams params;
	params.disableUnsolOnStartup = false;
	MasterTestObject t(params);
	t.context->OnLowerLayerUp();

	t.SendToMaster(hex::NullUnsolicited(0, IINField::Empty()));

	t.exe->RunMany();

	REQUIRE(t.lower->PopWriteAsHex() == hex::UnsolConfirm(0));
	t.context->OnSendResult(true);

	t.exe->RunMany();

	REQUIRE(t.lower->PopWriteAsHex() == hex::IntegrityPoll(0));
}


