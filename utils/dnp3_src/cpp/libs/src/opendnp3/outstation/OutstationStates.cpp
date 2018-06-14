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

#include "opendnp3/outstation/OutstationStates.h"

#include "opendnp3/outstation/OutstationContext.h"

#include "opendnp3/LogLevels.h"

#include "openpal/logging/LogMacros.h"

namespace opendnp3
{


// ------------- StateIdle ----------------

StateIdle StateIdle::instance;

OutstationState& StateIdle::OnConfirm(OContext& ctx, const APDUHeader& header)
{
	FORMAT_LOG_BLOCK(ctx.logger, flags::WARN, "unexpected confirm while IDLE with sequence: %u", header.control.SEQ);
	return StateIdle::Inst();
}

OutstationState& StateIdle::OnConfirmTimeout(OContext& ctx)
{
	SIMPLE_LOG_BLOCK(ctx.logger, flags::WARN, "unexpected confirm timeout");
	return StateIdle::Inst();
}

OutstationState& StateIdle::OnNewReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	return ctx.RespondToReadRequest(header, objects);
}

OutstationState& StateIdle::OnNewNonReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	ctx.RespondToNonReadRequest(header, objects);
	return *this;
}

OutstationState& StateIdle::OnRepeatNonReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	ctx.BeginResponseTx(ctx.sol.tx.GetLastControl(),  ctx.sol.tx.GetLastResponse());
	return *this;
}

OutstationState& StateIdle::OnRepeatReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	ctx.BeginResponseTx(ctx.sol.tx.GetLastControl(), ctx.sol.tx.GetLastResponse());
	return *this;
}


// ------------- StateSolicitedConfirmWait ----------------

StateSolicitedConfirmWait StateSolicitedConfirmWait::instance;

OutstationState& StateSolicitedConfirmWait::OnConfirm(OContext& ctx, const APDUHeader& header)
{
	if (header.control.UNS)
	{
		FORMAT_LOG_BLOCK(ctx.logger, flags::WARN, "received unsolicited confirm while waiting for solicited confirm (seq: %u)", header.control.SEQ);
		return *this;
	}

	if (!ctx.sol.seq.confirmNum.Equals(header.control.SEQ))
	{
		FORMAT_LOG_BLOCK(ctx.logger, flags::WARN, "solicited confirm with wrong seq: %u, expected: %u", header.control.SEQ, ctx.sol.seq.confirmNum.Get());
		return *this;
	}

	ctx.history.Reset(); // any time we get a confirm we can treat any request as a new request
	ctx.confirmTimer.Cancel();
	ctx.eventBuffer.ClearWritten();

	if (ctx.rspContext.HasSelection())
	{
		return ctx.ContinueMultiFragResponse(AppSeqNum(header.control.SEQ).Next());
	}
	else
	{
		return StateIdle::Inst();
	}
}

OutstationState& StateSolicitedConfirmWait::OnConfirmTimeout(OContext& ctx)
{
	SIMPLE_LOG_BLOCK(ctx.logger, flags::WARN, "solicited confirm timeout");
	return StateIdle::Inst();
}

OutstationState& StateSolicitedConfirmWait::OnNewReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	ctx.confirmTimer.Cancel();
	return ctx.RespondToReadRequest(header, objects);
}

OutstationState& StateSolicitedConfirmWait::OnNewNonReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	ctx.confirmTimer.Cancel();
	ctx.RespondToNonReadRequest(header, objects);
	return StateIdle::Inst();
}

OutstationState& StateSolicitedConfirmWait::OnRepeatNonReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	ctx.confirmTimer.Cancel();
	ctx.BeginResponseTx(ctx.sol.tx.GetLastControl(), ctx.sol.tx.GetLastResponse());
	return *this;
}

OutstationState& StateSolicitedConfirmWait::OnRepeatReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	ctx.RestartConfirmTimer();
	ctx.BeginResponseTx(ctx.sol.tx.GetLastControl(), ctx.sol.tx.GetLastResponse());
	return *this;
}


// ------------- StateUnsolicitedConfirmWait ----------------

StateUnsolicitedConfirmWait StateUnsolicitedConfirmWait::instance;

OutstationState& StateUnsolicitedConfirmWait::OnConfirm(OContext& ctx, const APDUHeader& header)
{
	if (!header.control.UNS)
	{
		FORMAT_LOG_BLOCK(ctx.logger, flags::WARN, "received solicited confirm while waiting for unsolicited confirm (seq: %u)", header.control.SEQ);
		return *this;
	}

	if (!ctx.unsol.seq.confirmNum.Equals(header.control.SEQ))
	{
		FORMAT_LOG_BLOCK(ctx.logger, flags::WARN, "unsolicited confirm with wrong seq: %u, expected: %u", header.control.SEQ, ctx.unsol.seq.confirmNum.Get());
		return *this;
	}

	ctx.history.Reset(); // any time we get a confirm we can treat any request as a new request
	ctx.confirmTimer.Cancel();

	if (ctx.unsol.completedNull)
	{
		ctx.eventBuffer.ClearWritten();
	}
	else
	{
		ctx.unsol.completedNull = true;
	}

	return StateIdle::Inst();
}

OutstationState& StateUnsolicitedConfirmWait::OnConfirmTimeout(OContext& ctx)
{
	SIMPLE_LOG_BLOCK(ctx.logger, flags::WARN, "unsolicited confirm timeout");

	if (ctx.unsol.completedNull)
	{
		ctx.eventBuffer.Unselect();
	}

	return StateIdle::Inst();
}

OutstationState& StateUnsolicitedConfirmWait::OnNewReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	ctx.deferred.Set(header, objects);
	return *this;
}

OutstationState& StateUnsolicitedConfirmWait::OnNewNonReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	ctx.deferred.Reset();	
	ctx.RespondToNonReadRequest(header, objects);
	return *this;
}

OutstationState& StateUnsolicitedConfirmWait::OnRepeatNonReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	ctx.BeginResponseTx(ctx.sol.tx.GetLastControl(), ctx.sol.tx.GetLastResponse());
	return *this;
}

OutstationState& StateUnsolicitedConfirmWait::OnRepeatReadRequest(OContext& ctx, const APDUHeader& header, const openpal::RSlice& objects)
{
	ctx.deferred.Set(header, objects);
	return *this;
}



} //end ns





