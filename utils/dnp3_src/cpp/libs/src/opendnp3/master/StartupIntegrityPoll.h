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
#ifndef OPENDNP3_STARTUPINTEGRITYPOLL_H
#define OPENDNP3_STARTUPINTEGRITYPOLL_H

#include "opendnp3/app/ClassField.h"

#include "opendnp3/master/PollTaskBase.h"
#include "opendnp3/master/TaskPriority.h"

namespace opendnp3
{

class ISOEHandler;

/**
 * A generic interface for defining master request/response style tasks
 */
class StartupIntegrityPoll final : public PollTaskBase
{

public:

	StartupIntegrityPoll(IMasterApplication& app, ISOEHandler& soeHandler, ClassField classes, openpal::TimeDuration retryPeriod, openpal::Logger logger);

	virtual bool IsRecurring() const override
	{
		return true;
	}

	virtual bool BuildRequest(APDURequest& request, uint8_t seq) override;

	virtual int Priority() const override
	{
		return priority::INTEGRITY_POLL;
	}

	virtual bool BlocksLowerPriority() const override
	{
		return true;
	}

private:

	ClassField classes;

	openpal::TimeDuration retryPeriod;

	virtual bool IsEnabled() const override;

	virtual MasterTaskType GetTaskType() const override
	{
		return MasterTaskType::STARTUP_INTEGRITY_POLL;
	}

	virtual IMasterTask::TaskState OnTaskComplete(TaskCompletion result, openpal::MonotonicTimestamp now) override;

};


} //end ns


#endif
