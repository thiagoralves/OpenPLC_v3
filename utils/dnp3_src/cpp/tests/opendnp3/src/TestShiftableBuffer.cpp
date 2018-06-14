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

#include <opendnp3/link/ShiftableBuffer.h>

#include <openpal/container/Buffer.h>

#include <cstring>

using namespace opendnp3;
using namespace openpal;


#define SUITE(name) "ShiftableBufferSuite - " name

const static uint8_t SYNC[] = {0x05, 0x64};

TEST_CASE(SUITE("ConstructDestruct"))
{
	Buffer buffer(100);
	ShiftableBuffer b(buffer(), buffer.Size());
}

TEST_CASE(SUITE("InitialState"))
{
	Buffer buffer(100);
	ShiftableBuffer b(buffer(), buffer.Size());

	REQUIRE(b.NumBytesRead() == 0);
	REQUIRE(b.NumWriteBytes() ==  100);
	REQUIRE(b.ReadBuffer() ==  b.WriteBuff());
}

TEST_CASE(SUITE("ReadingWriting"))
{
	Buffer buffer(100);
	ShiftableBuffer b(buffer(), buffer.Size());

	b.AdvanceWrite(40);
	REQUIRE(b.NumWriteBytes() ==  60);
	REQUIRE(b.NumBytesRead() == 40);

	b.AdvanceWrite(60);
	REQUIRE(b.NumWriteBytes() ==  0);
	REQUIRE(b.NumBytesRead() == 100);

	b.AdvanceRead(30);
	REQUIRE(b.NumWriteBytes() ==  0);
	REQUIRE(b.NumBytesRead() == 70);

	b.AdvanceRead(70);
	REQUIRE(b.NumWriteBytes() ==  0);
	REQUIRE(b.NumBytesRead() == 0);
}

TEST_CASE(SUITE("Shifting"))
{
	Buffer buffer(100);
	ShiftableBuffer b(buffer(), buffer.Size());

	//initialize buffer to all zeros
	for(size_t i = 0; i < b.NumWriteBytes(); ++i) b.WriteBuff()[i] = 0;
	b.WriteBuff()[97] = 1;
	b.WriteBuff()[98] = 2;
	b.WriteBuff()[99] = 3;

	b.AdvanceWrite(100);

	b.AdvanceRead(97);
	b.Shift();
}

TEST_CASE(SUITE("SyncNoPattern"))
{
	Buffer buffer(100);
	ShiftableBuffer b(buffer(), buffer.Size());

	for (size_t i = 0; i < b.NumWriteBytes(); ++i)
	{
		b.WriteBuff()[i] = 0;
	}

	b.AdvanceWrite(100);

	uint32_t skipBytes = 0;
	REQUIRE_FALSE(b.Sync(skipBytes));
	REQUIRE(b.NumBytesRead() == 1); // 1 byte left since need 2 bytes to sync
	REQUIRE(b.NumWriteBytes() ==  0);
}

TEST_CASE(SUITE("SyncBeginning"))
{
	Buffer buffer(100);
	ShiftableBuffer b(buffer(), buffer.Size());

	for(size_t i = 0; i < b.NumWriteBytes(); ++i) b.WriteBuff()[i] = 0;

	memcpy(b.WriteBuff(), SYNC, 2);
	b.AdvanceWrite(100);

	uint32_t skipBytes = 0;
	REQUIRE(b.Sync(skipBytes));
	REQUIRE(b.NumBytesRead() == 100);
	REQUIRE(b.NumWriteBytes() ==  0);

}

TEST_CASE(SUITE("SyncFullPattern"))
{
	Buffer buffer(100);
	ShiftableBuffer b(buffer(), buffer.Size());

	//initialize buffer to all zeros
	for(size_t i = 0; i < b.NumWriteBytes(); ++i) b.WriteBuff()[i] = 0;
	uint8_t pattern[] = {0x05, 0x64};
	memcpy(b.WriteBuff() + 50, pattern, 2); //copy the pattern into the buffer
	b.AdvanceWrite(100);

	uint32_t skipBytes = 0;
	REQUIRE(b.Sync(skipBytes));
	REQUIRE(b.NumBytesRead() == 50);
	REQUIRE(b.NumWriteBytes() ==  0);
}

TEST_CASE(SUITE("SyncPartialPattern"))
{
	Buffer buffer(100);
	ShiftableBuffer b(buffer(), buffer.Size());

	//initialize buffer to all zeros
	for(size_t i = 0; i < b.NumWriteBytes(); ++i) b.WriteBuff()[i] = 0;

	b.WriteBuff()[97] = 0x05;
	b.AdvanceWrite(98);

	uint32_t skipBytes = 0;
	REQUIRE_FALSE(b.Sync(skipBytes));
	REQUIRE(b.NumBytesRead() == 1);
	REQUIRE(b.NumWriteBytes() ==  2);
}


