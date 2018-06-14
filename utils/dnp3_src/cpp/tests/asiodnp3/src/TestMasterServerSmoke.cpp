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
#include "catch.hpp"

#include "asiodnp3/DNP3Manager.h"
#include "asiodnp3/DefaultMasterApplication.h"
#include "asiodnp3/DefaultListenCallbacks.h"

#include "opendnp3/LogLevels.h"
#include "opendnp3/outstation/SimpleCommandHandler.h"
#include "opendnp3/outstation/IOutstationApplication.h"
#include "opendnp3/master/ISOEHandler.h"

#include "asiopal/UTCTimeSource.h"
#include "asiodnp3/ConsoleLogger.h"
#include "dnp3mocks/NullSOEHandler.h"

#include <thread>

using namespace opendnp3;
using namespace asiodnp3;
using namespace asiopal;
using namespace openpal;

#define SUITE(name) "MasterServerSmokeTestSuite - " name

struct TestComponents
{
	TestComponents(DNP3Manager& manager, int numOutstations, std::error_code& ec) :
		listener(manager.CreateListener("listener", levels::ALL, IPEndpoint::Localhost(20000), std::make_shared<DefaultListenCallbacks>(), ec))
	{
		for(int i = 0; i < numOutstations; ++i)
		{
			auto channel = manager.AddTCPClient("client", levels::ALL, ChannelRetry::Default(), "127.0.0.1", "", 20000, nullptr);
			auto outstation = channel->AddOutstation("outstation", SuccessCommandHandler::Create(), DefaultOutstationApplication::Create(), GetConfig());

			this->channels.push_back(channel);
			this->outstations.push_back(outstation);
		}
	}

	void Enable()
	{
		for(auto& outstation : outstations)
		{
			outstation->Enable();
		}
	}

	static OutstationStackConfig GetConfig()
	{
		OutstationStackConfig config(DatabaseSizes::Empty());
		config.outstation.params.allowUnsolicited = true;
		return config;
	}

	std::shared_ptr<IListener> listener;
	std::vector<std::shared_ptr<IChannel>> channels;
	std::vector<std::shared_ptr<IOutstation>> outstations;
};

const int ITERATIONS = 5;
const int NUM_OUTSTATIONS = 5;

TEST_CASE(SUITE("ConstructionDestruction"))
{
	for(int i = 0; i < ITERATIONS; ++i)
	{
		DNP3Manager manager(std::thread::hardware_concurrency());
		std::error_code ec;
		TestComponents components(manager, NUM_OUTSTATIONS, ec);
		if (ec) throw std::logic_error(ec.message());
		components.Enable();
	}
}





