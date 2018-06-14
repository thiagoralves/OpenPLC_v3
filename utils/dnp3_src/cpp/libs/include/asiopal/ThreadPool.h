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
#ifndef ASIOPAL_THREADPOOL_H
#define ASIOPAL_THREADPOOL_H

#include <openpal/logging/Logger.h>

#include "asiopal/SteadyClock.h"
#include "asiopal/Executor.h"

#include <functional>
#include <thread>
#include <memory>

namespace asiopal
{

/**
*	A thread pool that calls asio::io_service::run
*/
class ThreadPool
{
public:

	friend class ThreadPoolTest;

	ThreadPool(
	    const openpal::Logger& logger,
	    const std::shared_ptr<IO>& io,
	    uint32_t concurrency,
	std::function<void()> onThreadStart = []() {},
	std::function<void()> onThreadExit = []() {}
	);

	~ThreadPool();

	inline std::shared_ptr<Executor> CreateExecutor() const
	{
		return Executor::Create(io);
	}

	void Shutdown();

private:

	openpal::Logger logger;
	const std::shared_ptr<IO> io;

	std::function<void ()> onThreadStart;
	std::function<void ()> onThreadExit;

	bool isShutdown;

	void Run(int threadnum);

	asio::basic_waitable_timer< asiopal::steady_clock_t > infiniteTimer;
	std::vector<std::unique_ptr<std::thread>> threads;
};

}


#endif
