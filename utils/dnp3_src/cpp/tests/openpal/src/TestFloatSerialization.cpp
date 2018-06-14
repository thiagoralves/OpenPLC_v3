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
#include <openpal/serialization/FloatByteOrder.h>
#include <openpal/util/Comparisons.h>

#include <memory>

using namespace openpal;
using namespace testlib;

using namespace std;

template <class T>
bool TestReadWriteFloat(T value)
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
		if (!(Parse::Read(written, readValue) && FloatEqual(value, readValue)))
		{
			return false;
		}
	}

	return true;
}

template <class T>
bool TestFloatParsing(std::string hex, typename T::Type value)
{
	HexSequence hs(hex);
	const uint32_t TYPE_SIZE = static_cast<uint32_t>(sizeof(typename T::Type));
	REQUIRE((hs.Size() == TYPE_SIZE));

	Buffer buffer(2 * TYPE_SIZE);

	for (uint32_t i = 0; i < TYPE_SIZE; ++i)
	{
		auto dest = buffer.GetWSlice();
		dest.Advance(i);
		if (!Format::Write(dest, value))
		{
			return false;
		}
		auto written = buffer.ToRSlice().Skip(i);

		typename T::Type val = 0;
		if (!(Parse::Read(written, val) && openpal::FloatEqual(val, value)))
		{
			return false;
		}
	}

	return true;
}

#define SUITE(name) "FloatSerializationTestSuite - " name

TEST_CASE(SUITE("Float memory byte order is IEEE 754"))
{
	REQUIRE(openpal::FloatByteOrder::ORDER != FloatByteOrder::Value::UNSUPPORTED);
}

TEST_CASE(SUITE("DoublePacking"))
{
	REQUIRE(TestFloatParsing<openpal::DoubleFloat>("20 74 85 2F C7 2B A2 C0", -2.3258890344E3));
	REQUIRE(TestFloatParsing<openpal::DoubleFloat>("00 00 00 00 64 89 67 41", 12340000.0));
	REQUIRE(TestFloatParsing<openpal::DoubleFloat>("00 00 00 00 00 00 34 C0", -20.0));
	REQUIRE(TestFloatParsing<openpal::DoubleFloat>("8F 81 9C 95 2D F9 64 BB", -13.879E-23));
	REQUIRE(TestFloatParsing<openpal::DoubleFloat>("00 00 00 00 00 00 59 40", 100.0));
}

TEST_CASE(SUITE("SinglePacking"))
{
	REQUIRE(TestFloatParsing<openpal::SingleFloat>("20 4B 3C 4B", 12340000.0f));
	REQUIRE(TestFloatParsing<openpal::SingleFloat>("6D C9 27 9B", -13.879E-23f));
	REQUIRE(TestFloatParsing<openpal::SingleFloat>("00 00 A0 C1", -20.0));
}


TEST_CASE(SUITE("DoubleFloat"))
{
	REQUIRE(TestReadWriteFloat<double>(0.0));
	REQUIRE(TestReadWriteFloat<double>(-100000));
	REQUIRE(TestReadWriteFloat<double>(-2.3258890344E3));
	REQUIRE(TestReadWriteFloat<double>(1E20));
	REQUIRE(TestReadWriteFloat<double>(100.0));
}

TEST_CASE(SUITE("SingleFloat"))
{
	REQUIRE(TestReadWriteFloat<float>(0.0f));
	REQUIRE(TestReadWriteFloat<float>(-100000.0f));
	REQUIRE(TestReadWriteFloat<float>(-2.3258890344E3f));
	REQUIRE(TestReadWriteFloat<float>(1E20f));
	REQUIRE(TestReadWriteFloat<float>(100.0f));
}

