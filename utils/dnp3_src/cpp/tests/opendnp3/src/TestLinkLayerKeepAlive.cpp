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

#include <opendnp3/link/LinkFrame.h>

#include <openpal/util/ToHex.h>
#include <openpal/container/Buffer.h>

#include "mocks/BufferSegment.h"
#include "mocks/LinkLayerTest.h"
#include "mocks/LinkHex.h"

#include <testlib/HexConversions.h>

#include <iostream>

using namespace openpal;
using namespace opendnp3;
using namespace testlib;

#define SUITE(name) "LinkLayerKeepAliveTestSuite - " name

TEST_CASE(SUITE("Timers activated and canceled in response to layer up/down"))
{
	LinkLayerTest t;
	REQUIRE(t.exe->NumPendingTimers() == 0);
	t.link.OnLowerLayerUp();
	REQUIRE(t.exe->NumPendingTimers() == 1);
	t.link.OnLowerLayerDown();
	REQUIRE(t.exe->NumPendingTimers() == 0);
}

TEST_CASE(SUITE("ForwardsKeepAliveTimeouts"))
{
	LinkConfig config(true, false);
	config.KeepAliveTimeout = TimeDuration::Seconds(5);
	LinkLayerTest t(config);

	t.link.OnLowerLayerUp();

	REQUIRE(t.exe->NumPendingTimers() == 1);
	REQUIRE(t.listener->numKeepAliveTransmissions == 0);

	REQUIRE(t.exe->AdvanceToNextTimer());
	REQUIRE(t.exe->RunMany() > 0);
	REQUIRE(t.listener->numKeepAliveTransmissions == 1);
}

TEST_CASE(SUITE("KeepAliveFailureCallbackIsInvokedOnTimeout"))
{
	LinkConfig config(true, false);
	config.KeepAliveTimeout = TimeDuration::Seconds(5);
	LinkLayerTest t(config);

	t.link.OnLowerLayerUp();

	REQUIRE(t.exe->NumPendingTimers() == 1);
	REQUIRE(t.listener->numKeepAliveTransmissions == 0);

	REQUIRE(t.exe->AdvanceToNextTimer());
	REQUIRE(t.exe->RunMany() > 0);

	REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
	REQUIRE(t.exe->NumPendingTimers() == 1);
	t.link.OnTransmitResult(true);
	REQUIRE(t.exe->NumPendingTimers() == 2);
	t.exe->AdvanceTime(config.Timeout);
	REQUIRE(t.exe->RunMany() > 0);
	REQUIRE(t.listener->numKeepAliveFailure == 1);
}

TEST_CASE(SUITE("KeepAliveSuccessCallbackIsInvokedWhenLinkStatusReceived"))
{
	LinkConfig config(true, false);
	config.KeepAliveTimeout = TimeDuration::Seconds(5);
	LinkLayerTest t(config);

	t.link.OnLowerLayerUp();

	REQUIRE(t.exe->NumPendingTimers() == 1);
	REQUIRE(t.listener->numKeepAliveTransmissions == 0);

	REQUIRE(t.exe->AdvanceToNextTimer());
	REQUIRE(t.exe->RunMany() > 0);

	REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
	REQUIRE(t.exe->NumPendingTimers() == 1);
	t.link.OnTransmitResult(true);
	REQUIRE(t.exe->NumPendingTimers() == 2);
	t.OnFrame(LinkFunction::SEC_LINK_STATUS, false, false, false, 1, 1024);
	REQUIRE(t.listener->numKeepAliveReplys == 1);
	REQUIRE(t.exe->NumPendingTimers() == 1);
}

TEST_CASE(SUITE("KeepAliveIsPeriodicOnFailure"))
{
	LinkConfig config(true, false);
	config.KeepAliveTimeout = TimeDuration::Seconds(5);
	LinkLayerTest t(config);

	t.link.OnLowerLayerUp();

	for (int count = 0; count < 3; ++count)
	{
		REQUIRE(t.exe->NumPendingTimers() == 1);
		REQUIRE(t.listener->numKeepAliveTransmissions == count);

		REQUIRE(t.exe->AdvanceToNextTimer());
		REQUIRE(t.exe->RunMany() > 0);

		REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
		REQUIRE(t.exe->NumPendingTimers() == 1);
		t.link.OnTransmitResult(true);
		REQUIRE(t.exe->NumPendingTimers() == 2);

		t.exe->AdvanceTime(config.Timeout);
		REQUIRE(t.exe->RunMany() > 0);
		REQUIRE(t.listener->numKeepAliveFailure == (count + 1));
	}
}

TEST_CASE(SUITE("KeepAliveIsPeriodicOnSuccess"))
{
	LinkConfig config(true, false);
	config.KeepAliveTimeout = TimeDuration::Seconds(5);
	LinkLayerTest t(config);

	t.link.OnLowerLayerUp();

	for (int count = 0; count < 3; ++count)
	{
		REQUIRE(t.exe->NumPendingTimers() == 1);
		REQUIRE(t.listener->numKeepAliveTransmissions == count);

		REQUIRE(t.exe->AdvanceToNextTimer());
		REQUIRE(t.exe->RunMany() > 0);

		REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
		REQUIRE(t.exe->NumPendingTimers() == 1);
		t.link.OnTransmitResult(true);
		REQUIRE(t.exe->NumPendingTimers() == 2);
		t.OnFrame(LinkFunction::SEC_LINK_STATUS, false, false, false, 1, 1024);
		REQUIRE(t.listener->numKeepAliveReplys == (count + 1));
	}
}



