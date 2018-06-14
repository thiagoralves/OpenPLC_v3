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

using namespace std;
using namespace testlib;


#define SUITE(name) "UtilSuite - " name
template <int N>
void TestHex(const std::string& hex, uint8_t* compareBytes, size_t count)
{
	HexSequence hs(hex);

	REQUIRE(hs.Size() <= N);

	REQUIRE((hs.Size() ==  count));
	for ( size_t i = 0; i < count; i++ )
	{
		REQUIRE((hs[i] ==  compareBytes[i]));
	}
}

TEST_CASE(SUITE("HexToBytes2TestSmall"))
{
	uint8_t values[] = { 0xAF, 0x23 };
	TestHex<2>( "AF23", values, 2 );
}
TEST_CASE(SUITE("HexToBytes2Test64"))
{
	uint8_t values[] = { 0x13, 0xA2, 0x00, 0x40, 0x56, 0x1D, 0x08 };
	TestHex<7>( "13A20040561D08", values, 7 );
}

TEST_CASE(SUITE("HexToBytes2Test64TooBig"))
{
	uint8_t values[] = { 0x13, 0xA2, 0x00, 0x40, 0x56, 0x1D, 0x08 };
	TestHex<8>( "13A20040561D08", values, 7 );
}

TEST_CASE(SUITE("HexToBytes2Test64Hole"))
{
	uint8_t values[] = { 0x13, 0xA2, 0x00, 0x40, 0x56, 0x1D, 0x08 };
	TestHex<8>( "13A200 40561   D08", values, 7 );
}


