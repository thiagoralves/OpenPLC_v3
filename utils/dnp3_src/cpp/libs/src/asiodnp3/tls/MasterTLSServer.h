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
#ifndef ASIOPAL_MASTERTLSSERVER_H
#define ASIOPAL_MASTERTLSSERVER_H

#include <openpal/logging/Logger.h>

#include <asiopal/tls/TLSServer.h>

#include <asiopal/TLSConfig.h>
#include <asiopal/IPEndpoint.h>
#include <asiopal/ResourceManager.h>

#include "asiodnp3/IListenCallbacks.h"

namespace asiodnp3
{

class MasterTLSServer final : public asiopal::TLSServer
{

public:

	MasterTLSServer(
	    const openpal::Logger& logger,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    const asiopal::IPEndpoint& endpoint,
	    const asiopal::TLSConfig& tlsConfig,
	    const std::shared_ptr<IListenCallbacks>& callbacks,
	    const std::shared_ptr<asiopal::ResourceManager>& manager,
	    std::error_code& ec
	);

	static std::shared_ptr<MasterTLSServer> Create(
	    const openpal::Logger& logger,
	    const std::shared_ptr<asiopal::Executor> executor,
	    const asiopal::IPEndpoint endpoint,
	    const asiopal::TLSConfig& tlsConfig,
	    const std::shared_ptr<IListenCallbacks> callbacks,
	    const std::shared_ptr<asiopal::ResourceManager>& manager,
	    std::error_code& ec)
	{
		auto ret = std::make_shared<MasterTLSServer>(logger, executor, endpoint, tlsConfig, callbacks, manager, ec);

		if (ec) return nullptr;

		ret->StartAccept(ec);

		if (ec)
		{
			return nullptr;
		}

		return ret;
	}

	virtual bool AcceptConnection(uint64_t sessionid, const asio::ip::tcp::endpoint& remote) override;

	virtual bool VerifyCallback(uint64_t sessionid, bool preverified, asio::ssl::verify_context& ctx) override;

	virtual void AcceptStream(
	    uint64_t sessionid,
	    const std::shared_ptr<asiopal::Executor>& executor,
	    std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> stream
	) override;

	virtual void OnShutdown() override;

private:

	std::shared_ptr<IListenCallbacks> callbacks;
	std::shared_ptr<asiopal::ResourceManager> manager;

	static std::string SessionIdToString(uint64_t sessionid);

};

}

#endif
