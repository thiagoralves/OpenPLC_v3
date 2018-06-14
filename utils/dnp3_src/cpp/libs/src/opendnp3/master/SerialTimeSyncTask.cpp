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
#include "SerialTimeSyncTask.h"

#include "opendnp3/objects/Group50.h"
#include "opendnp3/app/parsing/APDUParser.h"
#include "opendnp3/app/APDUBuilders.h"
#include "opendnp3/master/TimeSyncHandler.h"

#include <openpal/serialization/Serialization.h>

using namespace openpal;

namespace opendnp3
{

SerialTimeSyncTask::SerialTimeSyncTask(IMasterApplication& app, openpal::Logger logger) :
	IMasterTask(app, MonotonicTimestamp::Max(), logger, TaskConfig::Default()),
	delay(-1)
{}

void SerialTimeSyncTask::Initialize()
{
	delay = -1;
}

bool SerialTimeSyncTask::BuildRequest(APDURequest& request, uint8_t seq)
{
	if (delay < 0)
	{
		start = pApplication->Now();
		build::MeasureDelay(request, seq);
	}
	else
	{
		auto now = pApplication->Now();
		Group50Var1 time;
		time.time = DNPTime(now.msSinceEpoch + delay);
		request.SetFunction(FunctionCode::WRITE);
		request.SetControl(AppControlField::Request(seq));
		auto writer = request.GetWriter();
		writer.WriteSingleValue<UInt8, Group50Var1>(QualifierCode::UINT8_CNT, time);
	}

	return true;
}

IMasterTask::TaskState SerialTimeSyncTask::OnTaskComplete(TaskCompletion result, openpal::MonotonicTimestamp now)
{
	switch (result)
	{
	case(TaskCompletion::FAILURE_BAD_RESPONSE) :
		return TaskState::Disabled();
	default:
		return TaskState::Infinite();
	}
}

IMasterTask::ResponseResult SerialTimeSyncTask::ProcessResponse(const APDUResponseHeader& response, const openpal::RSlice& objects)
{
	return (delay < 0) ? OnResponseDelayMeas(response, objects) : OnResponseWriteTime(response, objects);
}

IMasterTask::ResponseResult SerialTimeSyncTask::OnResponseDelayMeas(const APDUResponseHeader& response, const openpal::RSlice& objects)
{
	if (ValidateSingleResponse(response))
	{
		TimeSyncHandler handler;
		auto result = APDUParser::Parse(objects, handler, &logger);
		if (result == ParseResult::OK)
		{
			uint16_t rtuTurnAroundTime;
			if (handler.GetTimeDelay(rtuTurnAroundTime))
			{
				auto now = pApplication->Now();
				auto sendReceieveTime = now.msSinceEpoch - start.msSinceEpoch;

				// The later shouldn't happen, but could cause a negative delay which would result in a weird time setting
				delay = (sendReceieveTime >= rtuTurnAroundTime) ? (sendReceieveTime - rtuTurnAroundTime) / 2 : 0;

				return ResponseResult::OK_REPEAT;
			}
			else
			{
				return ResponseResult::ERROR_BAD_RESPONSE;
			}
		}
		else
		{
			return ResponseResult::ERROR_BAD_RESPONSE;
		}
	}
	else
	{
		return ResponseResult::ERROR_BAD_RESPONSE;
	}
}

IMasterTask::ResponseResult SerialTimeSyncTask::OnResponseWriteTime(const APDUResponseHeader& header, const openpal::RSlice& objects)
{
	return ValidateNullResponse(header, objects) ? ResponseResult::OK_FINAL : ResponseResult::ERROR_BAD_RESPONSE;
}

} //ens ns



