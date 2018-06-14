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
#include "UserPollTask.h"

using namespace openpal;

namespace opendnp3
{

UserPollTask::UserPollTask(
    const HeaderBuilderT& builder_,
    bool recurring_,
    openpal::TimeDuration period_,
    openpal::TimeDuration retryDelay_,
    IMasterApplication& app,
    ISOEHandler& soeHandler,
    openpal::Logger logger,
    TaskConfig config
) :
	PollTaskBase(app, soeHandler, openpal::MonotonicTimestamp(0), logger, config),
	builder(builder_),
	recurring(recurring_),
	period(period_),
	retryDelay(retryDelay_)
{}

bool UserPollTask::BuildRequest(APDURequest& request, uint8_t seq)
{
	rxCount = 0;
	request.SetFunction(FunctionCode::READ);
	request.SetControl(AppControlField::Request(seq));
	auto writer = request.GetWriter();
	return builder(writer);
}

IMasterTask::TaskState UserPollTask::OnTaskComplete(TaskCompletion result, openpal::MonotonicTimestamp now)
{
	switch (result)
	{
	case(TaskCompletion::FAILURE_BAD_RESPONSE) :
		return TaskState::Disabled();
	case(TaskCompletion::FAILURE_NO_COMMS) :
		return TaskState::Immediately();
	default:
		return period.IsNegative() ? TaskState::Infinite() : TaskState::Retry(now.Add(period));
	}
}

} //end ns



