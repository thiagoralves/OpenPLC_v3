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
#ifndef ASIOPAL_EXECUTOR_H
#define ASIOPAL_EXECUTOR_H

#include <openpal/executor/IExecutor.h>
#include <openpal/util/Uncopyable.h>

#include "asiopal/IO.h"
#include "asiopal/SteadyClock.h"

#include <future>

namespace asiopal
{

/**
*
* Implementation of openpal::IExecutor backed by asio::strand
*
* Shutdown life-cycle guarantees are provided by using std::shared_ptr
*
*/
class Executor final :
	public openpal::IExecutor,
	public std::enable_shared_from_this<Executor>,
	private openpal::Uncopyable
{

public:

	Executor(const std::shared_ptr<IO>& io);

	static std::shared_ptr<Executor> Create(const std::shared_ptr<IO>& io)
	{
		return std::make_shared<Executor>(io);
	}

	/// ---- Implement IExecutor -----

	virtual openpal::MonotonicTimestamp GetTime() override;
	virtual openpal::ITimer* Start(const openpal::TimeDuration&, const openpal::action_t& runnable)  override;
	virtual openpal::ITimer* Start(const openpal::MonotonicTimestamp&, const openpal::action_t& runnable)  override;
	virtual void Post(const openpal::action_t& runnable) override;

	template <class T>
	T ReturnFrom(const std::function<T()>& action);

	void BlockUntil(const std::function<void ()>& action);

	void BlockUntilAndFlush(const std::function<void()>& action);

private:

	// we hold a shared_ptr to the pool so that it cannot dissapear while the strand is still executing
	std::shared_ptr<IO> io;

public:

	// Create a new Executor that shares the underling std::shared_ptr<IO>
	std::shared_ptr<Executor> Fork() const
	{
		return Create(this->io);
	}

	asio::strand strand;

private:

	openpal::ITimer* Start(const steady_clock_t::time_point& expiration, const openpal::action_t& runnable);

};

template <class T>
T Executor::ReturnFrom(const std::function<T()>& action)
{
	if (strand.running_in_this_thread())
	{
		return action();
	}

	std::promise<T> ready;

	auto future = ready.get_future();

	auto run = [&]
	{
		ready.set_value(action());
	};

	strand.post(run);

	future.wait();

	return future.get();
}


}

#endif

