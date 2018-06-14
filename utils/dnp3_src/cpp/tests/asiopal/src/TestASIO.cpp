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

#include <iostream>
#include <asio.hpp>
#include <functional>
#include <chrono>

using namespace std;
using namespace asio;

namespace asio
{
typedef asio::basic_waitable_timer<std::chrono::steady_clock> steady_timer;
}

void AssertCanceled(bool* apFlag, const std::error_code& ec)
{
	if(ec) *apFlag = true;
}

void Cancel(asio::basic_waitable_timer<std::chrono::steady_clock>* aptimer)
{
	aptimer->cancel();
}

#define SUITE(name) "TestBoostASIO - " name

TEST_CASE(SUITE("TimerCancel"))
{
	bool flag = false;

	io_service io;
	steady_timer t1(io, std::chrono::seconds(0));
	steady_timer t2(io, std::chrono::seconds(1));

	t1.async_wait(std::bind(Cancel, &t2));
	t2.async_wait(std::bind(AssertCanceled, &flag, std::placeholders::_1));

	io.run();

	REQUIRE(flag);
}



