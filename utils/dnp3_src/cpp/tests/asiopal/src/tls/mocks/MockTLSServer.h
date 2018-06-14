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

#ifndef ASIOPAL_MOCKTLSSERVER_H
#define ASIOPAL_MOCKTLSSERVER_H

#include "asiopal/tls/TLSServer.h"
#include "asiopal/IAsyncChannel.h"
#include "asiopal/tls/TLSStreamChannel.h"

#include <deque>

namespace asiopal
{

class MockTLSServer final : public TLSServer
{

public:

	MockTLSServer(
	    const openpal::Logger& logger,
	    std::shared_ptr<Executor> executor,
	    IPEndpoint endpoint,
	    const TLSConfig& config,
	    std::error_code& ec
	) : TLSServer(logger, executor, endpoint, config, ec)
	{

	}

	static std::shared_ptr<MockTLSServer> Create(
	    const openpal::Logger& logger,
	    std::shared_ptr<Executor> executor,
	    IPEndpoint endpoint,
	    const TLSConfig& config,
	    std::error_code& ec)
	{
		auto server = std::make_shared<MockTLSServer>(logger, executor, endpoint, config, ec);

		if (ec) return nullptr;

		server->StartAccept(ec);

		if (ec) return nullptr;

		return server;
	}

	virtual bool AcceptConnection(uint64_t sessionid, const asio::ip::tcp::endpoint& remote) override
	{
		return true;
	}

	virtual bool VerifyCallback(uint64_t sessionid, bool preverified, asio::ssl::verify_context& ctx)
	{
		return preverified;
	}

	virtual void AcceptStream(uint64_t sessionid, const std::shared_ptr<Executor>& executor, std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> stream)
	{
		channels.push_back(TLSStreamChannel::Create(executor, stream));
	}

	virtual void OnShutdown() {}


	~MockTLSServer()
	{
		for (auto& channel : channels)
		{
			channel->Shutdown();
		}
	}

	std::deque<std::shared_ptr<IAsyncChannel>> channels;
};

}

#endif








