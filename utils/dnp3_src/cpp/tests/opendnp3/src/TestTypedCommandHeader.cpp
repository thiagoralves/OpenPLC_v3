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

#include <opendnp3/master/TypedCommandHeader.h>
#include <opendnp3/master/CommandSetOps.h>
#include <opendnp3/objects/Group12.h>
#include <opendnp3/objects/Group41.h>

#include <opendnp3/app/APDURequest.h>

#include <openpal/container/StaticBuffer.h>

#include <testlib/HexConversions.h>

using namespace opendnp3;
using namespace openpal;
using namespace testlib;

std::string WriteToHex(const CommandSet& set);

#define SUITE(name) "TypedCommandHeaderTestSuite - " name

TEST_CASE(SUITE("Can instantiate a CROB header"))
{
	TypedCommandHeader<ControlRelayOutputBlock> header(Group12Var1::Inst());
}

TEST_CASE(SUITE("Formats properly if enough space is available"))
{
	TypedCommandHeader<AnalogOutputInt16> header(Group41Var2::Inst());
	header.Add(AnalogOutputInt16(7), 10);
	header.Add(AnalogOutputInt16(8), 11);

	StaticBuffer<100> buffer;
	auto dest = buffer.GetWSlice();
	APDURequest request(dest);
	auto writer = request.GetWriter();

	REQUIRE(header.Write(writer));

	auto hex = ToHex(request.ToRSlice().Skip(2));

	REQUIRE(hex == "29 02 28 02 00 0A 00 07 00 00 0B 00 08 00 00");
}

TEST_CASE(SUITE("Does not format if insufficient space"))
{
	TypedCommandHeader<AnalogOutputInt16> header(Group41Var2::Inst());
	header.Add(AnalogOutputInt16(7), 10);
	header.Add(AnalogOutputInt16(8), 11);

	StaticBuffer<14> buffer;
	auto dest = buffer.GetWSlice();
	APDURequest request(dest);
	auto writer = request.GetWriter();

	REQUIRE_FALSE(header.Write(writer));
}

TEST_CASE(SUITE("Command set can be moved and written"))
{
	CommandSet commands;

	commands.Add<AnalogOutputInt16>({ WithIndex(AnalogOutputInt16(7), 10) });
	commands.Add<AnalogOutputInt32>({ WithIndex(AnalogOutputInt32(8), 11) });

	CommandSet commands2(std::move(commands));

	REQUIRE(WriteToHex(commands) == "");
	REQUIRE(WriteToHex(commands2) == "29 02 28 01 00 0A 00 07 00 00 29 01 28 01 00 0B 00 08 00 00 00 00");
}

std::string WriteToHex(const CommandSet& commands)
{
	StaticBuffer<100> buffer;
	auto dest = buffer.GetWSlice();
	APDURequest request(dest);
	auto writer = request.GetWriter();

	REQUIRE(CommandSetOps::Write(commands, writer));
	return ToHex(request.ToRSlice().Skip(2));
}