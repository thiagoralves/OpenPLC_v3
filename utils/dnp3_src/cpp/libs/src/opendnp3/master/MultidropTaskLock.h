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
#ifndef OPENDNP3_MULTIDROPTASKLOCK_H
#define OPENDNP3_MULTIDROPTASKLOCK_H

#include "opendnp3/master/ITaskLock.h"

#include <set>
#include <deque>

namespace opendnp3
{

class MultidropTaskLock final : public opendnp3::ITaskLock
{
public:

	MultidropTaskLock();

	/// these are controlled by the link layer router
	void SetOnline()
	{
		m_is_online = true;
	}

	void SetOffline()
	{
		m_is_online = false;
	}

	virtual bool Acquire(IScheduleCallback&) override;
	virtual bool Release(IScheduleCallback&) override;

private:

	bool AddIfNotContained(IScheduleCallback&);

	bool m_is_online;
	std::set<IScheduleCallback*> m_callback_set;
	std::deque<IScheduleCallback*> m_callback_queue;

	IScheduleCallback* m_active;
};

}

#endif
