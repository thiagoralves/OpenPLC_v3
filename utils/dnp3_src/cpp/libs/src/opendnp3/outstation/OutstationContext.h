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
#ifndef OPENDNP3_OUTSTATIONCONTEXT_H
#define OPENDNP3_OUTSTATIONCONTEXT_H

#include "opendnp3/LayerInterfaces.h"

#include "opendnp3/gen/SecurityStatIndex.h"

#include "opendnp3/outstation/OutstationConfig.h"
#include "opendnp3/outstation/RequestHistory.h"
#include "opendnp3/outstation/DeferredRequest.h"
#include "opendnp3/outstation/OutstationChannelStates.h"
#include "opendnp3/outstation/ControlState.h"
#include "opendnp3/outstation/OutstationSeqNum.h"
#include "opendnp3/outstation/Database.h"
#include "opendnp3/outstation/EventBuffer.h"
#include "opendnp3/outstation/ResponseContext.h"
#include "opendnp3/outstation/ICommandHandler.h"
#include "opendnp3/outstation/IOutstationApplication.h"
#include "opendnp3/outstation/OutstationStates.h"

#include <openpal/executor/TimerRef.h>
#include <openpal/logging/Logger.h>
#include <openpal/container/Pair.h>

namespace opendnp3
{

///
/// Represent all of the mutable state in an outstation
///
class OContext : public IUpperLayer
{
	friend class StateIdle;
	friend class StateSolicitedConfirmWait;
	friend class StateUnsolicitedConfirmWait;

public:

	OContext(	const OutstationConfig& config,
	            const DatabaseSizes& dbSizes,
	            const openpal::Logger& logger,
	            const std::shared_ptr<openpal::IExecutor>& executor,
	            const std::shared_ptr<ILowerLayer>& lower,
	            const std::shared_ptr<ICommandHandler>& commandHandler,
	            const std::shared_ptr<IOutstationApplication>& application);

	/// ----- Implement IUpperLayer ------

	virtual bool OnLowerLayerUp() override;

	virtual bool OnLowerLayerDown() override;

	virtual bool OnSendResult(bool isSuccess) override final;

	virtual bool OnReceive(const openpal::RSlice& fragment) override final;

	/// --- Other public members ----

	void CheckForTaskStart();

	IUpdateHandler& GetUpdateHandler();

	DatabaseConfigView GetConfigView();

	void SetRestartIIN();

private:

	/// ---- Helper functions that operate on the current state, and may return a new state ----

	OutstationState& ContinueMultiFragResponse(const AppSeqNum& seq);

	OutstationState& RespondToReadRequest(const APDUHeader& header, const openpal::RSlice& objects);

	OutstationState& ProcessNewRequest(const APDUHeader& header, const openpal::RSlice& objects);

	OutstationState& OnReceiveSolRequest(const APDUHeader& header, const openpal::RSlice& objects);

	void RespondToNonReadRequest(const APDUHeader& header, const openpal::RSlice& objects);

	/// ---- Processing functions --------

	void ProcessAPDU(const openpal::RSlice& apdu, const APDUHeader& header, const openpal::RSlice& objects);

	void ProcessRequest(const APDUHeader& header, const openpal::RSlice& objects);

	void ProcessConfirm(const APDUHeader& header);

	/// ---- common helper methods ----

	void ParseHeader(const openpal::RSlice& apdu);

	void BeginResponseTx(const AppControlField& control, const openpal::RSlice& response);

	void BeginUnsolTx(const AppControlField& control, const openpal::RSlice& response);

	void BeginTx(const openpal::RSlice& response);

	void CheckForDeferredRequest();

	bool ProcessDeferredRequest(APDUHeader header, openpal::RSlice objects);

	void RestartConfirmTimer();

	void CheckForUnsolicited();

	bool CanTransmit() const;

	IINField GetResponseIIN();

	IINField GetDynamicIIN();

	/// --- methods for handling app-layer functions ---

	/// Handles non-read function codes that require a response. builds the response using the supplied writer.
	/// @return An IIN field indicating the validity of the request, and to be returned in the response.
	IINField HandleNonReadResponse(const APDUHeader& header, const openpal::RSlice& objects, HeaderWriter& writer);

	/// Handles read function codes. May trigger an unsolicited response
	/// @return an IIN field and a partial AppControlField (missing sequence info)
	openpal::Pair<IINField, AppControlField> HandleRead(const openpal::RSlice& objects, HeaderWriter& writer);

	/// Handles no-response function codes.
	void ProcessRequestNoAck(const APDUHeader& header, const openpal::RSlice& objects);

	// ------ Function Handlers ------

	IINField HandleWrite(const openpal::RSlice& objects);
	IINField HandleSelect(const openpal::RSlice& objects, HeaderWriter& writer);
	IINField HandleOperate(const openpal::RSlice& objects, HeaderWriter& writer);
	IINField HandleDirectOperate(const openpal::RSlice& objects, OperateType opType, HeaderWriter* pWriter);
	IINField HandleDelayMeasure(const openpal::RSlice& objects, HeaderWriter& writer);
	IINField HandleRestart(const openpal::RSlice& objects, bool isWarmRestart, HeaderWriter* pWriter);
	IINField HandleAssignClass(const openpal::RSlice& objects);
	IINField HandleDisableUnsolicited(const openpal::RSlice& objects, HeaderWriter& writer);
	IINField HandleEnableUnsolicited(const openpal::RSlice& objects, HeaderWriter& writer);
	IINField HandleCommandWithConstant(const openpal::RSlice& objects, HeaderWriter& writer, CommandStatus status);

	// ------ resources --------
	openpal::Logger logger;
	const std::shared_ptr<openpal::IExecutor> executor;
	const std::shared_ptr<ILowerLayer> lower;
	const std::shared_ptr<ICommandHandler> commandHandler;
	const std::shared_ptr<IOutstationApplication> application;

	// ------ Database, event buffer, and response tracking
	EventBuffer eventBuffer;
	Database database;
	ResponseContext rspContext;

	// ------ Static configuration -------
	OutstationParams params;

	// ------ Shared dynamic state --------
	bool isOnline;
	bool isTransmitting;
	IINField staticIIN;
	openpal::TimerRef confirmTimer;
	RequestHistory history;
	DeferredRequest deferred;

	// ------ Dynamic state related to controls ------
	ControlState control;

	// ------ Dynamic state related to solicited and unsolicited modes ------
	OutstationSolState  sol;
	OutstationUnsolState unsol;
	OutstationState* state = &StateIdle::Inst();
};


}

#endif
