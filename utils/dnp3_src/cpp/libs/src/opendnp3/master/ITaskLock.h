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
#ifndef OPENDNP3_ITASKLOCK_H
#define OPENDNP3_ITASKLOCK_H

#include <openpal/util/Uncopyable.h>

#include "opendnp3/master/IScheduleCallback.h"

namespace opendnp3
{

/**
	Interface used in multi-drop configurations (multiple masters on same channel)
	to control task execution such that only 1 master is running a task at a time.
*/
class ITaskLock
{
public:

	/// Acquire a lock
	virtual bool Acquire(IScheduleCallback&) = 0;

	/// Release a lock
	virtual bool Release(IScheduleCallback&) = 0;

};

class NullTaskLock final : public ITaskLock, private openpal::Uncopyable
{
public:

	virtual bool Acquire(IScheduleCallback&) override
	{
		return true;
	}

	virtual bool Release(IScheduleCallback&) override
	{
		return true;
	}

	static ITaskLock& Instance();

private:

	NullTaskLock() {}

	static NullTaskLock instance;
};

}

#endif
