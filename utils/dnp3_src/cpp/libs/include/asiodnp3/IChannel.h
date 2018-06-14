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
#ifndef ASIODNP3_ICHANNEL_H
#define ASIODNP3_ICHANNEL_H

#include <opendnp3/gen/ChannelState.h>
#include <opendnp3/link/LinkStatistics.h>

#include <opendnp3/master/ISOEHandler.h>
#include <opendnp3/master/IMasterApplication.h>

#include <opendnp3/outstation/ICommandHandler.h>
#include <opendnp3/outstation/IOutstationApplication.h>

#include <openpal/logging/LogFilters.h>
#include <openpal/executor/IExecutor.h>

#include "asiopal/IResourceManager.h"

#include "IMaster.h"
#include "IOutstation.h"
#include "MasterStackConfig.h"
#include "OutstationStackConfig.h"

#include <memory>

namespace asiodnp3
{

/**
* Represents a communication channel upon which masters and outstations can be bound.
*/
class IChannel : public asiopal::IResource
{
public:

	virtual ~IChannel() {}

	/**
	* Synchronously read the channel statistics
	*/
	virtual opendnp3::LinkStatistics GetStatistics() = 0;

	/**
	*  @return The current logger settings for this channel
	*/
	virtual openpal::LogFilters GetLogFilters() const = 0;

	/**
	*  @param filters Adjust the filters to this value
	*/
	virtual void SetLogFilters(const openpal::LogFilters& filters) = 0;

	/**
	* Add a master to the channel
	*
	* @param id An ID that gets used for logging
	* @param SOEHandler Callback object for all received measurements
	* @param application The master application bound to the master session
	* @param config Configuration object that controls how the master behaves
	*
	* @return shared_ptr to the running master
	*/
	virtual std::shared_ptr<IMaster>  AddMaster(const std::string& id,
	        std::shared_ptr<opendnp3::ISOEHandler> SOEHandler,
	        std::shared_ptr<opendnp3::IMasterApplication> application,
	        const MasterStackConfig& config) = 0;

	/**
	* Add an outstation to the channel
	*
	* @param id An ID that gets used for logging
	* @param commandHandler Callback object for handling command requests
	* @param application Callback object for user code
	* @param config Configuration object that controls how the outstation behaves
	* @return shared_ptr to the running outstation
	*/
	virtual std::shared_ptr<IOutstation>  AddOutstation( const std::string& id,
	        std::shared_ptr<opendnp3::ICommandHandler> commandHandler,
	        std::shared_ptr<opendnp3::IOutstationApplication> application,
	        const OutstationStackConfig& config) = 0;

};

}

#endif
