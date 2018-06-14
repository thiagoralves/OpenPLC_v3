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

#include "mocks/MockTCPPair.h"

#include <iostream>

using namespace asiopal;

#define SUITE(name) "TCPClientServerSuite - " name

template <class F>
void WithIO(const F& fun)
{
	auto io = std::make_shared<MockIO>();
	fun(io);
	io->RunUntilOutOfWork();
}

TEST_CASE(SUITE("Client and server can connect"))
{
	auto iteration = []()
	{
		auto test = [](const std::shared_ptr<MockIO>& io)
		{
			MockTCPPair pair(io, 20000);
			pair.Connect(1);
		};

		WithIO(test);
	};

	// run multiple times to ensure the test is cleaning up after itself in terms of system resources
	for (int i = 0; i < 5; ++i)
	{
		iteration();
		//std::cout << "iteration: " << i << " complete" << std::endl;
	}
}










