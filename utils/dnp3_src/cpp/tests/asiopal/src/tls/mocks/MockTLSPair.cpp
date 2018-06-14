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

#include "MockTLSPair.h"

namespace asiopal
{


MockTLSPair::MockTLSPair(std::shared_ptr<MockIO> io, uint16_t port, const TLSConfig& client, const TLSConfig& server, std::error_code ec) :
	log(),
	io(io),
	chandler(std::make_shared<MockTLSClientHandler>()),
	client(TLSClient::Create(log.logger, io->GetExecutor(), IPEndpoint::Localhost(port), "127.0.0.1", client, ec)),
	server(ec ? nullptr : MockTLSServer::Create(log.logger, io->GetExecutor(), IPEndpoint::Localhost(port), server, ec))
{
	if (ec)
	{
		throw std::logic_error(ec.message());
	}
}

MockTLSPair::~MockTLSPair()
{
	this->server->Shutdown();
	this->client->Cancel();
}

void MockTLSPair::Connect(size_t num)
{
	auto callback = [handler = this->chandler](
	                    const std::shared_ptr<Executor>& executor,
	                    const std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>>& stream,
	                    const std::error_code & ec)
	{
		handler->OnConnect(executor, stream, ec);
	};

	if (!this->client->BeginConnect(callback))
	{
		throw std::logic_error("BeginConnect returned false");
	}

	auto connected = [this, num]() -> bool
	{
		return this->NumConnectionsEqual(num);
	};

	io->CompleteInXIterations(10, connected);
}

bool MockTLSPair::NumConnectionsEqual(size_t num) const
{
	return (this->server->channels.size() == num) && (this->chandler->channels.size() == num);
}

}









