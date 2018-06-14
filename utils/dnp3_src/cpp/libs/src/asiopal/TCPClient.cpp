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

#include "asiopal/TCPClient.h"

#include "asiopal/SocketHelpers.h"

namespace asiopal
{

TCPClient::TCPClient(
    const openpal::Logger& logger,
    const std::shared_ptr<Executor>& executor,
    const IPEndpoint& remote,
    const std::string& adapter
) :
	condition(logger),
	executor(executor),
	host(remote.address),
	adapter(adapter),
	socket(executor->strand.get_io_service()),
	remoteEndpoint(asio::ip::tcp::v4(), remote.port),
	localEndpoint(),
	resolver(executor->strand.get_io_service())
{}


bool TCPClient::Cancel()
{
	if (this->canceled || !this->connecting)
	{
		return false;
	}

	std::error_code ec;
	socket.cancel(ec);
	resolver.cancel();
	this->canceled = true;
	return true;
}

bool TCPClient::BeginConnect(const connect_callback_t& callback)
{
	if (connecting || canceled) return false;

	this->connecting = true;

	std::error_code ec;
	SocketHelpers::BindToLocalAddress(this->adapter, this->localEndpoint, this->socket, ec);

	if (ec)
	{
		return this->PostConnectError(callback, ec);
	}

	const auto address = asio::ip::address::from_string(this->host, ec);
	auto self = this->shared_from_this();
	if (ec)
	{
		// Try DNS resolution instead
		auto cb = [self, callback](const std::error_code & ec, asio::ip::tcp::resolver::iterator endpoints)
		{
			self->HandleResolveResult(callback, endpoints, ec);
		};

		std::stringstream portstr;
		portstr << remoteEndpoint.port();

		resolver.async_resolve(
		    asio::ip::tcp::resolver::query(host, portstr.str()),
		    executor->strand.wrap(cb)
		);

		return true;
	}
	else
	{
		remoteEndpoint.address(address);
		auto cb = [self, callback](const std::error_code & ec)
		{
			self->connecting = false;
			if (!self->canceled)
			{
				callback(self->executor, std::move(self->socket), ec);
			}
		};

		socket.async_connect(remoteEndpoint, executor->strand.wrap(cb));
		return true;
	}
}

void TCPClient::HandleResolveResult(
    const connect_callback_t& callback,
    const asio::ip::tcp::resolver::iterator& endpoints,
    const std::error_code& ec
)
{
	if (ec)
	{
		this->PostConnectError(callback, ec);
	}
	else
	{
		// attempt a connection to each endpoint in the iterator until we connect
		auto cb = [self = shared_from_this(), callback](const std::error_code & ec, asio::ip::tcp::resolver::iterator endpoints)
		{
			self->connecting = false;
			if (!self->canceled)
			{
				callback(self->executor, std::move(self->socket), ec);
			}
		};

		asio::async_connect(this->socket, endpoints, this->condition, this->executor->strand.wrap(cb));
	}
}

bool TCPClient::PostConnectError(const connect_callback_t& callback, const std::error_code& ec)
{
	auto cb = [self = shared_from_this(), ec, callback]()
	{
		self->connecting = false;
		if (!self->canceled)
		{
			callback(self->executor, std::move(self->socket), ec);
		}
	};
	executor->strand.post(cb);
	return true;
}

}


