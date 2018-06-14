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

#include "asiodnp3/tls/MasterTLSServer.h"

#include "asiodnp3/LinkSession.h"

#include "asiopal/tls/TLSStreamChannel.h"

#include <openpal/logging/LogMacros.h>
#include <opendnp3/LogLevels.h>

using namespace openpal;
using namespace asiopal;
using namespace opendnp3;

namespace asiodnp3
{

MasterTLSServer::MasterTLSServer(
    const openpal::Logger& logger,
    const std::shared_ptr<asiopal::Executor>& executor,
    const asiopal::IPEndpoint& endpoint,
    const asiopal::TLSConfig& config,
    const std::shared_ptr<IListenCallbacks>& callbacks,
    const std::shared_ptr<asiopal::ResourceManager>& manager,
    std::error_code& ec
) :
	TLSServer(logger, executor, endpoint, config, ec),
	callbacks(callbacks),
	manager(manager)
{

}

bool MasterTLSServer::AcceptConnection(uint64_t sessionid, const asio::ip::tcp::endpoint& remote)
{
	std::ostringstream oss;
	oss << remote;

	if (this->callbacks->AcceptConnection(sessionid, remote.address().to_string()))
	{
		FORMAT_LOG_BLOCK(this->logger, flags::INFO, "Accepted connection from: %s", oss.str().c_str());
		return true;
	}
	else
	{
		FORMAT_LOG_BLOCK(this->logger, flags::INFO, "Rejected connection from: %s", oss.str().c_str());
		return false;
	}
}

bool MasterTLSServer::VerifyCallback(uint64_t sessionid, bool preverified, asio::ssl::verify_context& ctx)
{
	const int MAX_SUBJECT_NAME = 512;
	int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());

	// lookup the subject name
	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	char subjectName[MAX_SUBJECT_NAME];
	X509_NAME_oneline(X509_get_subject_name(cert), subjectName, MAX_SUBJECT_NAME);

	X509Info info(
	    depth,
	    RSlice(cert->sha1_hash, SHA_DIGEST_LENGTH), // the thumbprint
	    std::string(subjectName)
	);

	if (!preverified)
	{
		int err = X509_STORE_CTX_get_error(ctx.native_handle());

		FORMAT_LOG_BLOCK(this->logger, flags::WARN, "Error verifying certificate at depth: %d subject: %s error: %d:%s", depth, subjectName, err, X509_verify_cert_error_string(err));

		this->callbacks->OnCertificateError(sessionid, info, err);

		return preverified;
	}

	FORMAT_LOG_BLOCK(this->logger, flags::INFO, "Verified certificate at depth: %d subject: %s", depth, subjectName);

	return this->callbacks->AcceptCertificate(sessionid, info);
}

void MasterTLSServer::AcceptStream(uint64_t sessionid, const std::shared_ptr<Executor>& executor, std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> stream)
{
	auto channel = TLSStreamChannel::Create(executor->Fork(), stream); 	// run the link session in a new strand

	auto create = [&]() -> std::shared_ptr<LinkSession>
	{
		return LinkSession::Create(
		    this->logger.Detach(SessionIdToString(sessionid)),
		    sessionid,
		    this->manager,
		    callbacks,
		    channel
		);
	};

	if (!this->manager->Bind<LinkSession>(create))
	{
		channel->Shutdown();
	}
}

void MasterTLSServer::OnShutdown()
{
	this->manager->Detach(this->shared_from_this());
}

std::string MasterTLSServer::SessionIdToString(uint64_t sessionid)
{
	std::ostringstream oss;
	oss << "session-" << sessionid;
	return oss.str();
}

}

