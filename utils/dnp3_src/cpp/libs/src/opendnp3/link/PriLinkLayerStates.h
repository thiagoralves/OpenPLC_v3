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
#ifndef OPENDNP3_PRILINKLAYERSTATES_H
#define OPENDNP3_PRILINKLAYERSTATES_H

#include "opendnp3/link/Singleton.h"

namespace opendnp3
{

class LinkContext;
class ITransportSegment;


class PriStateBase
{
public:

	// Incoming messages for primary station
	virtual PriStateBase& OnAck(LinkContext&, bool receiveBuffFull);
	virtual PriStateBase& OnNack(LinkContext&, bool receiveBuffFull);
	virtual PriStateBase& OnLinkStatus(LinkContext&, bool receiveBuffFull);
	virtual PriStateBase& OnNotSupported(LinkContext&, bool receiveBuffFull);

	virtual PriStateBase& OnTransmitResult(LinkContext&, bool success);

	virtual PriStateBase& OnTimeout(LinkContext&);

	// transmission events to handle
	virtual PriStateBase& TrySendConfirmed(LinkContext&, ITransportSegment& segments);
	virtual PriStateBase& TrySendUnconfirmed(LinkContext&, ITransportSegment& segments);
	virtual PriStateBase& TrySendRequestLinkStatus(LinkContext&);

	//every concrete state implements this for logging purposes
	virtual char const* Name() const = 0;
};

//	@section desc Entry state for primary station
class PLLS_Idle final : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_Idle);

	virtual PriStateBase& TrySendUnconfirmed(LinkContext&, ITransportSegment& segments) override;
	virtual PriStateBase& TrySendConfirmed(LinkContext&, ITransportSegment& segments) override;
	virtual PriStateBase& TrySendRequestLinkStatus(LinkContext&) override;
};


/////////////////////////////////////////////////////////////////////////////
//  template wait state for send unconfirmed data
/////////////////////////////////////////////////////////////////////////////

class PLLS_SendUnconfirmedTransmitWait : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_SendUnconfirmedTransmitWait);

	virtual PriStateBase& OnTransmitResult(LinkContext& link, bool success) override;
};


/////////////////////////////////////////////////////////////////////////////
//  Wait for the link layer to transmit the reset links
/////////////////////////////////////////////////////////////////////////////

class PLLS_LinkResetTransmitWait : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_LinkResetTransmitWait);

	virtual PriStateBase& OnTransmitResult(LinkContext& link, bool success) override;
};

/////////////////////////////////////////////////////////////////////////////
//  Wait for the link layer to transmit confirmed user data
/////////////////////////////////////////////////////////////////////////////

class PLLS_ConfUserDataTransmitWait : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_ConfUserDataTransmitWait);

	virtual PriStateBase& OnTransmitResult(LinkContext& link, bool success) override;
};

/////////////////////////////////////////////////////////////////////////////
//  Wait for the link layer to transmit confirmed user data
/////////////////////////////////////////////////////////////////////////////

class PLLS_RequestLinkStatusTransmitWait : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_RequestLinkStatusTransmitWait);

	virtual PriStateBase& OnTransmitResult(LinkContext& link, bool success) override;
};

/////////////////////////////////////////////////////////////////////////////
//  Waiting for a ACK to reset the link
/////////////////////////////////////////////////////////////////////////////

//	@section desc As soon as we get an ACK, send the delayed pri frame
class PLLS_ResetLinkWait final : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_ResetLinkWait);

	PriStateBase& OnAck(LinkContext&, bool aIsRcvBuffFull) override;

	PriStateBase& OnNack(LinkContext&  link, bool)  override
	{
		return Failure(link);
	}
	PriStateBase& OnLinkStatus(LinkContext& link, bool) override
	{
		return Failure(link);
	}
	PriStateBase& OnNotSupported(LinkContext&  link, bool)  override
	{
		return Failure(link);
	}

	PriStateBase& OnTimeout(LinkContext&) override;

private:
	PriStateBase& Failure(LinkContext&);
};

/////////////////////////////////////////////////////////////////////////////
//  Waiting for a ACK to user data
/////////////////////////////////////////////////////////////////////////////

//	@section desc As soon as we get an ACK, send the delayed pri frame
class PLLS_ConfDataWait final : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_ConfDataWait);

	PriStateBase& OnAck(LinkContext&, bool aIsRcvBuffFull) override;
	PriStateBase& OnNack(LinkContext& link, bool) override;
	PriStateBase& OnLinkStatus(LinkContext& link, bool) override
	{
		return Failure(link);
	}
	PriStateBase& OnNotSupported(LinkContext& link, bool)  override
	{
		return Failure(link);
	}

	PriStateBase& OnTimeout(LinkContext&) override;

private:
	PriStateBase& Failure(LinkContext&);
};

/////////////////////////////////////////////////////////////////////////////
//  Waiting for a link status response
/////////////////////////////////////////////////////////////////////////////

//	@section desc As soon as we get an ACK, send the delayed pri frame
class PLLS_RequestLinkStatusWait final : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_RequestLinkStatusWait);

	PriStateBase& OnNack(LinkContext& link, bool) override;
	PriStateBase& OnLinkStatus(LinkContext& link, bool) override;
	PriStateBase& OnNotSupported(LinkContext& link, bool)  override;
	PriStateBase& OnTimeout(LinkContext&) override;
};

} //end namepsace

#endif

