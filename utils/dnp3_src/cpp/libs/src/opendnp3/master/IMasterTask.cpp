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
#include "IMasterTask.h"

#include "openpal/logging/LogMacros.h"
#include "opendnp3/LogLevels.h"

using namespace openpal;

namespace opendnp3
{

IMasterTask::IMasterTask(IMasterApplication& app, openpal::MonotonicTimestamp expiration, openpal::Logger logger_, TaskConfig config_) :
	pApplication(&app),
	logger(logger_),
	state(expiration),
	config(config_),
	taskStartExpiration(MonotonicTimestamp::Max())
{}

IMasterTask::~IMasterTask()
{
	if (config.pCallback)
	{
		config.pCallback->OnDestroyed();
	}
}

openpal::MonotonicTimestamp IMasterTask::ExpirationTime() const
{
	return (!state.disabled && this->IsEnabled()) ? state.expiration : MonotonicTimestamp::Max();
}

void IMasterTask::ConfigureStartExpiration(openpal::MonotonicTimestamp time)
{
	this->taskStartExpiration = time;
}

openpal::MonotonicTimestamp IMasterTask::StartExpirationTime() const
{
	return taskStartExpiration;
}

IMasterTask::ResponseResult IMasterTask::OnResponse(const APDUResponseHeader& response, const openpal::RSlice& objects, openpal::MonotonicTimestamp now)
{
	auto result = ProcessResponse(response, objects);

	switch (result)
	{
	case(ResponseResult::ERROR_BAD_RESPONSE) :
		this->state = this->OnTaskComplete(TaskCompletion::FAILURE_BAD_RESPONSE, now);
		this->NotifyResult(TaskCompletion::FAILURE_BAD_RESPONSE);
		break;
	case(ResponseResult::ERROR_INTERNAL_FAILURE) :
		this->state = this->OnTaskComplete(TaskCompletion::FAILURE_INTERNAL_ERROR, now);
		this->NotifyResult(TaskCompletion::FAILURE_INTERNAL_ERROR);
		break;
	case(ResponseResult::OK_FINAL) :
		this->state = this->OnTaskComplete(TaskCompletion::SUCCESS, now);
		this->NotifyResult(TaskCompletion::SUCCESS);
		break;
	default:
		break;
	}

	return result;
}

void IMasterTask::OnResponseTimeout(openpal::MonotonicTimestamp now)
{
	this->state = this->OnTaskComplete(TaskCompletion::FAILURE_RESPONSE_TIMEOUT, now);
	this->NotifyResult(TaskCompletion::FAILURE_RESPONSE_TIMEOUT);
}

void IMasterTask::OnLowerLayerClose(openpal::MonotonicTimestamp now)
{
	this->state = this->OnTaskComplete(TaskCompletion::FAILURE_NO_COMMS, now);
	this->NotifyResult(TaskCompletion::FAILURE_NO_COMMS);
}

void IMasterTask::OnStartTimeout(openpal::MonotonicTimestamp now)
{
	this->state = this->OnTaskComplete(TaskCompletion::FAILURE_START_TIMEOUT, now);
	this->NotifyResult(TaskCompletion::FAILURE_START_TIMEOUT);
}

void IMasterTask::OnNoUser(openpal::MonotonicTimestamp now)
{
	this->state = this->OnTaskComplete(TaskCompletion::FAILURE_NO_USER, now);
	this->NotifyResult(TaskCompletion::FAILURE_NO_USER);
}

void IMasterTask::OnInternalError(openpal::MonotonicTimestamp now)
{
	this->state = this->OnTaskComplete(TaskCompletion::FAILURE_INTERNAL_ERROR, now);
	this->NotifyResult(TaskCompletion::FAILURE_INTERNAL_ERROR);
}

void IMasterTask::OnAuthenticationFailure(openpal::MonotonicTimestamp now)
{
	this->state = this->OnTaskComplete(TaskCompletion::FAILURE_BAD_AUTHENTICATION, now);
	this->NotifyResult(TaskCompletion::FAILURE_BAD_AUTHENTICATION);
}

void IMasterTask::OnAuthorizationFailure(openpal::MonotonicTimestamp now)
{
	this->state = this->OnTaskComplete(TaskCompletion::FAILURE_NOT_AUTHORIZED, now);
	this->NotifyResult(TaskCompletion::FAILURE_NOT_AUTHORIZED);
}

void IMasterTask::NotifyResult(TaskCompletion result)
{
	if (config.pCallback)
	{
		config.pCallback->OnComplete(result);
	}

	pApplication->OnTaskComplete(TaskInfo(this->GetTaskType(), result, config.taskId));
}

void IMasterTask::OnStart()
{
	if (config.pCallback)
	{
		config.pCallback->OnStart();
	}

	pApplication->OnTaskStart(this->GetTaskType(), config.taskId);

	this->Initialize();
}

void IMasterTask::Demand()
{
	this->state = TaskState::Immediately();
}

bool IMasterTask::ValidateSingleResponse(const APDUResponseHeader& header)
{
	if (header.control.FIR && header.control.FIN)
	{
		return true;
	}
	else
	{
		SIMPLE_LOG_BLOCK(logger, flags::WARN, "Ignoring unexpected response FIR/FIN not set");
		return false;
	}
}

bool IMasterTask::ValidateNullResponse(const APDUResponseHeader& header, const openpal::RSlice& objects)
{
	return ValidateSingleResponse(header) && ValidateNoObjects(objects) && ValidateInternalIndications(header);
}


bool IMasterTask::ValidateInternalIndications(const APDUResponseHeader& header)
{
	if (header.IIN.HasRequestError())
	{
		FORMAT_LOG_BLOCK(logger, flags::WARN, "Task was explicitly rejected via response with error IIN bit(s): %s", this->Name());
		return false;
	}
	else
	{
		return true;
	}
}

bool IMasterTask::ValidateNoObjects(const openpal::RSlice& objects)
{
	if (objects.IsEmpty())
	{
		return true;
	}
	else
	{
		FORMAT_LOG_BLOCK(logger, flags::WARN, "Received unexpected response object headers for task: %s", this->Name());
		return false;
	}
}

}

