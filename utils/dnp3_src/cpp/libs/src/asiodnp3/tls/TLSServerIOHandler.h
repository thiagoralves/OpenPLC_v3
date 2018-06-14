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

#ifndef ASIOPAL_TLSSERVERIOHANDLER_H
#define ASIOPAL_TLSSERVERIOHANDLER_H

#include "asiodnp3/IOHandler.h"

#include "asiopal/ChannelRetry.h"
#include "asiopal/IPEndpoint.h"
#include "asiopal/tls/TLSServer.h"

#include "openpal/executor/TimerRef.h"

namespace asiodnp3
{

class TLSServerIOHandler final : public IOHandler
{
	class Server final : public asiopal::TLSServer
	{
	public:

		typedef std::function<void(const std::shared_ptr<asiopal::IAsyncChannel>& )> callback_t;

		Server(
		    const openpal::Logger& logger,
		    const std::shared_ptr<asiopal::Executor>& executor,
		    const asiopal::IPEndpoint& endpoint,
		    const asiopal::TLSConfig& config,
		    std::error_code& ec
		) :
			TLSServer(logger, executor, endpoint, config, ec)
		{}

		void StartAcceptingConnection(const callback_t& callback, std::error_code& ec)
		{
			this->callback = callback;
			this->StartAccept(ec);
		}

	private:

		callback_t callback;

		virtual bool AcceptConnection(uint64_t sessionid, const asio::ip::tcp::endpoint& remote) override
		{
			return true;
		}
		virtual bool VerifyCallback(uint64_t sessionid, bool preverified, asio::ssl::verify_context& ctx) override
		{
			return preverified;
		}
		virtual void OnShutdown() override {}

		virtual void AcceptStream(uint64_t sessionid, const std::shared_ptr<asiopal::Executor>& executor, std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> stream) override;
	};

public:

	static std::shared_ptr<TLSServerIOHandler> Create(
	    const openpal::Logger& logger,
	    const std::shared_ptr<IChannelListener>& listener,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    const asiopal::IPEndpoint& endpoint,
	    const asiopal::TLSConfig& config,
	    std::error_code& ec)
	{
		return std::make_shared<TLSServerIOHandler>(logger, listener, executor, endpoint, config, ec);
	}

	TLSServerIOHandler(
	    const openpal::Logger& logger,
	    const std::shared_ptr<IChannelListener>& listener,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    const asiopal::IPEndpoint& endpoint,
	    const asiopal::TLSConfig& config,
	    std::error_code& ec
	);

protected:

	virtual void ShutdownImpl() override;
	virtual void BeginChannelAccept() override;
	virtual void SuspendChannelAccept() override;
	virtual void OnChannelShutdown() override {} // do nothing, always accepting new connections

private:

	const std::shared_ptr<asiopal::Executor> executor;
	const asiopal::IPEndpoint endpoint;
	const asiopal::TLSConfig config;
	std::shared_ptr<Server> server;
};

}

#endif
