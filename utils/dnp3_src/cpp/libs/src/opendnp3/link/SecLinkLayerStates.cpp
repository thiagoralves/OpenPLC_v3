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
#include "SecLinkLayerStates.h"

#include "openpal/logging/LogMacros.h"

#include "opendnp3/link/LinkLayer.h"
#include "opendnp3/LogLevels.h"

using namespace openpal;

namespace opendnp3
{

////////////////////////////////////////
// SecStateBase
////////////////////////////////////////

SecStateBase& SecStateBase::OnTransmitResult(LinkContext& ctx, bool success)
{
	FORMAT_LOG_BLOCK(ctx.logger, flags::ERR, "Invalid event for state: %s", this->Name());
	return *this;
}

////////////////////////////////////////////////////////
//	Class SLLS_NotReset
////////////////////////////////////////////////////////
SLLS_NotReset SLLS_NotReset::instance;

SecStateBase& SLLS_NotReset::OnTestLinkStatus(LinkContext& ctx, bool aFcb)
{
	++ctx.statistics.numUnexpectedFrame;
	SIMPLE_LOG_BLOCK(ctx.logger, flags::WARN, "TestLinkStatus ignored");
	return *this;
}

SecStateBase& SLLS_NotReset::OnConfirmedUserData(LinkContext& ctx, bool aFcb, const openpal::RSlice&)
{
	++ctx.statistics.numUnexpectedFrame;
	SIMPLE_LOG_BLOCK(ctx.logger, flags::WARN, "ConfirmedUserData ignored");
	return *this;
}

SecStateBase& SLLS_NotReset::OnResetLinkStates(LinkContext& ctx)
{
	ctx.QueueAck();
	ctx.ResetReadFCB();
	return SLLS_TransmitWaitReset::Instance();
}

SecStateBase& SLLS_NotReset::OnRequestLinkStatus(LinkContext& ctx)
{
	ctx.QueueLinkStatus();
	return SLLS_TransmitWaitNotReset::Instance();
}

////////////////////////////////////////////////////////
//	Class SLLS_Reset
////////////////////////////////////////////////////////
SLLS_Reset SLLS_Reset::instance;

SecStateBase& SLLS_Reset::OnTestLinkStatus(LinkContext& ctx, bool fcb)
{
	if(ctx.nextReadFCB == fcb)
	{
		ctx.QueueAck();
		ctx.ToggleReadFCB();
		return SLLS_TransmitWaitReset::Instance();
	}
	else
	{
		// "Re-transmit most recent response that contained function code 0 (ACK) or 1 (NACK)."
		// This is a PITA implement
		// TODO - see if this function is deprecated or not
		SIMPLE_LOG_BLOCK(ctx.logger, flags::WARN, "Received TestLinkStatus with invalid FCB");
		return *this;
	}
}

SecStateBase& SLLS_Reset::OnConfirmedUserData(LinkContext& ctx, bool fcb, const openpal::RSlice& data)
{
	ctx.QueueAck();

	if (ctx.nextReadFCB == fcb)
	{
		ctx.ToggleReadFCB();
		ctx.PushDataUp(data);
	}
	else
	{
		SIMPLE_LOG_BLOCK(ctx.logger, flags::WARN, "Confirmed data w/ wrong FCB");
	}

	return SLLS_TransmitWaitReset::Instance();
}

SecStateBase& SLLS_Reset::OnResetLinkStates(LinkContext& ctx)
{
	ctx.QueueAck();
	ctx.ResetReadFCB();
	return SLLS_TransmitWaitReset::Instance();
}

SecStateBase& SLLS_Reset::OnRequestLinkStatus(LinkContext& ctx)
{
	ctx.QueueLinkStatus();
	return SLLS_TransmitWaitReset::Instance();
}

////////////////////////////////////////////////////////
//	Class SLLS_TransmitWaitReset
////////////////////////////////////////////////////////
SLLS_TransmitWaitReset SLLS_TransmitWaitReset::instance;

////////////////////////////////////////////////////////
//	Class SLLS_TransmitWaitNotReset
////////////////////////////////////////////////////////
SLLS_TransmitWaitNotReset SLLS_TransmitWaitNotReset::instance;

} //end namepsace

