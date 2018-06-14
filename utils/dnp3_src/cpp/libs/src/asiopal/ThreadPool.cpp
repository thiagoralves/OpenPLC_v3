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
#include "asiopal/ThreadPool.h"

#include <openpal/logging/LogMacros.h>
#include <openpal/logging/LogLevels.h>

#include <asiopal/SteadyClock.h>

using namespace std;
using namespace std::chrono;
using namespace openpal;

namespace asiopal
{

ThreadPool::ThreadPool(
    const openpal::Logger& logger,
    const std::shared_ptr<IO>& io,
    uint32_t concurrency,
    std::function<void()> onThreadStart,
    std::function<void()> onThreadExit) :
	logger(logger),
	io(io),
	onThreadStart(onThreadStart),
	onThreadExit(onThreadExit),
	isShutdown(false),
	infiniteTimer(io->service)
{
	if(concurrency == 0)
	{
		concurrency = 1;
		SIMPLE_LOG_BLOCK(this->logger, logflags::WARN, "Concurrency was set to 0, defaulting to 1 thread");
	}

	infiniteTimer.expires_at(asiopal::steady_clock_t::time_point::max());
	infiniteTimer.async_wait([](const std::error_code&) {});
	for(uint32_t i = 0; i < concurrency; ++i)
	{
		auto run = [this, i]()
		{
			this->Run(i);
		};
		threads.push_back(std::make_unique<thread>(run));
	}
}

ThreadPool::~ThreadPool()
{
	this->Shutdown();
	threads.clear();
}

void ThreadPool::Shutdown()
{
	if(!isShutdown)
	{
		isShutdown = true;
		infiniteTimer.cancel();
		for (auto& thread : threads)
		{
			thread->join();
		}
	}
}

void ThreadPool::Run(int threadnum)
{
	onThreadStart();

	FORMAT_LOG_BLOCK(this->logger, logflags::INFO, "Starting thread (%d)", threadnum);

	this->io->service.run();

	FORMAT_LOG_BLOCK(this->logger, logflags::INFO, "Exiting thread (%d)", threadnum);

	onThreadExit();
}

}
