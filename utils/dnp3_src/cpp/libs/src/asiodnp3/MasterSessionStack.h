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
#ifndef ASIODNP3_MASTERSESSIONSTACK_H
#define ASIODNP3_MASTERSESSIONSTACK_H

#include <opendnp3/master/MasterContext.h>
#include <opendnp3/transport/TransportStack.h>

#include "asiodnp3/IMasterSession.h"
#include "asiodnp3/MasterStackConfig.h"
#include "asiodnp3/MasterScan.h"

namespace asiopal
{
class Executor;
}

namespace asiodnp3
{

class LinkSession;

/**
* Interface that represents an ephemeral master session
*/
class MasterSessionStack final : public IMasterSession, public ITaskActions, public std::enable_shared_from_this<MasterSessionStack>
{
public:

	static std::shared_ptr<MasterSessionStack> Create(
	    const openpal::Logger& logger,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    const std::shared_ptr<opendnp3::ISOEHandler>& SOEHandler,
	    const std::shared_ptr<opendnp3::IMasterApplication>& application,
	    const std::shared_ptr<LinkSession>& session,
	    opendnp3::ILinkTx& linktx,
	    const MasterStackConfig& config
	);

	void OnLowerLayerUp();

	void OnLowerLayerDown();

	bool OnFrame(const opendnp3::LinkHeaderFields& header, const openpal::RSlice& userdata);

	void OnTransmitComplete(bool success);

	virtual void SetLogFilters(const openpal::LogFilters& filters) override;

	virtual void Demand(const std::shared_ptr<opendnp3::IMasterTask>& task) override;

	/// --- IGPRSMaster ---

	virtual void BeginShutdown() override;

	/// --- ICommandOperations ---

	virtual opendnp3::StackStatistics GetStackStatistics() override;
	virtual std::shared_ptr<IMasterScan> AddScan(openpal::TimeDuration period, const std::vector<opendnp3::Header>& headers, const opendnp3::TaskConfig& config) override;
	virtual std::shared_ptr<IMasterScan> AddAllObjectsScan(opendnp3::GroupVariationID gvId, openpal::TimeDuration period, const opendnp3::TaskConfig& config) override;
	virtual std::shared_ptr<IMasterScan> AddClassScan(const opendnp3::ClassField& field, openpal::TimeDuration period, const opendnp3::TaskConfig& config) override;
	virtual std::shared_ptr<IMasterScan> AddRangeScan(opendnp3::GroupVariationID gvId, uint16_t start, uint16_t stop, openpal::TimeDuration period, const opendnp3::TaskConfig& config) override;
	virtual void Scan(const std::vector<opendnp3::Header>& headers, const opendnp3::TaskConfig& config) override;
	virtual void ScanAllObjects(opendnp3::GroupVariationID gvId, const opendnp3::TaskConfig& config) override;
	virtual void ScanClasses(const opendnp3::ClassField& field, const opendnp3::TaskConfig& config) override;
	virtual void ScanRange(opendnp3::GroupVariationID gvId, uint16_t start, uint16_t stop, const opendnp3::TaskConfig& config) override;
	virtual void Write(const opendnp3::TimeAndInterval& value, uint16_t index, const opendnp3::TaskConfig& config) override;
	virtual void Restart(opendnp3::RestartType op, const opendnp3::RestartOperationCallbackT& callback, opendnp3::TaskConfig config) override;
	virtual void PerformFunction(const std::string& name, opendnp3::FunctionCode func, const std::vector<opendnp3::Header>& headers, const opendnp3::TaskConfig& config) override;

	/// --- ICommandProcessor ---

	virtual void SelectAndOperate(opendnp3::CommandSet&& commands, const opendnp3::CommandCallbackT& callback, const opendnp3::TaskConfig& config) override;
	virtual void DirectOperate(opendnp3::CommandSet&& commands, const opendnp3::CommandCallbackT& callback, const opendnp3::TaskConfig& config) override;

	MasterSessionStack(
	    const openpal::Logger& logger,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    const std::shared_ptr<opendnp3::ISOEHandler>& SOEHandler,
	    const std::shared_ptr<opendnp3::IMasterApplication>& application,
	    const std::shared_ptr<LinkSession>& session,
	    opendnp3::ILinkTx& linktx,
	    const MasterStackConfig& config
	);

private:

	opendnp3::StackStatistics CreateStatistics() const;

	std::shared_ptr<asiopal::Executor> executor;
	std::shared_ptr<LinkSession> session;
	opendnp3::TransportStack stack;
	opendnp3::MContext context;
};

}

#endif

