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
#ifndef __MOCK_EXECUTOR_H_
#define __MOCK_EXECUTOR_H_

#include <openpal/executor/IExecutor.h>

#include <map>
#include <queue>
#include <limits>
#include <stddef.h>

namespace testlib
{

class MockTimer;

/** @section desc Test class that doles out MockTimer* */
class MockExecutor : public openpal::IExecutor
{
	friend class MockTimer;

public:
	MockExecutor();
	~MockExecutor();

	// Implement IExecutor
	virtual openpal::ITimer* Start(const openpal::MonotonicTimestamp&, const openpal::action_t& runnable) override;
	virtual openpal::ITimer* Start(const openpal::TimeDuration&, const openpal::action_t& runnable) override;
	virtual void Post(const openpal::action_t& runnable) override;
	virtual openpal::MonotonicTimestamp GetTime() override;

	/** Turns the auto-post feature on/off. When Auto post is on, Post() is executed synchronously */
	void SetAutoPost(bool aAutoPost)
	{
		this->mPostIsSynchronous = aAutoPost;
	}

	/**	@return true if an action was run. */
	bool RunOne();

	/** Calls RunOne() up to some maximum number of times continuing while
		there are still events to dispatch

		@return the number of events dispatched
	*/
	size_t RunMany(size_t aMaximum = std::numeric_limits<size_t>::max());

	/** @return The number of active, pending timers and post operations */
	size_t NumActive() const
	{
		return postQueue.size();
	}

	size_t NumPendingTimers() const
	{
		return timers.size();
	}

	openpal::MonotonicTimestamp NextTimerExpiration();

	size_t AdvanceTime(openpal::TimeDuration aDuration);

	// doesn't check timers
	void AddTime(openpal::TimeDuration aDuration);

	bool AdvanceToNextTimer();

private:

	size_t CheckForExpiredTimers();

	bool FindExpiredTimer();

	void Cancel(openpal::ITimer* apTimer);

	typedef std::deque<openpal::action_t> PostQueue;
	typedef std::vector<MockTimer*> TimerVector;

	bool mPostIsSynchronous;
	openpal::MonotonicTimestamp mCurrentTime;

	PostQueue postQueue;
	TimerVector timers;
};

/** @section desc Test timer class used in conjunction with MockExecutor */
class MockTimer : public openpal::ITimer
{
	friend class MockExecutor;

public:
	MockTimer(MockExecutor*, const openpal::MonotonicTimestamp&, const openpal::action_t& runnable);

	//implement ITimer
	void Cancel();
	openpal::MonotonicTimestamp ExpiresAt();

private:
	openpal::MonotonicTimestamp mTime;
	MockExecutor* mpSource;
	openpal::action_t runnable;
};

}

#endif

