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

#ifndef ASIOPAL_STEADYCLOCK_H
#define ASIOPAL_STEADYCLOCK_H

#include <chrono>

namespace asiopal
{

#if (defined WIN32 && (_MSC_VER < 1900)) // Windows with VS eariler than 2015, e.g. 2013

#include <Windows.h>

/*
Custom steady clock implementation to handle the situation where the Windows steady clock
implementation is not monotonic. Normal steady clock implementation is used on other platforms.
*/
struct SteadyClockWindows
{
	// Type defininitions for this clock - ticks are in nanoseconds for this clock
	typedef LONGLONG representation;
	typedef std::ratio_multiply<std::ratio<1, 1>, std::nano> period;
	typedef std::chrono::duration<representation, period> duration;
	typedef std::chrono::time_point<SteadyClockWindows, duration> time_point;


	static time_point now()
	{
		// Special case for WIN32 because std::chrono::steady_clock is broken
		LARGE_INTEGER freq;
		LONGLONG nanoSecondsPerTick = 0;
		if (QueryPerformanceFrequency(&freq))
		{
			nanoSecondsPerTick = LONGLONG(1000000000.0L / freq.QuadPart);
		}

		LARGE_INTEGER performanceCount;
		if (nanoSecondsPerTick <= 0 || !QueryPerformanceCounter(&performanceCount))
		{
			// Error condition, return 0 time
			return time_point();
		}

		// Return time in nanoseconds
		LONGLONG timeNanos = performanceCount.QuadPart * nanoSecondsPerTick;
		return time_point(duration(timeNanos));
	}
};

// use this custom steady clock
typedef SteadyClockWindows steady_clock_t;

#else

// In all other situations use the normal steady clock
typedef std::chrono::steady_clock steady_clock_t;

#endif

}

#endif