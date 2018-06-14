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
#include "MasterStack.h"

#include <opendnp3/app/APDUBuilders.h>

#include <opendnp3/master/MasterContext.h>

#include "Conversions.h"

using namespace openpal;
using namespace asiopal;
using namespace opendnp3;

namespace asiodnp3
{

MasterStack::MasterStack(
    const Logger& logger,
    const std::shared_ptr<Executor>& executor,
    const std::shared_ptr<ISOEHandler>& SOEHandler,
    const std::shared_ptr<IMasterApplication>& application,
    const std::shared_ptr<IOHandler>& iohandler,
    const std::shared_ptr<asiopal::IResourceManager>& manager,
    const MasterStackConfig& config,
    ITaskLock& taskLock) :

	StackBase(logger, executor, application, iohandler, manager, config.master.maxRxFragSize, config.link),
	mcontext(logger, executor, tstack.transport, SOEHandler, application,  config.master, taskLock)
{
	tstack.transport->SetAppLayer(mcontext);
}

bool MasterStack::Enable()
{
	auto action = [self = shared_from_this()] { return self->iohandler->Enable(self); };
	return this->executor->ReturnFrom<bool>(action);
}

bool MasterStack::Disable()
{
	auto action = [self = shared_from_this()] { return self->iohandler->Disable(self); };
	return this->executor->ReturnFrom<bool>(action);
}

void MasterStack::Shutdown()
{
	this->PerformShutdown(shared_from_this());
}

StackStatistics MasterStack::GetStackStatistics()
{
	auto get = [self = shared_from_this()]() -> StackStatistics
	{
		return self->CreateStatistics();
	};
	return this->executor->ReturnFrom<StackStatistics>(get);
}

void MasterStack::Demand(const std::shared_ptr<opendnp3::IMasterTask>& task)
{
	auto action = [task, self = shared_from_this()]
	{
		task->Demand();
		self->mcontext.CheckForTask();
	};
	this->executor->strand.post(action);
}

void MasterStack::SetLogFilters(const openpal::LogFilters& filters)
{
	auto set = [self = this->shared_from_this(), filters]()
	{
		self->logger.SetFilters(filters);
	};

	this->executor->strand.post(set);
}

std::shared_ptr<IMasterScan> MasterStack::AddScan(openpal::TimeDuration period, const std::vector<Header>& headers, const TaskConfig& config)
{
	auto builder = ConvertToLambda(headers);
	auto self = shared_from_this();
	auto add = [self, builder, period, config]()
	{
		return self->mcontext.AddScan(period, builder, config);
	};
	return MasterScan::Create(executor->ReturnFrom<std::shared_ptr<IMasterTask>>(add), self);
}

std::shared_ptr<IMasterScan> MasterStack::AddAllObjectsScan(GroupVariationID gvId, openpal::TimeDuration period, const TaskConfig& config)
{
	auto self = shared_from_this();
	auto add = [self, gvId, period, config]()
	{
		return self->mcontext.AddAllObjectsScan(gvId, period, config);
	};
	return MasterScan::Create(executor->ReturnFrom<std::shared_ptr<IMasterTask>>(add), self);
}

std::shared_ptr<IMasterScan> MasterStack::AddClassScan(const ClassField& field, openpal::TimeDuration period, const TaskConfig& config)
{
	auto self = shared_from_this();
	auto add = [self, field, period, config]()
	{
		return self->mcontext.AddClassScan(field, period, config);
	};
	return MasterScan::Create(executor->ReturnFrom<std::shared_ptr<IMasterTask>>(add), self);
}

std::shared_ptr<IMasterScan> MasterStack::AddRangeScan(GroupVariationID gvId, uint16_t start, uint16_t stop, openpal::TimeDuration period, const TaskConfig& config)
{
	auto add = [self = this->shared_from_this(), gvId, start, stop, period, config]()
	{
		return self->mcontext.AddRangeScan(gvId, start, stop, period, config);
	};
	return MasterScan::Create(executor->ReturnFrom<std::shared_ptr<IMasterTask>>(add), shared_from_this());
}

void MasterStack::Scan(const std::vector<Header>& headers, const TaskConfig& config)
{
	auto add = [self = this->shared_from_this(), builder = ConvertToLambda(headers), config]()
	{
		return self->mcontext.Scan(builder, config);
	};
	return this->executor->strand.post(add);
}

void MasterStack::ScanAllObjects(GroupVariationID gvId, const TaskConfig& config)
{
	auto add = [self = this->shared_from_this(), gvId, config]()
	{
		return self->mcontext.ScanAllObjects(gvId, config);
	};
	return this->executor->strand.post(add);
}

void MasterStack::ScanClasses(const ClassField& field, const TaskConfig& config)
{
	auto add = [self = this->shared_from_this(), field, config]()
	{
		return self->mcontext.ScanClasses(field, config);
	};
	return this->executor->strand.post(add);
}

void MasterStack::ScanRange(GroupVariationID gvId, uint16_t start, uint16_t stop, const TaskConfig& config)
{
	auto add = [self = this->shared_from_this(), gvId, start, stop, config]()
	{
		return self->mcontext.ScanRange(gvId, start, stop, config);
	};
	return this->executor->strand.post(add);
}

void MasterStack::Write(const TimeAndInterval& value, uint16_t index, const TaskConfig& config)
{
	auto add = [self = this->shared_from_this(), value, index, config]()
	{
		return self->mcontext.Write(value, index, config);
	};
	return this->executor->strand.post(add);
}

void MasterStack::Restart(RestartType op, const RestartOperationCallbackT& callback, TaskConfig config)
{
	auto add = [self = this->shared_from_this(), op, callback, config]()
	{
		return self->mcontext.Restart(op, callback, config);
	};
	return this->executor->strand.post(add);
}

void MasterStack::PerformFunction(const std::string& name, FunctionCode func, const std::vector<Header>& headers, const TaskConfig& config)
{
	auto add = [self = this->shared_from_this(), name, func, builder = ConvertToLambda(headers), config]()
	{
		return self->mcontext.PerformFunction(name, func, builder, config);
	};
	return this->executor->strand.post(add);
}

void MasterStack::SelectAndOperate(opendnp3::CommandSet&& commands, const opendnp3::CommandCallbackT& callback, const opendnp3::TaskConfig& config)
{
	/// this is to work around the fact that c++11 doesn't have generic move capture
	auto set = std::make_shared<opendnp3::CommandSet>(std::move(commands));

	auto action = [self = this->shared_from_this(), set, config, callback]()
	{
		self->mcontext.SelectAndOperate(std::move(*set), callback, config);
	};

	this->executor->strand.post(action);
}

void MasterStack::DirectOperate(opendnp3::CommandSet&& commands, const opendnp3::CommandCallbackT& callback, const opendnp3::TaskConfig& config)
{
	/// this is to work around the fact that c++11 doesn't have generic move capture
	auto set = std::make_shared<opendnp3::CommandSet>(std::move(commands));

	auto action = [self = this->shared_from_this(), set, config, callback]()
	{
		self->mcontext.DirectOperate(std::move(*set), callback, config);
	};

	this->executor->strand.post(action);
}

}
