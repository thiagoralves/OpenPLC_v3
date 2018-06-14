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
#include "MasterScheduler.h"

#include "opendnp3/master/TaskComparison.h"

#include <openpal/executor/MonotonicTimestamp.h>

#include <algorithm>

using namespace openpal;

namespace opendnp3
{

MasterScheduler::MasterScheduler(ITaskFilter& filter) :
	m_filter(&filter)
{

}

void MasterScheduler::Schedule(const std::shared_ptr<IMasterTask>& task)
{
	m_tasks.push_back(task);
	this->RecalculateTaskStartTimeout();
}

std::vector<std::shared_ptr<IMasterTask>>::iterator MasterScheduler::GetNextTask(const MonotonicTimestamp& now)
{
	auto runningBest = m_tasks.begin();

	if (!m_tasks.empty())
	{
		auto current = m_tasks.begin();
		++current;

		for (; current != m_tasks.end(); ++current)
		{
			auto result = TaskComparison::SelectHigherPriority(now, **runningBest, **current, *m_filter);
			if (result == TaskComparison::Result::Right)
			{
				runningBest = current;
			}
		}
	}

	return runningBest;
}

std::shared_ptr<IMasterTask> MasterScheduler::GetNext(const MonotonicTimestamp& now, MonotonicTimestamp& next)
{
	auto elem = GetNextTask(now);

	if (elem == m_tasks.end())
	{
		next = MonotonicTimestamp::Max();
		return nullptr;
	}
	else
	{
		const bool EXPIRED = (*elem)->ExpirationTime().milliseconds <= now.milliseconds;
		const bool CAN_RUN = this->m_filter->CanRun(**elem);

		if (EXPIRED && CAN_RUN)
		{
			std::shared_ptr<IMasterTask> ret = *elem;
			m_tasks.erase(elem);
			return ret;
		}
		else
		{
			next = CAN_RUN ? (*elem)->ExpirationTime() : MonotonicTimestamp::Max();
			return nullptr;
		}
	}
}

void MasterScheduler::Shutdown(const MonotonicTimestamp& now)
{
	m_tasks.clear();
}

bool MasterScheduler::IsTimedOut(const MonotonicTimestamp& now, const std::shared_ptr<IMasterTask>& task)
{
	if (task->IsRecurring() || task->StartExpirationTime() > now)
	{
		return false;
	}

	task->OnStartTimeout(now);

	return true;
}

void MasterScheduler::CheckTaskStartTimeout(const openpal::MonotonicTimestamp& now)
{
	auto timedOut = [this, now](const std::shared_ptr<IMasterTask>& task)
	{
		return this->IsTimedOut(now, task);
	};

	// erase-remove idion (https://en.wikipedia.org/wiki/Erase-remove_idiom)
	m_tasks.erase(std::remove_if(m_tasks.begin(), m_tasks.end(), timedOut), m_tasks.end());
}

void MasterScheduler::RecalculateTaskStartTimeout()
{
	auto min = MonotonicTimestamp::Max();

	for(auto& task : m_tasks)
	{
		if (!task->IsRecurring() && (task->StartExpirationTime() < min))
		{
			min = task->StartExpirationTime();
		}
	}

	this->m_filter->SetTaskStartTimeout(min);
}

}

