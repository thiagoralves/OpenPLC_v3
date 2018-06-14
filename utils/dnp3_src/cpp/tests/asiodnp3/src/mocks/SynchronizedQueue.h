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

#ifndef OPENDNP3_SYNCHRONIZEDQUEUE_H
#define OPENDNP3_SYNCHRONIZEDQUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace asiodnp3
{
template <class T>
class SynchronizedQueue
{
	std::deque<T> queue;
	std::mutex mutex;
	std::condition_variable cv;

public:

	template <class U>
	void AddMany(const U& collection)
	{
		{
			std::unique_lock<std::mutex> lock(mutex);
			for (auto& item : collection)
			{
				queue.push_back(item);
			}
		}
		cv.notify_one();
	}

	void Add(const T& item)
	{
		{
			std::unique_lock<std::mutex> lock(mutex);
			queue.push_back(item);
		}
		cv.notify_one();
	}

	template <class U>
	size_t DrainTo(U& collection, const std::chrono::steady_clock::duration& duration)
	{
		const auto timeout = std::chrono::steady_clock::now() + duration;

		std::unique_lock<std::mutex> lock(mutex);

		cv.wait_until(lock, timeout, [this]() -> bool { return !this->queue.empty(); });

		for (auto& item : queue)
		{
			collection.push_back(item);
		}

		const auto ret = queue.size();

		queue.clear();

		return ret;
	}

};

}

#endif

