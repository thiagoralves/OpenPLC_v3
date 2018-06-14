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

#include "asiodnp3/LinkSession.h"

#include <openpal/logging/LogMacros.h>

#include <opendnp3/LogLevels.h>

using namespace openpal;
using namespace asiopal;
using namespace opendnp3;

namespace asiodnp3
{

LinkSession::LinkSession(
    const openpal::Logger& logger,
    uint64_t sessionid,
    const std::shared_ptr<IResourceManager>& manager,
    const std::shared_ptr<IListenCallbacks>& callbacks,
    const std::shared_ptr<asiopal::IAsyncChannel>& channel) :
	logger(logger),
	session_id(sessionid),
	manager(manager),
	callbacks(callbacks),
	channel(channel),
	parser(logger),
	first_frame_timer(*channel->executor)
{

}

void LinkSession::Shutdown()
{
	auto shutdown = [self = shared_from_this()]()
	{
		self->ShutdownImpl();
	};

	this->channel->executor->BlockUntilAndFlush(shutdown);
}

void LinkSession::ShutdownImpl()
{
	if(this->is_shutdown) return;

	this->is_shutdown = true;

	this->callbacks->OnConnectionClose(this->session_id, this->stack);

	if (this->stack)
	{
		this->stack->OnLowerLayerDown();
	}

	this->first_frame_timer.Cancel();

	this->channel->Shutdown();

	auto detach = [self = shared_from_this()]()
	{
		self->manager->Detach(self);
	};

	this->channel->executor->strand.post(detach);
}

void LinkSession::SetLogFilters(openpal::LogFilters filters)
{
	this->logger.SetFilters(filters);
}

void LinkSession::OnReadComplete(const std::error_code& ec, size_t num)
{
	if (ec)
	{
		SIMPLE_LOG_BLOCK(this->logger, flags::WARN, ec.message().c_str());
		this->ShutdownImpl();
	}
	else
	{
		this->parser.OnRead(static_cast<uint32_t>(num), *this);
		this->BeginReceive();
	}
}

void LinkSession::OnWriteComplete(const std::error_code& ec, size_t num)
{
	if (ec)
	{
		SIMPLE_LOG_BLOCK(this->logger, flags::WARN, ec.message().c_str());
		this->ShutdownImpl();
	}
	else
	{
		this->stack->OnTransmitComplete(true);
	}
}

void LinkSession::BeginTransmit(const openpal::RSlice& buffer, opendnp3::ILinkSession& session)
{
	this->channel->BeginWrite(buffer);
}

bool LinkSession::OnFrame(const LinkHeaderFields& header, const openpal::RSlice& userdata)
{
	if (this->stack)
	{
		this->stack->OnFrame(header, userdata);
	}
	else
	{
		this->first_frame_timer.Cancel();

		this->callbacks->OnFirstFrame(this->session_id, header, *this);

		if (this->stack)
		{
			this->stack->OnLowerLayerUp();

			// push the frame into the newly created stack
			this->stack->OnFrame(header, userdata);
		}
		else
		{
			SIMPLE_LOG_BLOCK(this->logger, flags::WARN, "No master created. Closing socket.");
			this->ShutdownImpl();
		}
	}

	return true;
}

std::shared_ptr<IMasterSession> LinkSession::AcceptSession(
    const std::string& loggerid,
    std::shared_ptr<opendnp3::ISOEHandler> SOEHandler,
    std::shared_ptr<opendnp3::IMasterApplication> application,
    const MasterStackConfig& config)
{
	if (this->stack)
	{
		SIMPLE_LOG_BLOCK(this->logger, flags::ERR, "SocketSession already has a master bound");
		return nullptr;
	}

	// rename the logger id to something meaningful
	this->logger.Rename(loggerid);

	this->stack = MasterSessionStack::Create(
	                  this->logger,
	                  this->channel->executor,
	                  SOEHandler,
	                  application,
	                  shared_from_this(),
	                  *this,
	                  config
	              );

	return stack;
}

void LinkSession::Start()
{
	this->channel->SetCallbacks(shared_from_this());

	auto timeout = [self = shared_from_this()]()
	{
		SIMPLE_LOG_BLOCK(self->logger, flags::ERR, "Timed out before receving a frame. Closing socket.");
		self->channel->Shutdown();
	};

	this->first_frame_timer.Start(this->callbacks->GetFirstFrameTimeout(), timeout);

	this->BeginReceive();
}

void LinkSession::BeginReceive()
{
	auto dest = parser.WriteBuff();
	channel->BeginRead(dest);
}

}
