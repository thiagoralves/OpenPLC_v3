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

#include <testlib/BufferHelpers.h>
#include <testlib/HexConversions.h>

#include <opendnp3/objects/Group120.h>

#include <openpal/util/ToHex.h>
#include <openpal/container/Buffer.h>

using namespace openpal;
using namespace opendnp3;
using namespace testlib;

#define SUITE(name) "Group120Var2TestSuite - " name

TEST_CASE(SUITE("Parser rejects empty buffer"))
{
	HexSequence buffer("");

	Group120Var2 output;
	REQUIRE_FALSE(output.Read(buffer.ToRSlice()));
}

TEST_CASE(SUITE("Parser identifies data field"))
{
	HexSequence buffer("04 00 00 00 09 01 AB BA");

	Group120Var2 output;
	REQUIRE(output.Read(buffer.ToRSlice()));
	REQUIRE(output.challengeSeqNum == 4);
	REQUIRE(output.userNum == 265);
	REQUIRE(ToHex(output.hmacValue) == "AB BA");
}

TEST_CASE(SUITE("Parser allows empty data field"))
{
	HexSequence buffer("04 00 00 00 09 01");

	Group120Var2 output;
	REQUIRE(output.Read(buffer.ToRSlice()));
	REQUIRE(output.challengeSeqNum == 4);
	REQUIRE(output.userNum == 265);
	REQUIRE(output.hmacValue.IsEmpty());
}

TEST_CASE(SUITE("Parser rejects one less than min length"))
{
	HexSequence buffer("04 00 00 00 09");
	Group120Var2 output;
	REQUIRE_FALSE(output.Read(buffer.ToRSlice()));
}