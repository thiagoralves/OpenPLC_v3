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
#ifndef OPENDNP3_CLEARRESTARTTASK_H
#define OPENDNP3_CLEARRESTARTTASK_H

#include "opendnp3/master/IMasterTask.h"
#include "opendnp3/master/TaskPriority.h"

namespace opendnp3
{

/**
* Clear the IIN restart bit
*/
class ClearRestartTask final : public IMasterTask
{

public:

	ClearRestartTask(IMasterApplication& application, openpal::TimeDuration retryPeriod, openpal::Logger logger);

	virtual char const* Name() const override
	{
		return "Clear Restart IIN";
	}

	virtual bool IsRecurring() const override
	{
		return true;
	}

	virtual int Priority() const override
	{
		return priority::CLEAR_RESTART;
	}

	virtual bool BlocksLowerPriority() const override
	{
		return true;
	}

	virtual bool BuildRequest(APDURequest& request, uint8_t seq) override;

private:

	virtual MasterTaskType GetTaskType() const override
	{
		return MasterTaskType::CLEAR_RESTART;
	}

	virtual bool IsEnabled() const override
	{
		return true;
	}

	virtual IMasterTask::TaskState OnTaskComplete(TaskCompletion result, openpal::MonotonicTimestamp now) override;

	virtual ResponseResult ProcessResponse(const APDUResponseHeader& response, const openpal::RSlice& objects) override;

	openpal::TimeDuration retryPeriod;
};

} //end ns


#endif
