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
#ifndef ASIODNP3_DNP3CHANNEL_H
#define ASIODNP3_DNP3CHANNEL_H


#include "asiodnp3/IChannel.h"
#include "asiodnp3/IOHandler.h"
#include "asiopal/ResourceManager.h"
#include "opendnp3/master/MultidropTaskLock.h"

namespace asiodnp3
{

class DNP3Channel final : public IChannel, public std::enable_shared_from_this<DNP3Channel>
{

public:

	DNP3Channel(
	    const openpal::Logger& logger,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    const std::shared_ptr<IOHandler>& iohandler,
	    const std::shared_ptr<asiopal::IResourceManager>& manager
	);

	static std::shared_ptr<DNP3Channel> Create(
	    const openpal::Logger& logger,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    const std::shared_ptr<IOHandler>& iohandler,
	    const std::shared_ptr<asiopal::IResourceManager>& manager)
	{
		return std::make_shared<DNP3Channel>(logger, executor, iohandler, manager);
	}

	~DNP3Channel();

	// ----------------------- Implement IChannel -----------------------

	void Shutdown() override;

	virtual opendnp3::LinkStatistics GetStatistics() override;

	virtual openpal::LogFilters GetLogFilters() const override;

	virtual void SetLogFilters(const openpal::LogFilters& filters) override;

	virtual std::shared_ptr<IMaster> AddMaster(const std::string& id,
	        std::shared_ptr<opendnp3::ISOEHandler> SOEHandler,
	        std::shared_ptr<opendnp3::IMasterApplication> application,
	        const MasterStackConfig& config) override;



	virtual std::shared_ptr<IOutstation> AddOutstation(const std::string& id,
	        std::shared_ptr<opendnp3::ICommandHandler> commandHandler,
	        std::shared_ptr<opendnp3::IOutstationApplication> application,
	        const OutstationStackConfig& config) override;

private:

	void ShutdownImpl();

	// ----- generic method for adding a stack ------
	template <class T>
	std::shared_ptr<T> AddStack(const opendnp3::LinkConfig& link, const std::shared_ptr<T>& stack);

	openpal::Logger logger;
	const std::shared_ptr<asiopal::Executor> executor;

	std::shared_ptr<IOHandler> iohandler;
	std::shared_ptr<asiopal::IResourceManager> manager;
	std::shared_ptr<asiopal::ResourceManager> resources;

};

}

#endif
