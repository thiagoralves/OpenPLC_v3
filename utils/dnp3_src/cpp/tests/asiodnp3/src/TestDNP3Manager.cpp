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
#include <catch.hpp>

#include <asiodnp3/DNP3Manager.h>
#include <asiodnp3/DefaultMasterApplication.h>

#include <opendnp3/LogLevels.h>
#include <opendnp3/outstation/SimpleCommandHandler.h>
#include <opendnp3/outstation/IOutstationApplication.h>
#include <opendnp3/master/ISOEHandler.h>

#include <asiopal/UTCTimeSource.h>
#include <asiodnp3/ConsoleLogger.h>

#include <dnp3mocks/NullSOEHandler.h>

#include <thread>
#include <iostream>

using namespace opendnp3;
using namespace asiodnp3;
using namespace asiopal;
using namespace openpal;

#define SUITE(name) "DNP3ManagerTestSuite - " name

const int ITERATIONS = 100;

struct Channels
{
	Channels(DNP3Manager& manager) :
		client(manager.AddTCPClient("client", levels::ALL, ChannelRetry::Default(), "127.0.0.1", "", 20000, nullptr)),
		server(manager.AddTCPServer("server", levels::ALL, ChannelRetry::Default(), "0.0.0.0", 20000, nullptr))
	{

	}

	std::shared_ptr<IChannel> client;
	std::shared_ptr<IChannel> server;
};

struct Components : Channels
{
	Components(DNP3Manager& manager) :
		Channels(manager),
		outstation(server->AddOutstation("outstation", SuccessCommandHandler::Create(), DefaultOutstationApplication::Create(), OutstationStackConfig(DatabaseSizes::Empty()))),
		master(client->AddMaster("master", NullSOEHandler::Create(), asiodnp3::DefaultMasterApplication::Create(), MasterStackConfig()))
	{

	}

	void Enable()
	{
		outstation->Enable();
		master->Enable();
	}


	std::shared_ptr<IOutstation> outstation;
	std::shared_ptr<IMaster> master;
};



TEST_CASE(SUITE("ConstructionDestruction"))
{
	for(int i = 0; i < ITERATIONS; ++i)
	{
		DNP3Manager manager(std::thread::hardware_concurrency());
		Components components(manager);
		components.Enable();
	}
}

TEST_CASE(SUITE("ManualStackShutdown"))
{
	for(int i = 0; i < ITERATIONS; ++i)
	{
		DNP3Manager manager(std::thread::hardware_concurrency());
		Components components(manager);
		components.Enable();

		// manually shutdown the stacks prior automatic cleanup
		components.outstation->Shutdown();
		components.master->Shutdown();
	}

}

TEST_CASE(SUITE("ManualChannelShutdownWithStacks"))
{
	for(int i = 0; i < ITERATIONS; ++i)
	{
		DNP3Manager manager(std::thread::hardware_concurrency());
		Components components(manager);
		components.Enable();

		// manually shutdown the channels prior automatic cleanup
		components.client->Shutdown();
		components.server->Shutdown();
	}
}

TEST_CASE(SUITE("ManualChannelShutdown"))
{
	for(int i = 0; i < ITERATIONS; ++i)
	{
		DNP3Manager manager(std::thread::hardware_concurrency());
		Channels channels(manager);

		channels.client->Shutdown();
		channels.server->Shutdown();
	}
}





