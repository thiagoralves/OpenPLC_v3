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
#ifndef ASIOPAL_SYNCHRONIZED_H
#define ASIOPAL_SYNCHRONIZED_H

#include <asio.hpp>
#include <mutex>
#include <condition_variable>

namespace asiopal
{

/**
* Provides thread-safe access to a value that can be set once.
*/
template <class T>
class Synchronized
{
public:

	Synchronized() : value(), isSet(false)
	{}

	T WaitForValue()
	{
		std::unique_lock<std::mutex> lock(mutex);
		while (!isSet)
		{
			auto complete = [this]()
			{
				return isSet;
			};
			condition.wait(lock, complete);
		}
		return value;
	}

	void SetValue(T value_)
	{
		std::unique_lock<std::mutex> lock(mutex);
		this->value = value_;
		isSet = true;
		condition.notify_all();
	}

private:

	T value;
	bool isSet;

	std::mutex mutex;
	std::condition_variable condition;
};

}

#endif



