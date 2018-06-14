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

#include <opendnp3/app/WriteConversions.h>
#include <opendnp3/app/QualityMasks.h>

using namespace opendnp3;

#define SUITE(name) "WriteConversionTestSuite - " name

TEST_CASE(SUITE("Group30Var2ConvertsWithinRange"))
{
	Analog a(12);
	auto gv = ConvertGroup30Var2::Apply(a);

	REQUIRE(gv.value ==  12);
	REQUIRE(gv.flags ==  ToUnderlying(AnalogQuality::ONLINE));
}

TEST_CASE(SUITE("Group30Var2ConvertsOverrange"))
{
	Analog a(32768);
	auto gv = ConvertGroup30Var2::Apply(a);

	REQUIRE(gv.value ==  32767);
	REQUIRE(gv.flags == (ToUnderlying(AnalogQuality::ONLINE) | ToUnderlying(AnalogQuality::OVERRANGE)));
}

TEST_CASE(SUITE("Group30Var2ConvertsUnderrange"))
{
	Analog a(-32769);
	auto gv = ConvertGroup30Var2::Apply(a);

	REQUIRE(gv.value ==  -32768);
	REQUIRE(gv.flags == (ToUnderlying(AnalogQuality::ONLINE) | ToUnderlying(AnalogQuality::OVERRANGE)));
}



