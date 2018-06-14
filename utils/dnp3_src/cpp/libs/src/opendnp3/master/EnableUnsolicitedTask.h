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
#ifndef OPENDNP3_ENABLEUNSOLICITEDTASK_H
#define OPENDNP3_ENABLEUNSOLICITEDTASK_H


#include "opendnp3/app/ClassField.h"

#include "opendnp3/master/IMasterTask.h"
#include "opendnp3/master/TaskPriority.h"


namespace openpal
{
class IExecutor;
}

namespace opendnp3
{

/**
* Base class for tasks that only require a single response
*/

class EnableUnsolicitedTask final : public IMasterTask
{

public:

	EnableUnsolicitedTask(IMasterApplication& app, ClassField enabledClasses, openpal::TimeDuration retryPeriod, openpal::Logger logger);

	virtual bool IsRecurring() const override
	{
		return true;
	}

	virtual char const* Name() const override
	{
		return "Enable Unsolicited";
	}

	virtual bool BuildRequest(APDURequest& request, uint8_t seq) override;

	virtual int Priority() const override
	{
		return priority::ENABLE_UNSOLICITED;
	}

	virtual bool BlocksLowerPriority() const override
	{
		return true;
	}

private:

	ClassField enabledClasses;
	openpal::TimeDuration retryPeriod;

	virtual MasterTaskType GetTaskType() const override
	{
		return MasterTaskType::ENABLE_UNSOLICITED;
	}

	virtual ResponseResult ProcessResponse(const opendnp3::APDUResponseHeader& header, const openpal::RSlice& objects) override;

	virtual bool IsEnabled() const override;

	virtual TaskState OnTaskComplete(TaskCompletion result, openpal::MonotonicTimestamp now) override;

};

} //end ns


#endif
