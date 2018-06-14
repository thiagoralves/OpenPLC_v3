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
#ifndef OPENDNP3_CHANNEL_RETRY_H
#define OPENDNP3_CHANNEL_RETRY_H

#include <openpal/executor/TimeDuration.h>

#include "asiopal/IOpenDelayStrategy.h"


namespace asiopal
{

/// Class used to configure how channel failures are retried
class ChannelRetry
{

public:

	/*
	* Construct a channel retry config class
	*
	* @param minOpenRetry minimum connection retry interval on failure
	* @param maxOpenRetry maximum connection retry interval on failure
	*/
	ChannelRetry(
	    openpal::TimeDuration minOpenRetry,
	    openpal::TimeDuration maxOpenRetry,
	    IOpenDelayStrategy& strategy = ExponentialBackoffStrategy::Instance()
	);

	/// Return the default configuration of exponential backoff from 1 sec to 1 minute
	static ChannelRetry Default();

	/// minimum connection retry interval on failure
	openpal::TimeDuration minOpenRetry;
	/// maximum connection retry interval on failure
	openpal::TimeDuration maxOpenRetry;

	openpal::TimeDuration NextDelay(const openpal::TimeDuration& current) const
	{
		return strategy.GetNextDelay(current, maxOpenRetry);
	}

private:

	//// Strategy to use (default to exponential backoff)
	IOpenDelayStrategy& strategy;

};

}

#endif
