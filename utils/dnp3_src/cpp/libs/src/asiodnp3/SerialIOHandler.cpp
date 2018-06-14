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

#include "asiodnp3/SerialIOHandler.h"

#include "openpal/logging/LogMacros.h"
#include "opendnp3/LogLevels.h"

namespace asiodnp3
{

SerialIOHandler::SerialIOHandler(
    const openpal::Logger& logger,
    const std::shared_ptr<IChannelListener>& listener,
    const std::shared_ptr<asiopal::Executor>& executor,
    const asiopal::ChannelRetry& retry,
    const asiopal::SerialSettings& settings
) :
	IOHandler(logger, listener),
	executor(executor),
	retry(retry),
	settings(settings),
	retrytimer(*executor)
{}

void SerialIOHandler::ShutdownImpl()
{
	this->ResetState();
}

void SerialIOHandler::BeginChannelAccept()
{
	this->TryOpen(retry.minOpenRetry);
}

void SerialIOHandler::SuspendChannelAccept()
{
	this->retrytimer.Cancel();
}

void SerialIOHandler::OnChannelShutdown()
{
	this->BeginChannelAccept();
}

void SerialIOHandler::TryOpen(const openpal::TimeDuration& timeout)
{
	auto port = std::make_shared<asiopal::SerialChannel>(executor);

	std::error_code ec;
	port->Open(settings, ec);

	if (ec)
	{
		FORMAT_LOG_BLOCK(this->logger, openpal::logflags::WARN, "Error Connecting: %s", ec.message().c_str());

		++this->statistics.numOpenFail;

		auto callback = [this, timeout]()
		{
			this->TryOpen(this->retry.NextDelay(timeout));
		};

		this->retrytimer.Start(timeout, callback);
	}
	else
	{
		this->OnNewChannel(port);
	}
}

void SerialIOHandler::ResetState()
{
	retrytimer.Cancel();
}

}


