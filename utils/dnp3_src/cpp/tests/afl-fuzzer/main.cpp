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

#include <cstddef>
#include <cstdio>
#include <iostream>

#include <opendnp3/app/parsing/APDUParser.h>
#include <openpal/logging/LogRoot.h>

#include <opendnp3/outstation/OutstationContext.h>
#include <opendnp3/outstation/Outstation.h>
#include <opendnp3/outstation/ICommandHandler.h>
#include <opendnp3/outstation/SimpleCommandHandler.h>


using namespace openpal;
using namespace opendnp3;

class ConsoleLogger : public openpal::ILogHandler
{

public:
	virtual void Log(const openpal::LogEntry& entry) override final
	{
		//std::cout << entry.GetMessage() << std::endl;
	}
};

class Timer : public openpal::ITimer
{
	virtual void Cancel() override {}
	virtual MonotonicTimestamp ExpiresAt() override
	{
		return MonotonicTimestamp(0);
	}
};

class Executor : public openpal::IExecutor
{
public:

	Timer m_timer;

	Executor() : m_timer() {}
	virtual MonotonicTimestamp GetTime() override
	{
		return MonotonicTimestamp(0);
	}
	virtual ITimer* Start(const TimeDuration& duration, const Action0& action) override
	{
		return &m_timer;
	}
	virtual ITimer* Start(const MonotonicTimestamp& expiration, const Action0& action) override
	{
		return &m_timer;
	}
	virtual void Post(const Action0& action) override {}
};

class LowerLayer : public ILowerLayer
{
public:

	virtual void BeginTransmit(const openpal::ReadBufferView& buffer) override
	{
		fwrite(buffer, 1, buffer.Size(), stdout);
		fflush(stdout);
	}

};

int main(int argc, char* argv[])
{
	uint8_t buffer[4096];

	//try to read from stdin
	size_t num = fread(buffer, 1, 4096, stdin);

	ReadBufferView input(buffer, num);

	ConsoleLogger handler;
	LogRoot root(&handler, "log", ~0);
	auto logger = root.GetLogger();

	OutstationConfig config;
	config.params.allowUnsolicited = false;
	DatabaseTemplate db = DatabaseTemplate::AllTypes(10);

	Executor exe;
	LowerLayer lower;

	OContext context(config, db, logger, exe, lower, SuccessCommandHandler::Instance(), DefaultOutstationApplication::Instance());

	Outstation outstation(context);

	outstation.OnLowerLayerUp();

	outstation.OnReceive(ReadBufferView(buffer, num));
	/*

	std::cout << "read: " << num << std::endl;

	ReadBufferView view(buffer, static_cast<uint32_t>(num));


	APDUParser::ParseAndLogAll(view, &logger);
	*/
}


