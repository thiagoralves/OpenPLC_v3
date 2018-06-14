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
#ifndef OPENPAL_TIMEDURATION_H
#define OPENPAL_TIMEDURATION_H

#include <cstdint>

namespace openpal
{

template <class T>
class TimeDurationBase
{

public:

	TimeDurationBase() : milliseconds(0)
	{}

	T GetMilliseconds() const
	{
		return milliseconds;
	}

	bool IsNegative() const
	{
		return milliseconds < 0;
	}

	operator T() const
	{
		return milliseconds;
	}

	T milliseconds;

protected:

	TimeDurationBase(T milliseconds) : milliseconds(milliseconds)
	{}

};


/**
*  Strong typing for millisecond based time durations
*/
class TimeDuration : public TimeDurationBase<int64_t>
{

public:

	TimeDuration();

	bool IsPostive() const;

	TimeDuration MultiplyBy(int factor) const
	{
		return TimeDuration(factor * milliseconds);
	}

	static TimeDuration Min();

	static TimeDuration Max();

	static TimeDuration Zero();

	static TimeDuration Milliseconds(int64_t milliseconds);

	static TimeDuration Seconds(int64_t seconds);

	static TimeDuration Minutes(int64_t minutes);

	static TimeDuration Hours(int64_t hours);

	static TimeDuration Days(int64_t days);

private:
	TimeDuration(int64_t aMilliseconds);
};

bool operator==(const TimeDuration& lhs, const TimeDuration& rhs);

}

#endif
