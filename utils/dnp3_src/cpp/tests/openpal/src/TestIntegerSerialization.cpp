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

#include <openpal/container/Buffer.h>
#include <openpal/serialization/Parse.h>
#include <openpal/serialization/Format.h>
#include <openpal/serialization/Serialization.h>
#include <openpal/util/Comparisons.h>

#include <memory>

using namespace openpal;
using namespace testlib;

using namespace std;

template <class T>
bool TestReadWrite(T value)
{
	Buffer buffer(2 * sizeof(T));

	for (uint32_t i = 0; i < sizeof(T); ++i)
	{
		auto dest = buffer.GetWSlice();
		dest.Advance(i);
		if (!Format::Write(dest, value))
		{
			return false;
		}

		auto written = buffer.ToRSlice().Skip(i);
		T readValue;
		if (!(Parse::Read(written, readValue) && value == readValue))
		{
			return false;
		}
	}

	return true;
}


#define SUITE(name) "IntegerSerializationTestSuite - " name

TEST_CASE(SUITE("UInt8"))
{
	REQUIRE(TestReadWrite<uint8_t>(0));
	REQUIRE(TestReadWrite<uint8_t>(123));
	REQUIRE(TestReadWrite<uint8_t>(255));
}

TEST_CASE(SUITE("UInt16"))
{
	REQUIRE(TestReadWrite<uint16_t>(0));
	REQUIRE(TestReadWrite<uint16_t>(123));
	REQUIRE(TestReadWrite<uint16_t>(65535));
}

TEST_CASE(SUITE("UInt16 read from little endian"))
{
	uint8_t arr[2] = { 0x01, 0x02 };

	// 2*256 + 1
	REQUIRE(UInt16::Read(arr) == 513);
}

TEST_CASE(SUITE("Int16"))
{
	REQUIRE(TestReadWrite<int16_t>(-32768));
	REQUIRE(TestReadWrite<int16_t>(0));
	REQUIRE(TestReadWrite<int16_t>(32767));
}

TEST_CASE(SUITE("Int16 read from little endian"))
{
	uint8_t arr[2] = { 0x00, 0x80 };

	// 2*256 + 1
	REQUIRE(Int16::Read(arr) == openpal::MinValue<int16_t>());
}

TEST_CASE(SUITE("UInt32"))
{
	REQUIRE(TestReadWrite<uint32_t>(0));
	REQUIRE(TestReadWrite<uint32_t>(123));
	REQUIRE(TestReadWrite<uint32_t>(4294967295UL));
}

TEST_CASE(SUITE("UInt32 read from little endian"))
{
	uint8_t arr[4] = { 0x01, 0x02, 0x00, 0x00 };

	// 2*256 + 1
	REQUIRE(UInt32::Read(arr) == 513);
}

TEST_CASE(SUITE("Int32"))
{
	REQUIRE(TestReadWrite<int32_t>(0x80000000));
	REQUIRE(TestReadWrite<int32_t>(0));
	REQUIRE(TestReadWrite<int32_t>(0x7fffffff));
}

TEST_CASE(SUITE("Int32 read from little endian"))
{
	uint8_t arr[4] = { 0x00, 0x00, 0x00, 0x80 };

	REQUIRE(Int32::Read(arr) == openpal::MinValue<int32_t>());
}

TEST_CASE(SUITE("UInt48"))
{
	REQUIRE(TestReadWrite<UInt48Type>(UInt48Type(0)));
	REQUIRE(TestReadWrite<UInt48Type>(UInt48Type(123)));
	REQUIRE(TestReadWrite<UInt48Type>(UInt48Type(281474976710655ULL)));
}

TEST_CASE(SUITE("UInt48  read from little endian"))
{
	uint8_t arr[6] = { 0x01, 0x02, 0x00, 0x00, 0x00, 0x00};

	// 2*256 + 1
	REQUIRE(UInt48::Read(arr) == 513);
}


TEST_CASE(SUITE("ParseMany"))
{
	HexSequence hex("FF AB BA 01 00 00 00 CC");

	uint8_t first = 0;
	uint16_t second = 0;
	uint32_t third = 0;

	{
		auto input = hex.ToRSlice();
		REQUIRE(Parse::Many(input, first, second, third));
		REQUIRE(first == 255);
		REQUIRE(second == 0xBAAB);
		REQUIRE(third == 1);
		REQUIRE(input.Size() == 1); // 1 byte remaining
	}

	{
		auto input = hex.ToRSlice().Skip(2);
		REQUIRE_FALSE(Parse::Many(input, first, second, third));
	}
}


TEST_CASE(SUITE("FormatMany"))
{

	uint8_t first = 255;
	uint16_t second = 0xBAAB;
	uint32_t third = 1;

	const uint32_t SIZE = 7;

	Buffer output(SIZE + 3);

	{
		auto dest = output.GetWSlice();
		REQUIRE(Format::Many(dest, first, second, third));
		REQUIRE(dest.Size() == (output.Size() - SIZE));
		auto written = ToHex(output.ToRSlice().Take(SIZE));
		REQUIRE(written == "FF AB BA 01 00 00 00");
	}

	{
		auto dest = output.GetWSlice(SIZE - 1);
		REQUIRE_FALSE(Format::Many(dest, first, second, third));
	}
}