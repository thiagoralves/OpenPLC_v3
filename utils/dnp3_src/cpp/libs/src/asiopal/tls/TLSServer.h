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
#ifndef ASIOPAL_TLSSERVER_H
#define ASIOPAL_TLSSERVER_H

#include "asiopal/IPEndpoint.h"
#include "asiopal/IListener.h"
#include "asiopal/TLSConfig.h"
#include "asiopal/Executor.h"

#include <openpal/util/Uncopyable.h>
#include <openpal/logging/Logger.h>

#include "asiopal/tls/SSLContext.h"

namespace asiopal
{
/**
* Binds and listens on an IPv4 TCP port. Implement TLS negotiation.
*
* Meant to be used exclusively as a shared_ptr
*/
class TLSServer :
	public IListener,
	public std::enable_shared_from_this<TLSServer>,
	private openpal::Uncopyable
{

public:

	TLSServer(
	    const openpal::Logger& logger,
	    const std::shared_ptr<Executor>& executor,
	    const IPEndpoint& endpoint,
	    const TLSConfig& config,
	    std::error_code& ec
	);

	/// Stop listening for connections, permanently shutting down the listener
	void Shutdown() override;

protected:

	// Inherited classes must implement these methods

	virtual bool AcceptConnection(uint64_t sessionid, const asio::ip::tcp::endpoint& remote) = 0;
	virtual bool VerifyCallback(uint64_t sessionid, bool preverified, asio::ssl::verify_context& ctx) = 0;
	virtual void AcceptStream(uint64_t sessionid, const std::shared_ptr<Executor>& executor, std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> stream) = 0;
	virtual void OnShutdown() = 0;

	void StartAccept(std::error_code& ec);

	openpal::Logger logger;
	std::shared_ptr<Executor> executor;

private:

	std::error_code ConfigureContext(const TLSConfig& config, std::error_code& ec);
	std::error_code ConfigureListener(const std::string& adapter, std::error_code& ec);

	SSLContext ctx;
	asio::ip::tcp::endpoint endpoint;
	asio::ip::tcp::acceptor acceptor;

	uint64_t session_id;
};

}

#endif
