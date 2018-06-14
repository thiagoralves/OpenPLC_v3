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
#include "MockExecutor.h"

#include <algorithm>

using namespace openpal;

namespace testlib
{

MockExecutor::MockExecutor() :
	mPostIsSynchronous(false),
	mCurrentTime(0)
{

}

MockExecutor::~MockExecutor()
{
	for(auto pTimer : timers) delete pTimer;
}

openpal::MonotonicTimestamp MockExecutor::NextTimerExpiration()
{
	auto lt = [](MockTimer * pLHS, MockTimer * pRHS)
	{
		return pLHS->ExpiresAt() < pRHS->ExpiresAt();
	};
	auto min = std::min_element(timers.begin(), timers.end(), lt);
	if (min == timers.end())
	{
		return MonotonicTimestamp::Max();
	}
	else
	{
		return (*min)->ExpiresAt();
	}
}

size_t MockExecutor::CheckForExpiredTimers()
{
	size_t count = 0;
	while (FindExpiredTimer())
	{
		++count;
	}
	return count;
}

bool MockExecutor::FindExpiredTimer()
{
	auto expired = [this](MockTimer * pTimer)
	{
		return pTimer->ExpiresAt().milliseconds <= this->mCurrentTime.milliseconds;
	};
	auto iter = std::find_if(timers.begin(), timers.end(), expired);
	if (iter == timers.end())
	{
		return false;
	}
	else
	{
		this->postQueue.push_back((*iter)->runnable);
		delete (*iter);
		timers.erase(iter);
		return true;
	}
}

size_t MockExecutor::AdvanceTime(TimeDuration aDuration)
{
	this->AddTime(aDuration);
	return this->CheckForExpiredTimers();
}

// doesn't check timers
void MockExecutor::AddTime(openpal::TimeDuration aDuration)
{
	mCurrentTime = mCurrentTime.Add(aDuration);
}

bool MockExecutor::AdvanceToNextTimer()
{
	if (timers.empty())
	{
		return false;
	}
	else
	{
		auto timestamp = NextTimerExpiration();
		if (timestamp > mCurrentTime)
		{
			mCurrentTime = timestamp;
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool MockExecutor::RunOne()
{
	this->CheckForExpiredTimers();

	if(postQueue.size() > 0)
	{
		postQueue.front()();
		postQueue.pop_front();
		return true;
	}
	else
	{
		return false;
	}
}

size_t MockExecutor::RunMany(size_t aMaximum)
{
	size_t num = 0;
	while(num < aMaximum && this->RunOne()) ++num;
	return num;
}

void MockExecutor::Post(const openpal::action_t& runnable)
{
	if (mPostIsSynchronous)
	{
		runnable();
	}
	else
	{
		postQueue.push_back(runnable);
	}
}

openpal::MonotonicTimestamp MockExecutor::GetTime()
{
	return mCurrentTime;
}

ITimer* MockExecutor::Start(const openpal::TimeDuration& aDelay, const openpal::action_t& runnable)
{
	auto expiration = mCurrentTime.Add(aDelay);
	return Start(expiration, runnable);
}

ITimer* MockExecutor::Start(const openpal::MonotonicTimestamp& arTime, const openpal::action_t& runnable)
{
	MockTimer* pTimer = new MockTimer(this, arTime, runnable);
	timers.push_back(pTimer);
	return pTimer;
}

void MockExecutor::Cancel(ITimer* pTimer)
{
	for (TimerVector::iterator i = timers.begin(); i != timers.end(); ++i)
	{
		if(*i == pTimer)
		{
			delete pTimer;
			timers.erase(i);
			return;
		}
	}
}

MockTimer::MockTimer(MockExecutor* source, const openpal::MonotonicTimestamp& time, const openpal::action_t& runnable) :
	mTime(time),
	mpSource(source),
	runnable(runnable)
{

}

void MockTimer::Cancel()
{
	mpSource->Cancel(this);
}

openpal::MonotonicTimestamp MockTimer::ExpiresAt()
{
	return mTime;
}

}


