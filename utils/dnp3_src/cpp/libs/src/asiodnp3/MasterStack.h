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
#ifndef ASIODNP3_MASTERSTACK_H
#define ASIODNP3_MASTERSTACK_H

#include "asiodnp3/IMaster.h"
#include "asiodnp3/MasterStackConfig.h"
#include "asiodnp3/StackBase.h"
#include "asiodnp3/MasterScan.h"

#include "opendnp3/master/MasterContext.h"

namespace asiodnp3
{


class MasterStack : public IMaster, public opendnp3::ILinkSession, public opendnp3::ILinkTx, public ITaskActions, public std::enable_shared_from_this<MasterStack>, public StackBase
{
public:

	MasterStack(
	    const openpal::Logger& logger,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    const std::shared_ptr<opendnp3::ISOEHandler>& SOEHandler,
	    const std::shared_ptr<opendnp3::IMasterApplication>& application,
	    const std::shared_ptr<IOHandler>& iohandler,
	    const std::shared_ptr<asiopal::IResourceManager>& manager,
	    const MasterStackConfig& config,
	    opendnp3::ITaskLock& taskLock
	);

	static std::shared_ptr<MasterStack> Create(
	    const openpal::Logger& logger,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    const std::shared_ptr<opendnp3::ISOEHandler>& SOEHandler,
	    const std::shared_ptr<opendnp3::IMasterApplication>& application,
	    const std::shared_ptr<IOHandler>& iohandler,
	    const std::shared_ptr<asiopal::IResourceManager>& manager,
	    const MasterStackConfig& config,
	    opendnp3::ITaskLock& taskLock
	)
	{
		auto ret = std::make_shared<MasterStack>(logger, executor, SOEHandler, application, iohandler, manager, config, taskLock);

		ret->tstack.link->SetRouter(*ret);

		return ret;
	}

	// --------- Implement IStack ---------

	virtual bool Enable() override;

	virtual bool Disable() override;

	virtual void Shutdown() override;

	virtual opendnp3::StackStatistics GetStackStatistics() override;

	virtual void Demand(const std::shared_ptr<opendnp3::IMasterTask>& task) override;

	// --------- Implement ILinkSession ---------

	virtual bool OnTransmitResult(bool success) override
	{
		return this->tstack.link->OnTransmitResult(success);
	}

	virtual bool OnLowerLayerUp() override
	{
		return this->tstack.link->OnLowerLayerUp();
	}

	virtual bool OnLowerLayerDown() override
	{
		return this->tstack.link->OnLowerLayerDown();
	}

	virtual bool OnFrame(const opendnp3::LinkHeaderFields& header, const openpal::RSlice& userdata)
	{
		return this->tstack.link->OnFrame(header, userdata);
	}

	virtual void BeginTransmit(const openpal::RSlice& buffer, opendnp3::ILinkSession& context)
	{
		this->iohandler->BeginTransmit(shared_from_this(), buffer);
	}

	// --------- Implement IMasterOperations ---------

	virtual void SetLogFilters(const openpal::LogFilters& filters) override;

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

	// ------- implement ICommandProcessor ---------

	virtual void SelectAndOperate(opendnp3::CommandSet&& commands, const opendnp3::CommandCallbackT& callback, const opendnp3::TaskConfig& config) override;

	virtual void DirectOperate(opendnp3::CommandSet&& commands, const opendnp3::CommandCallbackT& callback, const opendnp3::TaskConfig& config) override;

protected:

	opendnp3::MContext mcontext;
};

}

#endif

