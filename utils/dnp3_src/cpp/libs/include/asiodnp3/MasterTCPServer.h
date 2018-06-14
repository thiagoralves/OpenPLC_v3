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
#ifndef ASIOPAL_MASTERTCPSERVER_H
#define ASIOPAL_MASTERTCPSERVER_H

#include <openpal/logging/Logger.h>

#include <asiopal/TCPServer.h>
#include <asiopal/ResourceManager.h>
#include <asiopal/IListener.h>
#include <asiopal/IPEndpoint.h>

#include "asiodnp3/IListenCallbacks.h"

namespace asiodnp3
{
/**
* Binds and listens on an IPv4 TCP port
*
* Meant to be used exclusively as a shared_ptr
*/
class MasterTCPServer final : public asiopal::TCPServer
{

public:

	MasterTCPServer(
	    const openpal::Logger& logger,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    const asiopal::IPEndpoint& endpoint,
	    const std::shared_ptr<IListenCallbacks>& callbacks,
	    const std::shared_ptr<asiopal::ResourceManager>& manager,
	    std::error_code& ec
	);

	static std::shared_ptr<MasterTCPServer> Create(
	    const openpal::Logger& logger,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    const asiopal::IPEndpoint& endpoint,
	    const std::shared_ptr<IListenCallbacks>& callbacks,
	    const std::shared_ptr<asiopal::ResourceManager>& manager,
	    std::error_code& ec)
	{
		auto server = std::make_shared<MasterTCPServer>(logger, executor, endpoint, callbacks, manager, ec);

		if (!ec)
		{
			server->StartAccept();
		}

		return server;
	}


private:

	std::shared_ptr<IListenCallbacks> callbacks;
	std::shared_ptr<asiopal::ResourceManager> manager;

	static std::string SessionIdToString(uint64_t sessionid);

	// implement the virutal methods from TCPServer

	virtual void OnShutdown() override;

	virtual void AcceptConnection(uint64_t sessionid, const std::shared_ptr<asiopal::Executor>& executor, asio::ip::tcp::socket) override;
};

}

#endif
