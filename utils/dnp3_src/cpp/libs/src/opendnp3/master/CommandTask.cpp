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
#include "CommandTask.h"

#include "opendnp3/master/CommandSetOps.h"
#include "opendnp3/master/ICommandHeader.h"

#include <openpal/logging/LogMacros.h>

#include "opendnp3/app/parsing/APDUParser.h"
#include "opendnp3/LogLevels.h"

using namespace openpal;

namespace opendnp3
{

std::shared_ptr<IMasterTask> CommandTask::CreateDirectOperate(CommandSet&& set, IMasterApplication& app, const CommandCallbackT& callback, const TaskConfig& config, openpal::Logger logger)
{
	auto task = std::make_shared<CommandTask>(std::move(set), app, callback, config, logger);
	task->LoadDirectOperate();
	return task;
}


std::shared_ptr<IMasterTask> CommandTask::CreateSelectAndOperate(CommandSet&& set, IMasterApplication& app, const CommandCallbackT& callback, const TaskConfig& config, openpal::Logger logger)
{
	auto task = std::make_shared<CommandTask>(std::move(set), app, callback, config, logger);
	task->LoadSelectAndOperate();
	return task;
}

CommandTask::CommandTask(CommandSet&& commands, IMasterApplication& app, const CommandCallbackT& callback, const TaskConfig& config, openpal::Logger logger) :
	IMasterTask(app, MonotonicTimestamp::Min(), logger, config),
	statusResult(CommandStatus::UNDEFINED),
	commandCallback(callback),
	commands(std::move(commands))
{

}

void CommandTask::LoadSelectAndOperate()
{
	functionCodes.clear();
	functionCodes.push_back(FunctionCode::SELECT);
	functionCodes.push_back(FunctionCode::OPERATE);
}

void CommandTask::LoadDirectOperate()
{
	functionCodes.clear();
	functionCodes.push_back(FunctionCode::DIRECT_OPERATE);
}

bool CommandTask::BuildRequest(APDURequest& request, uint8_t seq)
{
	if (!functionCodes.empty())
	{
		request.SetFunction(functionCodes.front());
		functionCodes.pop_front();
		request.SetControl(AppControlField::Request(seq));
		auto writer = request.GetWriter();
		return CommandSetOps::Write(commands, writer);
	}

	return false;
}

IMasterTask::ResponseResult CommandTask::ProcessResponse(const APDUResponseHeader& header, const openpal::RSlice& objects)
{
	return ValidateSingleResponse(header) ? ProcessResponse(objects) : ResponseResult::ERROR_BAD_RESPONSE;
}

IMasterTask::TaskState CommandTask::OnTaskComplete(TaskCompletion result, openpal::MonotonicTimestamp now)
{
	CommandSetOps::InvokeCallback(commands, result, this->commandCallback);
	return TaskState::Infinite();
}

void CommandTask::Initialize()
{
	statusResult = CommandStatus::UNDEFINED;
}

IMasterTask::ResponseResult CommandTask::ProcessResponse(const openpal::RSlice& objects)
{
	if (functionCodes.empty())
	{
		auto result = CommandSetOps::ProcessOperateResponse(commands, objects, &logger);
		return (result == CommandSetOps::OperateResult::FAIL_PARSE) ? ResponseResult::ERROR_BAD_RESPONSE : ResponseResult::OK_FINAL;
	}
	else
	{
		auto result = CommandSetOps::ProcessSelectResponse(commands, objects, &logger);

		switch (result)
		{
		case(CommandSetOps::SelectResult::OK) :
			return ResponseResult::OK_REPEAT; // proceed to OPERATE
		case(CommandSetOps::SelectResult::FAIL_SELECT) :
			return ResponseResult::OK_FINAL; // end the task, let the user examine each command point
		default:
			return ResponseResult::ERROR_BAD_RESPONSE;
		}
	}
}

} //ens ns
