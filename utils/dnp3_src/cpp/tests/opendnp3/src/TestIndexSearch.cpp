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

#include <openpal/container/Array.h>

#include <opendnp3/app/MeasurementTypeSpecs.h>
#include <opendnp3/outstation/IndexSearch.h>

using namespace openpal;
using namespace opendnp3;

#define SUITE(name) "IndexSearch - " name

IndexSearch::Result TestResultLengthFour(uint16_t index)
{
	Array<Cell<BinarySpec>, uint16_t> values(4);
	values[0].config.vIndex = 1;
	values[1].config.vIndex = 3;
	values[2].config.vIndex = 7;
	values[3].config.vIndex = 9;

	return IndexSearch::FindClosestRawIndex(values.ToView(), index);
}

TEST_CASE(SUITE("FindsExactMatchForLastValue"))
{
	auto result = TestResultLengthFour(9);
	REQUIRE(result.match);
	REQUIRE(result.index == 3);
}

TEST_CASE(SUITE("FindsExactMatchForFirstValue"))
{
	auto result = TestResultLengthFour(1);
	REQUIRE(result.match);
	REQUIRE(result.index == 0);
}

TEST_CASE(SUITE("StopsOnFirstValueIfIndexLessThanFirst"))
{
	auto result = TestResultLengthFour(0);
	REQUIRE(!result.match);
	REQUIRE(result.index == 0);
}

TEST_CASE(SUITE("StopsOnLastValueIfIndexGreaterThanLast"))
{
	auto result = TestResultLengthFour(11);
	REQUIRE(!result.match);
	REQUIRE(result.index == 3);
}

TEST_CASE(SUITE("FindsNextSmallestIndex"))
{
	auto result = TestResultLengthFour(2);
	REQUIRE(!result.match);
	REQUIRE(result.index == 0);
}

TEST_CASE(SUITE("FindsNextBiggestIndex"))
{
	auto result = TestResultLengthFour(8);
	REQUIRE(!result.match);
	REQUIRE(result.index == 3);
}

Range TestRangeSearch(const Range& range)
{
	Array<Cell<BinarySpec>, uint16_t> values(4);
	values[0].config.vIndex = 1;
	values[1].config.vIndex = 3;
	values[2].config.vIndex = 7;
	values[3].config.vIndex = 9;

	return IndexSearch::FindRawRange(values.ToView(), range);
}

TEST_CASE(SUITE("FindsFullRangeIfInputBracketsEntireRange"))
{
	auto range = TestRangeSearch(Range::From(0, 20));
	REQUIRE(range.start == 0);
	REQUIRE(range.stop == 3);
}

TEST_CASE(SUITE("FindsInvalidRangeIfBeneathRange"))
{
	auto range = TestRangeSearch(Range::From(0, 0));
	REQUIRE(!range.IsValid());
}

TEST_CASE(SUITE("FindsInvalidRangeIfAboveRange"))
{
	auto range = TestRangeSearch(Range::From(10, 10));
	REQUIRE(!range.IsValid());
}

TEST_CASE(SUITE("FindsInvalidRangeIfNoMatchInside"))
{
	auto range = TestRangeSearch(Range::From(4, 6));
	REQUIRE(!range.IsValid());
}

TEST_CASE(SUITE("FindsSingleValueExactMatch"))
{
	auto range = TestRangeSearch(Range::From(3, 3));
	REQUIRE(range.start == 1);
	REQUIRE(range.stop == 1);
}

TEST_CASE(SUITE("FindsSingleValuePartialMatch"))
{
	auto range = TestRangeSearch(Range::From(2, 6));
	REQUIRE(range.start == 1);
	REQUIRE(range.stop == 1);
}

TEST_CASE(SUITE("FindsSubRangeAtFront"))
{
	auto range = TestRangeSearch(Range::From(0, 8));
	REQUIRE(range.start == 0);
	REQUIRE(range.stop == 2);
}

TEST_CASE(SUITE("FindsSubRangeInMiddle"))
{
	auto range = TestRangeSearch(Range::From(3, 7));
	REQUIRE(range.start == 1);
	REQUIRE(range.stop == 2);
}

TEST_CASE(SUITE("FindsSubRangeAtEnd"))
{
	auto range = TestRangeSearch(Range::From(5, 11));
	REQUIRE(range.start == 2);
	REQUIRE(range.stop == 3);
}

