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
#ifndef OPENPAL_TIMERREF_H
#define OPENPAL_TIMERREF_H

#include "openpal/executor/ITimer.h"
#include "openpal/executor/IExecutor.h"
#include "openpal/util/Uncopyable.h"

namespace openpal
{

/**
* A management class to make dealing with timer pointers safer a little safer
*
* Holds an optional pointer to an active ITimer pointer. Acts as a safe proxy for dealing with a recurring timer instance.
*/
class TimerRef : openpal::Uncopyable
{

public:
	TimerRef(openpal::IExecutor& executor);

	// automatically cancels any active timers on destructive
	~TimerRef();

	// Called to see if the timer is currently active
	bool IsActive() const;

	// return the expiration time, MonotonticTimestamp::Max() if not active
	MonotonicTimestamp ExpiresAt() const;

	// cancels any existing timer, returning true if the timer was active, false otherwise
	bool Cancel();

	// Attempts to start a new timer
	// returns false if timer is already active, true otherwise
	template <class Lambda>
	bool Start(const TimeDuration& timeout, const Lambda& action);

	template <class Lambda>
	bool Start(const MonotonicTimestamp& expiration, const Lambda& action);

	// Start a new timer, canceling any existing timer
	template <class Lambda>
	void Restart(const TimeDuration& expiration, const Lambda& action);

	template <class Lambda>
	void Restart(const MonotonicTimestamp& expiration, const Lambda& action);


private:

	// restart the timer, return false if already active
	bool StartAction(const TimeDuration& timeout, const action_t& action);
	bool StartAction(const MonotonicTimestamp& expiration, const action_t& action);

	// Start a new timer, canceling any existing timer
	void RestartAction(const TimeDuration& expiration, const action_t& action);
	void RestartAction(const MonotonicTimestamp& expiration, const action_t& action);


	IExecutor* pExecutor;
	ITimer* pTimer;
};

// Attempts to start a new timer
// returns false if timer is already active, true otherwise
template <class Lambda>
bool TimerRef::Start(const TimeDuration& timeout, const Lambda& action)
{
	auto proxy = [this, action]
	{
		pTimer = nullptr;
		action();
	};
	return this->StartAction(timeout, proxy);
}

template <class Lambda>
bool TimerRef::Start(const MonotonicTimestamp& expiration, const Lambda& action)
{
	auto proxy = [this, action]
	{
		pTimer = nullptr;
		action();
	};
	return this->StartAction(expiration, proxy);
}

template <class Lambda>
void TimerRef::Restart(const TimeDuration& timeout, const Lambda& action)
{
	auto proxy = [this, action]
	{
		pTimer = nullptr;
		action();
	};
	this->RestartAction(timeout, proxy);
}

template <class Lambda>
void TimerRef::Restart(const MonotonicTimestamp& expiration, const Lambda& action)
{
	auto proxy = [this, action]
	{
		pTimer = nullptr;
		action();
	};
	this->RestartAction(expiration, proxy);
}

}

#endif
