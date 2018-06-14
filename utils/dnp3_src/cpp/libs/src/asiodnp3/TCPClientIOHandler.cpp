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

#include "TCPClientIOHandler.h"

#include "asiopal/SocketChannel.h"

using namespace asiopal;

namespace asiodnp3
{

TCPClientIOHandler::TCPClientIOHandler(
    const openpal::Logger& logger,
    const std::shared_ptr<IChannelListener>& listener,
    const std::shared_ptr<asiopal::Executor>& executor,
    const asiopal::ChannelRetry& retry,
    const asiopal::IPEndpoint& remote,
    const std::string& adapter
) :
	IOHandler(logger, listener),
	executor(executor),
	retry(retry),
	remote(remote),
	adapter(adapter),
	retrytimer(*executor)
{}

void TCPClientIOHandler::ShutdownImpl()
{
	this->ResetState();
}

void TCPClientIOHandler::BeginChannelAccept()
{
	this->client = TCPClient::Create(logger, executor, remote, adapter);
	this->StartConnect(this->client, this->retry.minOpenRetry);
}

void TCPClientIOHandler::SuspendChannelAccept()
{
	this->ResetState();
}

void TCPClientIOHandler::OnChannelShutdown()
{
	this->BeginChannelAccept();
}

void TCPClientIOHandler::StartConnect(const std::shared_ptr<asiopal::TCPClient>& client, const openpal::TimeDuration& delay)
{
	FORMAT_LOG_BLOCK(this->logger, openpal::logflags::INFO, "Connecting to: %s", this->remote.address.c_str());

	auto cb = [ =, self = shared_from_this()](const std::shared_ptr<Executor>& executor, asio::ip::tcp::socket socket, const std::error_code & ec) -> void
	{
		if (ec)
		{
			FORMAT_LOG_BLOCK(this->logger, openpal::logflags::WARN, "Error Connecting: %s", ec.message().c_str());

			++this->statistics.numOpenFail;

			const auto newDelay = this->retry.NextDelay(delay);

			auto cb = [self, newDelay, client, this]()
			{
				this->StartConnect(client, newDelay);
			};

			this->retrytimer.Start(delay, cb);
		}
		else
		{
			FORMAT_LOG_BLOCK(this->logger, openpal::logflags::INFO, "Connected to: %s", this->remote.address.c_str());

			this->OnNewChannel(SocketChannel::Create(executor, std::move(socket)));
		}

	};

	this->client->BeginConnect(cb);
}

void TCPClientIOHandler::ResetState()
{
	if (this->client)
	{
		this->client->Cancel();
		this->client.reset();
	}

	retrytimer.Cancel();
}

}



