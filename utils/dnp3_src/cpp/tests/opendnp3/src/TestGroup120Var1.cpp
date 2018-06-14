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

#define SUITE(name) "Group120Var1TestSuite - " name

TEST_CASE(SUITE("Parser rejects empty buffer"))
{
	HexSequence buffer("");

	Group120Var1 output;
	REQUIRE_FALSE(output.Read(buffer.ToRSlice()));
}

TEST_CASE(SUITE("Parser correctly interprets challenge data"))
{
	// SEQ = 1, USER = 7, HMAC = 5 (SHA-1-8), REASON = 1, challenge = 0xDEADBEEF
	HexSequence buffer("01 00 00 00 07 00 05 01 DE AD BE EF");

	Group120Var1 output;
	REQUIRE(output.Read(buffer.ToRSlice()));
	REQUIRE(output.challengeSeqNum == 1);
	REQUIRE(output.userNum == 7);
	REQUIRE(output.hmacAlgo == HMACType::HMAC_SHA1_TRUNC_8);
	REQUIRE(output.challengeReason == ChallengeReason::CRITICAL);
	REQUIRE(ToHex(output.challengeData) == "DE AD BE EF");
}

TEST_CASE(SUITE("Parser accepts empty challenge data"))
{
	// SEQ = 1, USER = 7, HMAC = 5 (SHA-1-8), REASON = 1, challenge length of 3
	HexSequence buffer("01 00 00 00 07 00 05 01");

	Group120Var1 output;
	REQUIRE(output.Read(buffer.ToRSlice()));
	REQUIRE(output.challengeData.Size() == 0);
}

TEST_CASE(SUITE("Parser rejects one less than minimum required data"))
{
	// SEQ = 1, USER = 7, HMAC = 5 (SHA-1-8), REASON = ???? missing
	HexSequence buffer("01 00 00 00 07 00 05");

	Group120Var1 output;
	REQUIRE_FALSE(output.Read(buffer.ToRSlice()));
}

TEST_CASE(SUITE("Formatter correctly writes when sufficient space"))
{
	HexSequence challengeData("DE AD BE EF AB BA"); // 6 bytes

	Group120Var1 challenge(9, 3, HMACType::HMAC_SHA256_TRUNC_16, ChallengeReason::CRITICAL, challengeData.ToRSlice());
	const uint32_t SIZE = challenge.Size();

	Buffer output(64);
	auto dest = output.GetWSlice();
	REQUIRE(challenge.Write(dest));
	auto written = output.Size() - dest.Size();

	REQUIRE(written == SIZE);
	REQUIRE(ToHex(output.ToRSlice().Take(SIZE)) == "09 00 00 00 03 00 04 01 DE AD BE EF AB BA");
}

TEST_CASE(SUITE("Formatter return false when insufficient space"))
{
	HexSequence challengeData("DE AD BE EF AB BA"); // 6 bytes

	Group120Var1 challenge(9, 3, HMACType::HMAC_SHA256_TRUNC_16, ChallengeReason::CRITICAL, challengeData.ToRSlice());
	const uint32_t SIZE = challenge.Size();

	Buffer output(SIZE - 1);
	auto dest = output.GetWSlice();
	REQUIRE_FALSE(challenge.Write(dest));
}