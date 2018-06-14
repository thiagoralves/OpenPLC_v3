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
#include "openpal/executor/TimeDuration.h"

#include "openpal/util/Limits.h"

namespace openpal
{

TimeDuration TimeDuration::Min()
{
	return TimeDuration(openpal::MinValue<int64_t>());
}

TimeDuration TimeDuration::Max()
{
	return TimeDuration(openpal::MaxValue<int64_t>());
}

TimeDuration TimeDuration::Zero()
{
	return TimeDuration(0);
}

TimeDuration TimeDuration::Milliseconds(int64_t milliseconds)
{
	return TimeDuration(milliseconds);
}

TimeDuration TimeDuration::Seconds(int64_t seconds)
{
	return TimeDuration(1000 * seconds);
}

TimeDuration TimeDuration::Minutes(int64_t minutes)
{
	return TimeDuration(static_cast<int64_t>(1000 * 60) * minutes);
}

TimeDuration TimeDuration::Hours(int64_t hours)
{
	return TimeDuration(static_cast<int64_t>(1000 * 60 * 60) * hours);
}

TimeDuration TimeDuration::Days(int64_t days)
{
	return TimeDuration(static_cast<int64_t>(1000 * 60 * 60 * 24) * days);
}

TimeDuration::TimeDuration() : TimeDurationBase(0) {}

TimeDuration::TimeDuration(int64_t aMilliseconds) : TimeDurationBase(aMilliseconds)
{}

bool operator==(const TimeDuration& lhs, const TimeDuration& rhs)
{
	return lhs.GetMilliseconds() == rhs.GetMilliseconds();
}

}
