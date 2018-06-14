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
#include "openpal/executor/MonotonicTimestamp.h"

#include <stdint.h>

namespace openpal
{

MonotonicTimestamp MonotonicTimestamp::Max()
{
	return MonotonicTimestamp(INT64_MAX);
}

MonotonicTimestamp MonotonicTimestamp::Min()
{
	return MonotonicTimestamp(INT64_MIN);
}

bool MonotonicTimestamp::IsMax() const
{
	return milliseconds == INT64_MAX;
}

bool MonotonicTimestamp::IsMin() const
{
	return milliseconds == INT64_MIN;
}

MonotonicTimestamp::MonotonicTimestamp() : milliseconds(0)
{}

MonotonicTimestamp::MonotonicTimestamp(int64_t aMilliseconds) : milliseconds(aMilliseconds)
{}


MonotonicTimestamp MonotonicTimestamp::Add(const TimeDuration& duration) const
{
	const auto maximum = INT64_MAX - this->milliseconds;

	return duration.GetMilliseconds() >= maximum ? MonotonicTimestamp::Max() : MonotonicTimestamp(milliseconds + duration.GetMilliseconds());
}

bool operator==(const MonotonicTimestamp& first, const MonotonicTimestamp& second)
{
	return first.milliseconds == second.milliseconds;
}

bool operator<(const MonotonicTimestamp& first, const MonotonicTimestamp& second)
{
	return first.milliseconds < second.milliseconds;
}

bool operator>(const MonotonicTimestamp& first, const MonotonicTimestamp& second)
{
	return first.milliseconds > second.milliseconds;
}

}

