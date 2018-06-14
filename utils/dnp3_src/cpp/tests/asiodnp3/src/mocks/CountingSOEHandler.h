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

#ifndef OPENDNP3_COUNTINGSOEHANDLER_H
#define OPENDNP3_COUNTINGSOEHANDLER_H

#include "opendnp3/master/ISOEHandler.h"

#include <mutex>
#include <condition_variable>

namespace opendnp3
{

class CountingSOEHandler final : public opendnp3::ISOEHandler
{
	std::mutex mutex;
	std::condition_variable cv;
	size_t count = 0;

public:

	void WaitForCount(size_t num, std::chrono::steady_clock::duration timeout)
	{
		std::unique_lock<std::mutex> lock(mutex);
		auto equals_num = [this, num]() -> bool { return this->count == num; };

		if (!cv.wait_for(lock, timeout, equals_num))
		{
			throw std::logic_error("timeout waiting for count");
		}
		count -= num;
	}

	virtual void Start() override
	{
		mutex.lock();
	}

	virtual void End() override
	{
		mutex.unlock();
		cv.notify_all();
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Binary>>& values) override
	{
		count += values.Count();
	}
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<DoubleBitBinary>>& values) override
	{
		count += values.Count();
	}
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Analog>>& values) override
	{
		count += values.Count();
	}
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Counter>>& values) override
	{
		count += values.Count();
	}
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<FrozenCounter>>& values) override
	{
		count += values.Count();
	}
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryOutputStatus>>& values) override
	{
		count += values.Count();
	}
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogOutputStatus>>& values) override
	{
		count += values.Count();
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<OctetString>>& values) override
	{
		count += values.Count();
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryCommandEvent>>& values) override
	{
		count += values.Count();
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogCommandEvent>>& values) override
	{
		count += values.Count();
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<TimeAndInterval>>& values) override
	{
		count += values.Count();
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<SecurityStat>>& values) override
	{
		count += values.Count();
	}

	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::DNPTime>& values) override
	{
		count += values.Count();
	}
};

}

#endif

