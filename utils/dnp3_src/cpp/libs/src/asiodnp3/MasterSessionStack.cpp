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

#include "asiodnp3/MasterSessionStack.h"

#include "asiopal/Executor.h"
#include "asiodnp3/Conversions.h"
#include "asiodnp3/LinkSession.h"

using namespace opendnp3;

namespace asiodnp3
{
std::shared_ptr<MasterSessionStack> MasterSessionStack::Create(
    const openpal::Logger& logger,
    const std::shared_ptr<asiopal::Executor>& executor,
    const std::shared_ptr<opendnp3::ISOEHandler>& SOEHandler,
    const std::shared_ptr<opendnp3::IMasterApplication>& application,
    const std::shared_ptr<LinkSession>& session,
    opendnp3::ILinkTx& linktx,
    const MasterStackConfig& config
)
{
	return std::make_shared<MasterSessionStack>(logger, executor, SOEHandler, application, session, linktx, config);
}

MasterSessionStack::MasterSessionStack(
    const openpal::Logger& logger,
    const std::shared_ptr<asiopal::Executor>& executor,
    const std::shared_ptr<opendnp3::ISOEHandler>& SOEHandler,
    const std::shared_ptr<opendnp3::IMasterApplication>& application,
    const std::shared_ptr<LinkSession>& session,
    opendnp3::ILinkTx& linktx,
    const MasterStackConfig& config
) :
	executor(executor),
	session(session),
	stack(logger, executor, application, config.master.maxRxFragSize, config.link),
	context(logger, executor, stack.transport, SOEHandler, application, config.master, NullTaskLock::Instance())
{
	stack.link->SetRouter(linktx);
	stack.transport->SetAppLayer(context);
}

void MasterSessionStack::OnLowerLayerUp()
{
	stack.link->OnLowerLayerUp();
}

void MasterSessionStack::OnLowerLayerDown()
{
	stack.link->OnLowerLayerDown();

	// now we can release the socket session
	session.reset();
}

bool MasterSessionStack::OnFrame(const LinkHeaderFields& header, const openpal::RSlice& userdata)
{
	return stack.link->OnFrame(header, userdata);
}

void MasterSessionStack::OnTransmitComplete(bool success)
{
	this->stack.link->OnTransmitResult(success);
}

void MasterSessionStack::SetLogFilters(const openpal::LogFilters& filters)
{
	auto set = [this, filters]()
	{
		this->session->SetLogFilters(filters);
	};

	this->executor->strand.post(set);
}

void MasterSessionStack::Demand(const std::shared_ptr<opendnp3::IMasterTask>& task)
{
	auto action = [task, self = shared_from_this()]
	{
		task->Demand();
		self->context.CheckForTask();
	};
	this->executor->strand.post(action);
}

void MasterSessionStack::BeginShutdown()
{
	auto shutdown = [session = session]()
	{
		session->Shutdown();
	};

	executor->strand.post(shutdown);
}

StackStatistics MasterSessionStack::GetStackStatistics()
{
	auto get = [self = shared_from_this()]() -> StackStatistics
	{
		return self->CreateStatistics();
	};
	return executor->ReturnFrom<StackStatistics>(get);
}

std::shared_ptr<IMasterScan> MasterSessionStack::AddScan(openpal::TimeDuration period, const std::vector<Header>& headers, const TaskConfig& config)
{
	auto builder = ConvertToLambda(headers);
	auto get = [self = shared_from_this(), period, builder, config]()
	{
		return self->context.AddScan(period, builder, config);
	};
	return MasterScan::Create(executor->ReturnFrom<std::shared_ptr<IMasterTask>>(get), shared_from_this());
}

std::shared_ptr<IMasterScan> MasterSessionStack::AddAllObjectsScan(GroupVariationID gvId, openpal::TimeDuration period, const TaskConfig& config)
{
	auto get = [self = shared_from_this(), gvId, period, config] { return self->context.AddAllObjectsScan(gvId, period, config); };
	return MasterScan::Create(executor->ReturnFrom<std::shared_ptr<IMasterTask>>(get), shared_from_this());
}

std::shared_ptr<IMasterScan> MasterSessionStack::AddClassScan(const ClassField& field, openpal::TimeDuration period, const TaskConfig& config)
{
	auto get = [self = shared_from_this(), field, period, config] { return self->context.AddClassScan(field, period, config); };
	return MasterScan::Create(executor->ReturnFrom<std::shared_ptr<IMasterTask>>(get), shared_from_this());
}

std::shared_ptr<IMasterScan> MasterSessionStack::AddRangeScan(GroupVariationID gvId, uint16_t start, uint16_t stop, openpal::TimeDuration period, const TaskConfig& config)
{
	auto get = [self = shared_from_this(), gvId, start, stop, period, config] { return self->context.AddRangeScan(gvId, start, stop, period, config); };
	return MasterScan::Create(executor->ReturnFrom<std::shared_ptr<IMasterTask>>(get), shared_from_this());
}

void MasterSessionStack::Scan(const std::vector<Header>& headers, const TaskConfig& config)
{
	auto builder = ConvertToLambda(headers);
	auto action = [self = shared_from_this(), builder, config]() -> void { self->context.Scan(builder, config); };
	return executor->strand.post(action);
}

void MasterSessionStack::ScanAllObjects(GroupVariationID gvId, const TaskConfig& config)
{
	auto action = [self = shared_from_this(), gvId, config]() -> void { self->context.ScanAllObjects(gvId, config); };
	return executor->strand.post(action);
}

void MasterSessionStack::ScanClasses(const ClassField& field, const TaskConfig& config)
{
	auto action = [self = shared_from_this(), field, config]() -> void { self->context.ScanClasses(field, config); };
	return executor->strand.post(action);
}

void MasterSessionStack::ScanRange(GroupVariationID gvId, uint16_t start, uint16_t stop, const TaskConfig& config)
{
	auto action = [self = shared_from_this(), gvId, start, stop, config]() -> void { self->context.ScanRange(gvId, start, stop, config); };
	return executor->strand.post(action);
}

void MasterSessionStack::Write(const TimeAndInterval& value, uint16_t index, const TaskConfig& config)
{
	auto action = [self = shared_from_this(), value, index, config]() -> void { self->context.Write(value, index, config); };
	return executor->strand.post(action);
}

void MasterSessionStack::Restart(RestartType op, const RestartOperationCallbackT& callback, TaskConfig config)
{
	auto action = [self = shared_from_this(), op, callback, config]() -> void { self->context.Restart(op, callback, config); };
	return executor->strand.post(action);
}

void MasterSessionStack::PerformFunction(const std::string& name, FunctionCode func, const std::vector<Header>& headers, const TaskConfig& config)
{
	auto builder = ConvertToLambda(headers);
	auto action = [self = shared_from_this(), name, func, builder, config]() -> void { self->context.PerformFunction(name, func, builder, config); };
	return executor->strand.post(action);
}

/// --- ICommandProcessor ---

void MasterSessionStack::SelectAndOperate(CommandSet&& commands, const CommandCallbackT& callback, const TaskConfig& config)
{
	// this is required b/c move capture not supported in C++11
	auto set = std::make_shared<CommandSet>(std::move(commands));

	auto action = [self = shared_from_this(), set, config, callback]() -> void
	{
		self->context.SelectAndOperate(std::move(*set), callback, config);
	};
	executor->strand.post(action);
}

void MasterSessionStack::DirectOperate(CommandSet&& commands, const CommandCallbackT& callback, const TaskConfig& config)
{
	// this is required b/c move capture not supported in C++11
	auto set = std::make_shared<CommandSet>(std::move(commands));

	auto action = [self = shared_from_this(), set, config, callback]() -> void
	{
		self->context.DirectOperate(std::move(*set), callback, config);
	};
	executor->strand.post(action);
}

opendnp3::StackStatistics MasterSessionStack::CreateStatistics() const
{
	return opendnp3::StackStatistics(this->stack.link->GetStatistics(), this->stack.transport->GetStatistics());
}

}



