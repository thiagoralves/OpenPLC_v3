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
#ifndef OPENDNP3_MASTERCONTEXT_H
#define OPENDNP3_MASTERCONTEXT_H

#include <openpal/executor/IExecutor.h>
#include <openpal/logging/Logger.h>
#include <openpal/container/Buffer.h>
#include <openpal/executor/TimerRef.h>

#include "opendnp3/LayerInterfaces.h"

#include "opendnp3/app/AppSeqNum.h"
#include "opendnp3/app/MeasurementTypes.h"

#include "opendnp3/gen/RestartType.h"

#include "opendnp3/master/MasterScheduler.h"
#include "opendnp3/master/ITaskFilter.h"
#include "opendnp3/master/MasterTasks.h"
#include "opendnp3/master/ITaskLock.h"
#include "opendnp3/master/IMasterApplication.h"
#include "opendnp3/master/HeaderBuilder.h"
#include "opendnp3/master/RestartOperationResult.h"

namespace opendnp3
{
/*
	All of the mutable state and configuration for a master
*/
class MContext : public IUpperLayer, private IScheduleCallback, private ITaskFilter, private openpal::Uncopyable
{

protected:

	enum class TaskState
	{
		IDLE,
		TASK_READY,
		WAIT_FOR_RESPONSE
	};

public:

	MContext(
	    const openpal::Logger& logger,
	    const std::shared_ptr<openpal::IExecutor>& executor,
	    const std::shared_ptr<ILowerLayer>& lower,
	    const std::shared_ptr<ISOEHandler>& SOEHandler,
	    const std::shared_ptr<IMasterApplication>& application,
	    const MasterParams& params,
	    ITaskLock& taskLock
	);

	openpal::Logger logger;
	const std::shared_ptr<openpal::IExecutor> executor;
	const std::shared_ptr<ILowerLayer> lower;

	// ------- configuration --------
	MasterParams params;
	const std::shared_ptr<ISOEHandler> SOEHandler;
	const std::shared_ptr<IMasterApplication> application;
	ITaskLock* pTaskLock;


	// ------- dynamic state ---------
	bool isOnline = false;
	bool isSending = false;
	AppSeqNum solSeq;
	AppSeqNum unsolSeq;
	std::shared_ptr<IMasterTask> activeTask;
	openpal::TimerRef responseTimer;
	openpal::TimerRef scheduleTimer;
	openpal::TimerRef taskStartTimeoutTimer;
	MasterTasks tasks;
	MasterScheduler scheduler;
	std::deque<APDUHeader> confirmQueue;
	openpal::Buffer txBuffer;
	TaskState tstate;

	/// --- implement  IUpperLayer ------

	virtual bool OnLowerLayerUp() override;

	virtual bool OnLowerLayerDown() override;

	virtual bool OnReceive(const openpal::RSlice& apdu) override final;

	virtual bool OnSendResult(bool isSucccess) override final;

	/// additional virtual methods that can be overriden to implement secure authentication

	virtual void OnParsedHeader(const openpal::RSlice& apdu, const APDUResponseHeader& header, const openpal::RSlice& objects);

	virtual void RecordLastRequest(const openpal::RSlice& apdu) {}

	virtual bool MeetsUserRequirements(const std::shared_ptr<IMasterTask>& task)
	{
		return true;
	}

	/// ITaskFilter

	virtual bool CanRun(const IMasterTask& task) override
	{
		return true;
	}

	virtual void SetTaskStartTimeout(const openpal::MonotonicTimestamp& time) override final;

	/// methods for initiating command sequences

	void DirectOperate(CommandSet&& commands, const CommandCallbackT& callback, const TaskConfig& config);
	void SelectAndOperate(CommandSet&& commands, const CommandCallbackT& callback, const TaskConfig& config);


	/// -----  public methods used to add tasks -----

	std::shared_ptr<IMasterTask> AddScan(openpal::TimeDuration period, const HeaderBuilderT& builder, TaskConfig config = TaskConfig::Default());

	std::shared_ptr<IMasterTask> AddAllObjectsScan(GroupVariationID gvId, openpal::TimeDuration period, TaskConfig config = TaskConfig::Default());

	std::shared_ptr<IMasterTask> AddClassScan(const ClassField& field, openpal::TimeDuration period, TaskConfig config = TaskConfig::Default());

	std::shared_ptr<IMasterTask> AddRangeScan(GroupVariationID gvId, uint16_t start, uint16_t stop, openpal::TimeDuration period, TaskConfig config = TaskConfig::Default());

	/// ---- Single shot immediate scans ----

	void Scan(const HeaderBuilderT& builder, TaskConfig config = TaskConfig::Default());

	void ScanAllObjects(GroupVariationID gvId, TaskConfig config = TaskConfig::Default());

	void ScanClasses(const ClassField& field, TaskConfig config = TaskConfig::Default());

	void ScanRange(GroupVariationID gvId, uint16_t start, uint16_t stop, TaskConfig config = TaskConfig::Default());

	/// ---- Write tasks -----

	void Write(const TimeAndInterval& value, uint16_t index, TaskConfig config = TaskConfig::Default());

	void Restart(RestartType op, const RestartOperationCallbackT& callback, TaskConfig config = TaskConfig::Default());

	void PerformFunction(const std::string& name, opendnp3::FunctionCode func, const HeaderBuilderT& builder, TaskConfig config = TaskConfig::Default());

	/// public state manipulation actions

	TaskState BeginNewTask(const std::shared_ptr<IMasterTask>& task);

	TaskState ResumeActiveTask();

	void CompleteActiveTask();

	void QueueConfirm(const APDUHeader& header);

	void StartResponseTimer();

	void ProcessAPDU(const APDUResponseHeader& header, const openpal::RSlice& objects);

	void CheckForTask();

	bool CheckConfirmTransmit();

	void PostCheckForTask();

	void ProcessResponse(const APDUResponseHeader& header, const openpal::RSlice& objects);

	void ProcessUnsolicitedResponse(const APDUResponseHeader& header, const openpal::RSlice& objects);

	void Transmit(const openpal::RSlice& data);

private:

	void ScheduleRecurringPollTask(const std::shared_ptr<IMasterTask>& task);

	virtual void OnPendingTask() override
	{
		this->PostCheckForTask();
	}

	void ProcessIIN(const IINField& iin);

	void OnResponseTimeout();

protected:

	void ScheduleAdhocTask(const std::shared_ptr<IMasterTask>& task);

	/// state switch lookups
	TaskState OnStartEvent();
	TaskState OnResponseEvent(const APDUResponseHeader& header, const openpal::RSlice& objects);
	TaskState OnResponseTimeoutEvent();

	/// --- state handling functions ----

	TaskState StartTask_Idle();
	TaskState StartTask_TaskReady();

	TaskState OnResponse_WaitForResponse(const APDUResponseHeader& header, const openpal::RSlice& objects);
	TaskState OnResponseTimeout_WaitForResponse();
};

}

#endif
