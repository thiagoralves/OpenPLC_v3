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
#ifndef DNP3MOCKS_COMMAND_CALLBACK_QUEUE_H
#define DNP3MOCKS_COMMAND_CALLBACK_QUEUE_H

#include <queue>
#include <functional>

#include <opendnp3/master/CommandPointResult.h>

namespace opendnp3
{

class MockCommandResultType final : public IVisitor<CommandPointResult>
{
public:

	MockCommandResultType(TaskCompletion result_) : summary(result_)
	{}

	virtual void OnValue(const CommandPointResult& value) override
	{
		results.push_back(value);
	}

	bool Equals(TaskCompletion summary_, CommandPointResult result) const
	{
		return (results.size() == 1) && (summary_ == summary) && result.Equals(results.front());
	}

	TaskCompletion summary;
	std::vector<opendnp3::CommandPointResult> results;
};

class CommandCallbackQueue
{
public:

	std::function<void(const ICommandTaskResult&)> Callback()
	{
		return [this](const ICommandTaskResult & rsp) -> void
		{
			MockCommandResultType result(rsp.summary);
			rsp.Foreach(result);
			values.push_back(result);
		};
	}

	bool PopOnlyEqualValue(TaskCompletion summary, CommandPointResult item)
	{
		return PopOnlyEqualValue(summary, { item });
	}

	bool PopOnlyEqualValue(TaskCompletion summary, std::initializer_list<CommandPointResult> list)
	{
		if (values.size() != 1) return false;

		auto value = values.front();
		values.pop_front();

		if (value.summary != summary)
		{
			return false;
		}

		if (list.size() != value.results.size())
		{
			return false;
		}

		uint32_t i = 0;

		for (auto& item : list)
		{
			if (!value.results[i].Equals(item))
			{
				return false;
			}

			++i;
		}

		return true;
	}

	std::deque<MockCommandResultType> values;

};

}

#endif

