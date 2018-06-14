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
#include "DNP3Channel.h"

#include <openpal/logging/LogMacros.h>
#include <opendnp3/LogLevels.h>

#include "MasterStack.h"
#include "OutstationStack.h"

using namespace openpal;
using namespace asiopal;
using namespace opendnp3;

namespace asiodnp3
{

DNP3Channel::DNP3Channel(
    const Logger& logger,
    const std::shared_ptr<asiopal::Executor>& executor,
    const std::shared_ptr<IOHandler>& iohandler,
    const std::shared_ptr<asiopal::IResourceManager>& manager) :

	logger(logger),
	executor(executor),
	iohandler(iohandler),
	manager(manager),
	resources(ResourceManager::Create())
{

}

DNP3Channel::~DNP3Channel()
{
	this->ShutdownImpl();
}

// comes from the outside, so we need to post
void DNP3Channel::Shutdown()
{
	auto shutdown = [self = shared_from_this()]()
	{
		self->ShutdownImpl();
	};

	this->executor->BlockUntilAndFlush(shutdown);
}

void DNP3Channel::ShutdownImpl()
{
	if (!this->resources) return;

	// shutdown the IO handler
	this->iohandler->Shutdown();
	this->iohandler.reset();

	// shutdown any remaining stacks
	this->resources->Shutdown();
	this->resources.reset();

	// posting ensures that we run this after
	// and callbacks created by calls above
	auto detach = [self = shared_from_this()]
	{
		self->manager->Detach(self);
		self->manager.reset();
	};

	this->executor->strand.post(detach);
}

LinkStatistics DNP3Channel::GetStatistics()
{
	auto get = [this]()
	{
		return this->iohandler->Statistics();
	};
	return this->executor->ReturnFrom<LinkStatistics>(get);
}

LogFilters DNP3Channel::GetLogFilters() const
{
	auto get = [this]()
	{
		return this->logger.GetFilters();
	};
	return this->executor->ReturnFrom<LogFilters>(get);
}

void DNP3Channel::SetLogFilters(const LogFilters& filters)
{
	auto set = [self = this->shared_from_this(), filters]()
	{
		self->logger.SetFilters(filters);
	};
	this->executor->strand.post(set);
}

std::shared_ptr<IMaster> DNP3Channel::AddMaster(const std::string& id, std::shared_ptr<ISOEHandler> SOEHandler, std::shared_ptr<IMasterApplication> application, const MasterStackConfig& config)
{
	auto stack = MasterStack::Create(this->logger.Detach(id), this->executor, SOEHandler, application, this->iohandler, this->resources, config, this->iohandler->TaskLock());

	return this->AddStack(config.link, stack);

}

std::shared_ptr<IOutstation> DNP3Channel::AddOutstation(const std::string& id, std::shared_ptr<ICommandHandler> commandHandler, std::shared_ptr<IOutstationApplication> application, const OutstationStackConfig& config)
{
	auto stack = OutstationStack::Create(this->logger.Detach(id), this->executor, commandHandler, application, this->iohandler, this->resources, config);

	return this->AddStack(config.link, stack);
}

template <class T>
std::shared_ptr<T> DNP3Channel::AddStack(const LinkConfig& link, const std::shared_ptr<T>& stack)
{

	auto create = [stack, route = Route(link.RemoteAddr, link.LocalAddr), self = this->shared_from_this()]()
	{

		auto add = [stack, route, self]() -> bool
		{
			return self->iohandler->AddContext(stack, route);
		};

		return self->executor->ReturnFrom<bool>(add) ? stack : nullptr;
	};

	return this->resources->Bind<T>(create);
}

}
