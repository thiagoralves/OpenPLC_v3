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

#include <limits>

using namespace std;

#undef max
#undef min

template<class T>
void TestType(T min, T max, size_t numBytes)
{
	REQUIRE((sizeof(T) == numBytes));

	T val = min;
	T limit = numeric_limits<T>::min();
	REQUIRE((val == limit));

	val = max;
	limit = numeric_limits<T>::max();
	REQUIRE((val == limit));
}

#define SUITE(name) "TestTypes - " name

TEST_CASE(SUITE(" UByte "))
{
	TestType<uint8_t>(0, 255, 1);
}

TEST_CASE(SUITE(" Int16 "))
{
	TestType<int16_t>(-32768, 32767, 2);
}

TEST_CASE(SUITE(" UInt16 "))
{
	TestType<uint16_t>(0, 65535, 2);
}

TEST_CASE(SUITE(" Int32 "))
{
	TestType<int32_t>(0x80000000, 2147483647L, 4);
}

TEST_CASE(SUITE(" UInt32 "))
{
	TestType<uint32_t>(0, 4294967295UL, 4);
}

TEST_CASE(SUITE(" Int64"))
{
	TestType<int64_t>(0x8000000000000000LL, 9223372036854775807LL, 8);
}

TEST_CASE(SUITE("Float"))
{
	REQUIRE(4 ==  sizeof(float));
}

TEST_CASE(SUITE("Double"))
{
	REQUIRE(8 ==  sizeof(double));
}


