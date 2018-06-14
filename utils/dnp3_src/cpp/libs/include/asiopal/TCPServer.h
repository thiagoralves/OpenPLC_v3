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
#ifndef ASIOPAL_TCPSERVER_H
#define ASIOPAL_TCPSERVER_H

#include "asiopal/IPEndpoint.h"
#include "asiopal/Executor.h"
#include "asiopal/IListener.h"

#include <openpal/util/Uncopyable.h>
#include <openpal/logging/Logger.h>

#include <memory>

namespace asiopal
{

/**
* Binds and listens on an IPv4 TCP port
*
* Meant to be used exclusively as a shared_ptr
*/
class TCPServer :
	public std::enable_shared_from_this<TCPServer>,
	public IListener,
	private openpal::Uncopyable
{

public:

	TCPServer(
	    const openpal::Logger& logger,
	    const std::shared_ptr<Executor>& executor,
	    const IPEndpoint& endpoint,
	    std::error_code& ec
	);

	/// Implement IListener
	void Shutdown() override final;

protected:

	/// Inherited classes must define these functions

	virtual void OnShutdown() = 0;

	virtual void AcceptConnection(uint64_t sessionid, const std::shared_ptr<Executor>& executor, asio::ip::tcp::socket) = 0;

	/// Start asynchronously accepting connections on the strand
	void StartAccept();

	openpal::Logger logger;
	std::shared_ptr<Executor> executor;

private:

	void Configure(const std::string& adapter, std::error_code& ec);

	asio::ip::tcp::endpoint endpoint;
	asio::ip::tcp::acceptor acceptor;
	asio::ip::tcp::socket socket;
	asio::ip::tcp::endpoint remote_endpoint;
	uint64_t session_id = 0;
};

}

#endif
