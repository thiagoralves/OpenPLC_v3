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

#include <opendnp3/link/CRC.h>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;
using namespace opendnp3;
using namespace testlib;

#define SUITE(name) "CRC - " name

TEST_CASE(SUITE("CrcTest"))
{
	HexSequence hs("05 64 05 C0 01 00 00 04 E9 21");
	REQUIRE(hs.Size() ==  10);
	REQUIRE(CRC::CalcCrc(hs, 8) == 0x21E9);
}


