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
#include "MultidropTaskLock.h"

namespace opendnp3
{

MultidropTaskLock::MultidropTaskLock() : m_is_online(false), m_active(nullptr)
{

}

bool MultidropTaskLock::Acquire(IScheduleCallback& callback)
{
	if (!m_is_online)
	{
		return false;
	}

	if (m_active)
	{
		if (&callback == m_active)
		{
			return true;
		}


		this->AddIfNotContained(callback);
		return false;
	}
	else
	{
		m_active = &callback;
		return true;
	}
}

bool MultidropTaskLock::Release(IScheduleCallback& callback)
{
	if (m_active != &callback)
	{
		return false;
	}

	m_active = nullptr;

	if (!m_is_online)
	{
		return true;
	}

	if (m_callback_queue.empty())
	{
		return true;
	}

	m_active = m_callback_queue.front();
	m_callback_queue.pop_front();
	m_callback_set.erase(m_active);
	m_active->OnPendingTask();
	return true;
}

bool MultidropTaskLock::AddIfNotContained(IScheduleCallback& callback)
{
	auto result = m_callback_set.find(&callback);
	if (result != m_callback_set.end())
	{
		return false;
	}

	m_callback_set.insert(&callback);
	m_callback_queue.push_back(&callback);
	return true;
}

}

