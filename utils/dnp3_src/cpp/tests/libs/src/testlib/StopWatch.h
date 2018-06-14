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
#ifndef TESTLIB_STOP_WATCH_H
#define TESTLIB_STOP_WATCH_H

#include <chrono>

namespace testlib
{

/**
	This class is designed to make it easier to do simple timing tests
*/
class StopWatch
{
public:

	StopWatch();

	//get the elapsed time since creation or the last restart
	//by default each call to Elapsed restarts the timer.
	std::chrono::steady_clock::duration Elapsed(bool aReset = true);

	//restart or re-zero the StopWatch.
	void Restart();

private:
	std::chrono::steady_clock::time_point mStartTime;
};


}

#endif
