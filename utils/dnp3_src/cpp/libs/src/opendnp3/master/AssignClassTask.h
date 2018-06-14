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
#ifndef OPENDNP3_ASSIGNCLASSTASK_H
#define OPENDNP3_ASSIGNCLASSTASK_H

#include "opendnp3/master/IMasterTask.h"
#include "opendnp3/master/TaskPriority.h"

namespace opendnp3
{

class IMasterApplication;

class AssignClassTask final : public IMasterTask
{

public:

	AssignClassTask(IMasterApplication& application, openpal::TimeDuration retryPeriod, openpal::Logger logger);

	virtual char const* Name() const override
	{
		return "Assign Class";
	}

	virtual bool IsRecurring() const override
	{
		return true;
	}

	virtual bool BuildRequest(APDURequest& request, uint8_t seq) override;

	virtual int Priority(void) const override
	{
		return priority::ASSIGN_CLASS;
	}

	virtual bool BlocksLowerPriority() const override
	{
		return true;
	}

private:

	openpal::TimeDuration retryPeriod;

	virtual MasterTaskType GetTaskType() const override
	{
		return MasterTaskType::ASSIGN_CLASS;
	}

	virtual ResponseResult ProcessResponse(const opendnp3::APDUResponseHeader& header, const openpal::RSlice& objects) override;

	virtual TaskState OnTaskComplete(TaskCompletion result, openpal::MonotonicTimestamp now) override;

	virtual bool IsEnabled() const override;

};


} //end ns


#endif
