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
#include "LinkContext.h"

#include "opendnp3/link/PriLinkLayerStates.h"
#include "opendnp3/link/SecLinkLayerStates.h"
#include "opendnp3/link/LinkFrame.h"
#include "opendnp3/link/ILinkTx.h"

#include "openpal/util/Limits.h"

using namespace openpal;

namespace opendnp3
{

LinkContext::LinkContext(
    const openpal::Logger& logger,
    const std::shared_ptr<openpal::IExecutor>& executor,
    const std::shared_ptr<IUpperLayer>& upper,
    const std::shared_ptr<opendnp3::ILinkListener>& listener,
    ILinkSession& session,
    const LinkConfig& config)
	:
	logger(logger),
	config(config),
	pSegments(nullptr),
	txMode(LinkTransmitMode::Idle),
	numRetryRemaining(0),
	executor(executor),
	rspTimeoutTimer(*executor),
	keepAliveTimer(*executor),
	nextReadFCB(false),
	nextWriteFCB(false),
	isOnline(false),
	isRemoteReset(false),
	keepAliveTimeout(false),
	lastMessageTimestamp(executor->GetTime()),
	pPriState(&PLLS_Idle::Instance()),
	pSecState(&SLLS_NotReset::Instance()),
	listener(listener),
	upper(upper),
	pSession(&session)
{}

bool LinkContext::OnLowerLayerUp()
{
	if (this->isOnline)
	{
		SIMPLE_LOG_BLOCK(logger, flags::ERR, "Layer already online");
		return false;
	}

	const auto now = this->executor->GetTime();

	this->isOnline = true;

	this->lastMessageTimestamp = now; // no reason to trigger a keep-alive until we've actually expired

	this->StartKeepAliveTimer(now.Add(config.KeepAliveTimeout));

	listener->OnStateChange(opendnp3::LinkStatus::UNRESET);
	upper->OnLowerLayerUp();

	return true;
}

bool LinkContext::OnLowerLayerDown()
{
	if (!isOnline)
	{
		SIMPLE_LOG_BLOCK(logger, flags::ERR, "Layer is not online");
		return false;
	}

	isOnline = false;
	keepAliveTimeout = false;
	isRemoteReset = false;
	pSegments = nullptr;
	txMode = LinkTransmitMode::Idle;
	pendingPriTx.Clear();
	pendingSecTx.Clear();

	rspTimeoutTimer.Cancel();
	keepAliveTimer.Cancel();

	pPriState = &PLLS_Idle::Instance();
	pSecState = &SLLS_NotReset::Instance();

	listener->OnStateChange(opendnp3::LinkStatus::UNRESET);
	upper->OnLowerLayerDown();

	return true;
}

bool LinkContext::SetTxSegment(ITransportSegment& segments)
{
	if (!this->isOnline)
	{
		SIMPLE_LOG_BLOCK(this->logger, flags::ERR, "Layer is not online");
		return false;
	}

	if (this->pSegments)
	{
		SIMPLE_LOG_BLOCK(this->logger, flags::ERR, "Already transmitting a segment");
		return false;
	}

	this->pSegments = &segments;
	return true;
}

bool LinkContext::OnTransmitResult(bool success)
{
	if (this->txMode == LinkTransmitMode::Idle)
	{
		SIMPLE_LOG_BLOCK(this->logger, flags::ERR, "Unknown transmission callback");
		return false;
	}

	auto isPrimary = (this->txMode == LinkTransmitMode::Primary);
	this->txMode = LinkTransmitMode::Idle;

	// before we dispatch the transmit result, give any pending transmissions access first
	this->TryPendingTx(this->pendingSecTx, false);
	this->TryPendingTx(this->pendingPriTx, true);

	// now dispatch the completion event to the correct state handler
	if (isPrimary)
	{
		this->pPriState = &this->pPriState->OnTransmitResult(*this, success);
	}
	else
	{
		this->pSecState = &this->pSecState->OnTransmitResult(*this, success);
	}

	return true;
}

openpal::RSlice LinkContext::FormatPrimaryBufferWithConfirmed(const openpal::RSlice& tpdu, bool FCB)
{
	auto dest = this->priTxBuffer.GetWSlice();
	auto output = LinkFrame::FormatConfirmedUserData(dest, config.IsMaster, FCB, config.RemoteAddr, config.LocalAddr, tpdu, tpdu.Size(), &logger);
	FORMAT_HEX_BLOCK(logger, flags::LINK_TX_HEX, output, 10, 18);
	return output;
}

RSlice LinkContext::FormatPrimaryBufferWithUnconfirmed(const openpal::RSlice& tpdu)
{
	auto dest = this->priTxBuffer.GetWSlice();
	auto output = LinkFrame::FormatUnconfirmedUserData(dest, config.IsMaster, config.RemoteAddr, config.LocalAddr, tpdu, tpdu.Size(), &logger);
	FORMAT_HEX_BLOCK(logger, flags::LINK_TX_HEX, output, 10, 18);
	return output;
}

void LinkContext::QueueTransmit(const RSlice& buffer, bool primary)
{
	if (txMode == LinkTransmitMode::Idle)
	{
		txMode = primary ? LinkTransmitMode::Primary : LinkTransmitMode::Secondary;
		linktx->BeginTransmit(buffer, *pSession);
	}
	else
	{
		if (primary)
		{
			pendingPriTx.Set(buffer);
		}
		else
		{
			pendingSecTx.Set(buffer);
		}
	}
}

void LinkContext::QueueAck()
{
	auto dest = secTxBuffer.GetWSlice();
	auto buffer = LinkFrame::FormatAck(dest, config.IsMaster, false, config.RemoteAddr, config.LocalAddr, &logger);
	FORMAT_HEX_BLOCK(logger, flags::LINK_TX_HEX, buffer, 10, 18);
	this->QueueTransmit(buffer, false);
}

void LinkContext::QueueLinkStatus()
{
	auto dest = secTxBuffer.GetWSlice();
	auto buffer = LinkFrame::FormatLinkStatus(dest, config.IsMaster, false, config.RemoteAddr, config.LocalAddr, &logger);
	FORMAT_HEX_BLOCK(logger, flags::LINK_TX_HEX, buffer, 10, 18);
	this->QueueTransmit(buffer, false);
}

void LinkContext::QueueResetLinks()
{
	auto dest = priTxBuffer.GetWSlice();
	auto buffer = LinkFrame::FormatResetLinkStates(dest, config.IsMaster, config.RemoteAddr, config.LocalAddr, &logger);
	FORMAT_HEX_BLOCK(logger, flags::LINK_TX_HEX, buffer, 10, 18);
	this->QueueTransmit(buffer, true);
}

void LinkContext::QueueRequestLinkStatus()
{
	auto dest = priTxBuffer.GetWSlice();
	auto buffer = LinkFrame::FormatRequestLinkStatus(dest, config.IsMaster, config.RemoteAddr, config.LocalAddr, &logger);
	FORMAT_HEX_BLOCK(logger, flags::LINK_TX_HEX, buffer, 10, 18);
	this->QueueTransmit(buffer, true);
}

void LinkContext::ResetRetry()
{
	this->numRetryRemaining = config.NumRetry;
}

bool LinkContext::Retry()
{
	if (numRetryRemaining > 0)
	{
		--numRetryRemaining;
		return true;
	}
	else
	{
		return false;
	}
}

void LinkContext::PushDataUp(const openpal::RSlice& data)
{
	upper->OnReceive(data);
}

void LinkContext::CompleteSendOperation(bool success)
{
	this->pSegments = nullptr;

	auto callback = [upper = upper, success]()
	{
		upper->OnSendResult(success);
	};

	this->executor->Post(callback);

}

void LinkContext::TryStartTransmission()
{
	if (this->keepAliveTimeout)
	{
		this->pPriState = &pPriState->TrySendRequestLinkStatus(*this);
	}

	if (this->pSegments)
	{
		this->pPriState = (this->config.UseConfirms) ? &pPriState->TrySendConfirmed(*this, *pSegments) : &pPriState->TrySendUnconfirmed(*this, *pSegments);
	}
}

void LinkContext::OnKeepAliveTimeout()
{
	const auto now = this->executor->GetTime();

	auto elapsed = now.milliseconds - this->lastMessageTimestamp.milliseconds;

	if (elapsed >= this->config.KeepAliveTimeout.GetMilliseconds())
	{
		this->lastMessageTimestamp = now;
		this->keepAliveTimeout = true;
	}

	this->StartKeepAliveTimer(now.Add(config.KeepAliveTimeout));

	this->TryStartTransmission();
}

void LinkContext::OnResponseTimeout()
{
	this->pPriState = &(this->pPriState->OnTimeout(*this));

	this->TryStartTransmission();
}

void LinkContext::StartResponseTimer()
{
	rspTimeoutTimer.Start(
	    TimeDuration(config.Timeout),
	    [this]()
	{
		this->OnResponseTimeout();
	}
	);
}

void LinkContext::StartKeepAliveTimer(const MonotonicTimestamp& expiration)
{
	auto callback = [this]()
	{
		this->OnKeepAliveTimeout();
	};

	this->keepAliveTimer.Start(expiration, callback);
}

void LinkContext::CancelTimer()
{
	rspTimeoutTimer.Cancel();
}

void LinkContext::FailKeepAlive(bool timeout)
{
	if (timeout)
	{
		this->listener->OnKeepAliveFailure();
	}
}

void LinkContext::CompleteKeepAlive()
{
	this->listener->OnKeepAliveSuccess();
}

bool LinkContext::OnFrame(const LinkHeaderFields& header, const openpal::RSlice& userdata)
{
	if (!isOnline)
	{
		SIMPLE_LOG_BLOCK(logger, flags::ERR, "Layer is not online");
		return false;
	}

	if (!this->Validate(header.isFromMaster, header.src, header.dest))
	{
		return false;
	}

	// reset the keep-alive timestamp
	this->lastMessageTimestamp = this->executor->GetTime();

	switch (header.func)
	{
	case(LinkFunction::SEC_ACK) :
		pPriState = &pPriState->OnAck(*this, header.fcvdfc);
		return true;
	case(LinkFunction::SEC_NACK) :
		pPriState = &pPriState->OnNack(*this, header.fcvdfc);
		return true;
	case(LinkFunction::SEC_LINK_STATUS) :
		pPriState = &pPriState->OnLinkStatus(*this, header.fcvdfc);
		return true;
	case(LinkFunction::SEC_NOT_SUPPORTED) :
		pPriState = &pPriState->OnNotSupported(*this, header.fcvdfc);
		return true;
	case(LinkFunction::PRI_TEST_LINK_STATES) :
		pSecState = &pSecState->OnTestLinkStatus(*this, header.fcb);
		return true;
	case(LinkFunction::PRI_RESET_LINK_STATES) :
		pSecState = &pSecState->OnResetLinkStates(*this);
		return true;
	case(LinkFunction::PRI_REQUEST_LINK_STATUS) :
		pSecState = &pSecState->OnRequestLinkStatus(*this);
		return true;
	case(LinkFunction::PRI_CONFIRMED_USER_DATA) :
		pSecState = &pSecState->OnConfirmedUserData(*this, header.fcb, userdata);
		return true;
	case(LinkFunction::PRI_UNCONFIRMED_USER_DATA) :
		this->PushDataUp(userdata);
		return true;
	default:
		return false;
	}
}

bool LinkContext::Validate(bool isMaster, uint16_t src, uint16_t dest)
{
	if (isMaster == config.IsMaster)
	{
		++statistics.numBadMasterBit;
		SIMPLE_LOG_BLOCK(logger, flags::WARN, (isMaster ? "Master frame received for master" : "Outstation frame received for outstation"));
		return false;
	}

	if (dest != config.LocalAddr)
	{
		++statistics.numUnknownDestination;
		SIMPLE_LOG_BLOCK(logger, flags::WARN, "Frame for unknown destintation");
		return false;
	}

	if (src != config.RemoteAddr)
	{
		++statistics.numUnknownSource;
		SIMPLE_LOG_BLOCK(logger, flags::WARN, "Frame from unknwon source");
		return false;
	}

	return true;
}


bool LinkContext::TryPendingTx(openpal::Settable<RSlice>& pending, bool primary)
{
	if (this->txMode == LinkTransmitMode::Idle && pending.IsSet())
	{
		this->linktx->BeginTransmit(pending.Get(), *pSession);
		pending.Clear();
		this->txMode = primary ? LinkTransmitMode::Primary : LinkTransmitMode::Secondary;
		return true;
	}

	return false;
}

}

