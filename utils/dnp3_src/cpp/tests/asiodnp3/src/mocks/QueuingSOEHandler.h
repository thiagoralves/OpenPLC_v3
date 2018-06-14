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

#ifndef OPENDNP3_QUEUINGSOEHANDLER_H
#define OPENDNP3_QUEUINGSOEHANDLER_H

#include "opendnp3/master/ISOEHandler.h"

#include "ExpectedValue.h"

#include "SynchronizedQueue.h"

#include <mutex>
#include <vector>

namespace opendnp3
{

class QueuingSOEHandler final : public opendnp3::ISOEHandler
{
	std::mutex mutex;
	std::vector<ExpectedValue> temp;

	template <class T>
	void ProcessAny(const ICollection<Indexed<T>>& values)
	{
		auto add = [this](const Indexed<T>& item)
		{
			temp.push_back(ExpectedValue(item.value, item.index));
		};

		values.ForeachItem(add);
	}

public:

	asiodnp3::SynchronizedQueue<opendnp3::ExpectedValue> values;

	virtual void Start() override
	{
		mutex.lock();
	}

	virtual void End() override
	{
		values.AddMany(temp);
		temp.clear();
		mutex.unlock();
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Binary>>& values) override
	{
		this->ProcessAny(values);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<DoubleBitBinary>>& values) override
	{
		this->ProcessAny(values);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Analog>>& values) override
	{
		this->ProcessAny(values);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Counter>>& values) override
	{
		this->ProcessAny(values);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<FrozenCounter>>& values) override
	{
		this->ProcessAny(values);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryOutputStatus>>& values) override
	{
		this->ProcessAny(values);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogOutputStatus>>& values) override
	{
		this->ProcessAny(values);
	}

	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::DNPTime>& values) override {}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<OctetString>>& values) override {}
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryCommandEvent>>& values) override {}
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogCommandEvent>>& values) override {}
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<TimeAndInterval>>& values) override {}
	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<SecurityStat>>& values) override {}
};

}

#endif

