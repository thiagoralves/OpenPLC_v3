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
#ifndef ASIOPAL_TIMER_H
#define ASIOPAL_TIMER_H

#include <asio.hpp>

#include <openpal/executor/ITimer.h>
#include <openpal/util/Uncopyable.h>

#include "asiopal/SteadyClock.h"


namespace asiopal
{

/**
*
* Implementation of openpal::ITimer backed by asio::basic_waitable_timer<steady_clock>
*
*/
class Timer final : public openpal::ITimer, private openpal::Uncopyable
{
	friend class Executor;

public:

	Timer(asio::io_service& service);

	virtual void Cancel() override;

	virtual openpal::MonotonicTimestamp ExpiresAt() override;

private:

	asio::basic_waitable_timer< asiopal::steady_clock_t > timer;
};

}

#endif
