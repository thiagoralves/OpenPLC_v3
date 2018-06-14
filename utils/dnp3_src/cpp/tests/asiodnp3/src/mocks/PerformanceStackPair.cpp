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

#include "PerformanceStackPair.h"

#include "asiodnp3/DefaultMasterApplication.h"
#include "opendnp3/outstation/SimpleCommandHandler.h"

#include <sstream>
#include <iostream>
#include <exception>

using namespace opendnp3;

namespace asiodnp3
{

PerformanceStackPair::PerformanceStackPair(uint32_t levels, openpal::TimeDuration timeout, DNP3Manager& manager, uint16_t port, uint16_t numPointsPerType, uint32_t eventsPerIteration) :
	PORT(port),
	NUM_POINTS_PER_TYPE(numPointsPerType),
	EVENTS_PER_ITERATION(eventsPerIteration),
	soeHandler(std::make_shared<CountingSOEHandler>()),
	clientListener(std::make_shared<QueuedChannelListener>()),
	serverListener(std::make_shared<QueuedChannelListener>()),
	master(CreateMaster(levels, timeout, manager, port, this->soeHandler, this->clientListener)),
	outstation(CreateOutstation(levels, timeout, manager, port, numPointsPerType, 3 * eventsPerIteration, this->serverListener))
{
	this->outstation->Enable();
	this->master->Enable();
}

void PerformanceStackPair::WaitForChannelsOnline(std::chrono::steady_clock::duration timeout)
{
	if (!this->clientListener->WaitForState(ChannelState::OPEN, timeout))
	{
		throw std::runtime_error("client timed out before opening");
	}

	if (!this->serverListener->WaitForState(ChannelState::OPEN, timeout))
	{
		throw std::runtime_error("server timed out before opening");
	}
}

void PerformanceStackPair::SendValues()
{
	UpdateBuilder builder;
	for (uint32_t i = 0; i < EVENTS_PER_ITERATION; ++i)
	{
		AddValue(i, builder);
	}
	this->outstation->Apply(builder.Build());
}

void PerformanceStackPair::AddValue(uint32_t i, UpdateBuilder& builder)
{
	const uint16_t index = i % NUM_POINTS_PER_TYPE;

	switch (i % 7)
	{
	case(0):
		builder.Update(Binary(i % 2 == 0), index, EventMode::Force);
		break;
	case(1):
		builder.Update(DoubleBitBinary(i % 2 == 0 ? DoubleBit::DETERMINED_ON : DoubleBit::DETERMINED_OFF), index, EventMode::Force);
		break;
	case(2):
		builder.Update(Analog(i), index, EventMode::Force);
		break;
	case(3):
		builder.Update(Counter(i), index, EventMode::Force);
		break;
	case(4):
		builder.Update(FrozenCounter(i), index, EventMode::Force);
		break;
	case(5):
		builder.Update(BinaryOutputStatus(i % 2 == 0), index, EventMode::Force);
		break;
	default:
		builder.Update(AnalogOutputStatus(i), index, EventMode::Force);
		break;
	}
}

void PerformanceStackPair::WaitForValues(std::chrono::steady_clock::duration timeout)
{
	this->soeHandler->WaitForCount(this->EVENTS_PER_ITERATION, timeout);
}

OutstationStackConfig PerformanceStackPair::GetOutstationStackConfig(uint16_t numPointsPerType, uint16_t eventBufferSize, openpal::TimeDuration timeout)
{
	OutstationStackConfig config(DatabaseSizes::AllTypes(numPointsPerType));

	config.outstation.params.unsolConfirmTimeout = timeout;
	config.outstation.eventBufferConfig = EventBufferConfig::AllTypes(eventBufferSize);
	config.outstation.params.allowUnsolicited = true;

	return config;
}

MasterStackConfig PerformanceStackPair::GetMasterStackConfig(openpal::TimeDuration timeout)
{
	MasterStackConfig config;

	config.master.responseTimeout = timeout;
	config.master.taskRetryPeriod = timeout;
	config.master.disableUnsolOnStartup = false;
	config.master.startupIntegrityClassMask = ClassField::None();
	config.master.unsolClassMask = ClassField::AllEventClasses();

	return config;
}

std::shared_ptr<IMaster> PerformanceStackPair::CreateMaster(uint32_t levels, openpal::TimeDuration timeout, DNP3Manager& manager, uint16_t port, std::shared_ptr<ISOEHandler> soehandler, std::shared_ptr<IChannelListener> listener)
{
	auto channel = manager.AddTCPClient(
	                   GetId("client", port).c_str(),
	                   levels,
	                   asiopal::ChannelRetry::Default(),
	                   "127.0.0.1",
	                   "127.0.0.1",
	                   port,
	                   listener
	               );

	return channel->AddMaster(
	           GetId("master", port).c_str(),
	           soehandler,
	           DefaultMasterApplication::Create(),
	           GetMasterStackConfig(timeout)
	       );
}

std::shared_ptr<IOutstation> PerformanceStackPair::CreateOutstation(uint32_t levels, openpal::TimeDuration timeout, DNP3Manager& manager, uint16_t port, uint16_t numPointsPerType, uint16_t eventBufferSize, std::shared_ptr<IChannelListener> listener)
{
	auto channel = manager.AddTCPServer(
	                   GetId("server", port).c_str(),
	                   levels,
	                   asiopal::ChannelRetry::Default(),
	                   "127.0.0.1",
	                   port,
	                   listener
	               );

	return channel->AddOutstation(
	           GetId("outstation", port).c_str(),
	           SuccessCommandHandler::Create(),
	           DefaultOutstationApplication::Create(),
	           GetOutstationStackConfig(numPointsPerType, eventBufferSize, timeout)
	       );
}

std::string PerformanceStackPair::GetId(const char* name, uint16_t port)
{
	std::ostringstream oss;
	oss << name << ":" << port;
	return oss.str();
}
}



