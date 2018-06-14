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
#ifndef ASIODNP3_IOHANDLER_H
#define ASIODNP3_IOHANDLER_H

#include "opendnp3/Route.h"
#include "opendnp3/link/ILinkTx.h"
#include "opendnp3/link/LinkLayerParser.h"
#include "opendnp3/master/MultidropTaskLock.h"

#include "asiodnp3/IChannelListener.h"

#include "openpal/logging/Logger.h"

#include "asiopal/IAsyncChannel.h"

#include <vector>
#include <deque>

namespace asiodnp3
{

/**

Manages I/O for a number of link contexts

*/
class IOHandler : private opendnp3::IFrameSink, public asiopal::IChannelCallbacks, public std::enable_shared_from_this<IOHandler>
{

public:

	IOHandler(
	    const openpal::Logger& logger,
	    const std::shared_ptr<IChannelListener>& listener
	);

	virtual ~IOHandler() {}

	opendnp3::LinkStatistics Statistics() const
	{
		return opendnp3::LinkStatistics(this->statistics, this->parser.Statistics());
	}

	opendnp3::ITaskLock& TaskLock()
	{
		return this->taskLock;
	}

	void Shutdown();

	/// --- implement ILinkTx ---

	void BeginTransmit(const std::shared_ptr<opendnp3::ILinkSession>& session, const openpal::RSlice& data);

	// Bind a link layer session to the handler
	bool AddContext(const std::shared_ptr<opendnp3::ILinkSession>& session, const opendnp3::Route& route);

	// Begin sending messages to the context
	bool Enable(const std::shared_ptr<opendnp3::ILinkSession>& session);

	// Stop sending messages to this session
	bool Disable(const std::shared_ptr<opendnp3::ILinkSession>& session);

	// Remove this session entirely
	bool Remove(const std::shared_ptr<opendnp3::ILinkSession>& session);

	// Query to see if a route is in use
	bool IsRouteInUse(const opendnp3::Route& route) const;

protected:

	// ------ Implement IChannelCallbacks -----

	virtual void OnReadComplete(const std::error_code& ec, size_t num) override final;

	virtual void OnWriteComplete(const std::error_code& ec, size_t num) override final;

	// ------ Super classes will implement these -----

	// start getting a new channel
	virtual void BeginChannelAccept() = 0;

	// stop getting new channels
	virtual void SuspendChannelAccept() = 0;

	// shutdown any additional state
	virtual void ShutdownImpl() = 0;

	// the current channel has closed, start getting a new one
	virtual void OnChannelShutdown() = 0;

	// Called by the super class when a new channel is available
	void OnNewChannel(const std::shared_ptr<asiopal::IAsyncChannel>& channel);

	openpal::Logger logger;
	const std::shared_ptr<IChannelListener> listener;
	opendnp3::LinkStatistics::Channel statistics;

private:

	bool isShutdown = false;

	inline void UpdateListener(opendnp3::ChannelState state)
	{
		if (listener) listener->OnStateChange(state);
	}

	// called by the parser when a complete frame is read
	virtual bool OnFrame(const opendnp3::LinkHeaderFields& header, const openpal::RSlice& userdata) override final;


	bool IsSessionInUse(const std::shared_ptr<opendnp3::ILinkSession>& session) const;
	bool IsAnySessionEnabled() const;
	void Reset();
	void BeginRead();
	void CheckForSend();

	bool SendToSession(const opendnp3::Route& route, const opendnp3::LinkHeaderFields& header, const openpal::RSlice& userdata);

	class Session
	{

	public:

		Session(const std::shared_ptr<opendnp3::ILinkSession>& session, const opendnp3::Route& route) :
			route(route),
			session(session)
		{}

		Session() = default;

		inline bool Matches(const std::shared_ptr<opendnp3::ILinkSession>& session) const
		{
			return this->session == session;
		}
		inline bool Matches(const opendnp3::Route& route) const
		{
			return this->route.Equals(route);
		}

		inline bool OnFrame(const opendnp3::LinkHeaderFields& header, const openpal::RSlice& userdata)
		{
			return this->session->OnFrame(header, userdata);
		}

		inline bool LowerLayerUp()
		{
			if (!online)
			{
				online = true;
				return this->session->OnLowerLayerUp();
			}
			else
			{
				return false;
			}
		}

		inline bool LowerLayerDown()
		{
			if(online)
			{
				online = false;
				return this->session->OnLowerLayerDown();
			}
			else
			{
				return false;
			}
		}

		bool enabled = false;

	private:

		opendnp3::Route route;
		bool online = false;
		std::shared_ptr<opendnp3::ILinkSession> session;
	};

	struct Transmission
	{
		Transmission(const openpal::RSlice& txdata, const std::shared_ptr<opendnp3::ILinkSession>& session) :
			txdata(txdata),
			session(session)
		{}

		Transmission() = default;

		openpal::RSlice txdata;
		std::shared_ptr<opendnp3::ILinkSession> session;
	};

	std::vector<Session> sessions;
	std::deque<Transmission>  txQueue;

	opendnp3::LinkLayerParser parser;

	// current value of the channel, may be empty
	std::shared_ptr<asiopal::IAsyncChannel> channel;

	opendnp3::MultidropTaskLock taskLock;
};

}

#endif

