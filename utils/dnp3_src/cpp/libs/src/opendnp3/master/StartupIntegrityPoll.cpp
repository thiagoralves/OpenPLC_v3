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
#include "StartupIntegrityPoll.h"

#include "MasterTasks.h"

#include "opendnp3/app/parsing/APDUParser.h"
#include "opendnp3/app/APDUBuilders.h"

#include "opendnp3/master/MeasurementHandler.h"

#include "opendnp3/LogLevels.h"

#include <openpal/logging/LogMacros.h>

using namespace openpal;

namespace opendnp3
{

StartupIntegrityPoll::StartupIntegrityPoll(IMasterApplication& app, ISOEHandler& soeHandler, ClassField classes_, TimeDuration retryPeriod_, openpal::Logger logger) :
	PollTaskBase(app, soeHandler, openpal::MonotonicTimestamp(0), logger, TaskConfig::Default()),
	classes(classes_),
	retryPeriod(retryPeriod_)
{

}

bool StartupIntegrityPoll::BuildRequest(APDURequest& request, uint8_t seq)
{
	build::ReadIntegrity(request, classes, seq);
	request.SetFunction(FunctionCode::READ);
	request.SetControl(AppControlField::Request(seq));
	return true;
}

bool StartupIntegrityPoll::IsEnabled() const
{
	return classes.HasAnyClass();
}

IMasterTask::TaskState StartupIntegrityPoll::OnTaskComplete(TaskCompletion result, openpal::MonotonicTimestamp now)
{
	switch (result)
	{
	case(TaskCompletion::FAILURE_NO_COMMS) :
		return TaskState::Immediately();

	case(TaskCompletion::SUCCESS) :
		return TaskState::Infinite();

	default:
		return TaskState::Infinite();
	}
}

} //end ns
