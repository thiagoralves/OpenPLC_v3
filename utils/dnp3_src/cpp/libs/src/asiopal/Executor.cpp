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

#include "asiopal/Executor.h"
#include "asiopal/Timer.h"

#include "asiopal/TimeConversions.h"

#include <chrono>

using namespace openpal;

namespace asiopal
{

Executor::Executor(const std::shared_ptr<IO>& io) :
	io(io),
	strand(io->service)
{

}

MonotonicTimestamp Executor::GetTime()
{
	return TimeConversions::Convert(steady_clock_t::now());
}

ITimer* Executor::Start(const TimeDuration& delay, const action_t& runnable)
{
	const auto now = steady_clock_t::now();
	const auto max_ms = std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock_t::time_point::max() - now).count();
	const auto expiration = (delay.milliseconds > max_ms) ? steady_clock_t::time_point::max() : (now + std::chrono::milliseconds(delay.milliseconds));

	return Start(expiration, runnable);
}

ITimer* Executor::Start(const MonotonicTimestamp& time, const action_t& runnable)
{
	return Start(TimeConversions::Convert(time), runnable);
}

openpal::ITimer* Executor::Start(const steady_clock_t::time_point& expiration, const openpal::action_t& runnable)
{
	auto timer = std::make_shared<Timer>(this->strand.get_io_service());

	timer->timer.expires_at(expiration);

	// neither the executor nor the timer can be deleted while the timer is still active
	auto callback = [timer, runnable, self = shared_from_this()](const std::error_code & ec)
	{
		if (!ec)   // an error indicate timer was canceled
		{
			runnable();
		}
	};

	timer->timer.async_wait(strand.wrap(callback));

	return timer.get();
}

void Executor::Post(const action_t& runnable)
{
	auto callback = [runnable, self = shared_from_this()]()
	{
		runnable();
	};
	strand.post(callback);
}

void Executor::BlockUntil(const std::function<void()>& action)
{
	if (strand.running_in_this_thread())
	{
		action();
		return;
	}

	std::promise<bool> ready;

	auto future = ready.get_future();

	auto run = [&]
	{
		action();
		ready.set_value(true);
	};

	strand.post(run);

	future.wait();
}

void Executor::BlockUntilAndFlush(const std::function<void()>& action)
{
	this->BlockUntil(action);
	this->BlockUntil([]() {});
}

}
