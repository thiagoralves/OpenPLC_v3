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
#include "LinkLayerTest.h"

#include <testlib/HexConversions.h>

using namespace testlib;
using namespace openpal;

namespace opendnp3
{

LinkLayerTest::LinkLayerTest(LinkConfig config) :
	log(),
	exe(std::make_shared<testlib::MockExecutor>()),
	listener(std::make_shared<MockLinkListener>()),
	upper(std::make_shared<MockTransportLayer>()),
	link(log.logger, exe, upper, listener, config),
	numTotalWrites(0)
{
	upper->SetLinkLayer(link);
	link.SetRouter(*this);
}

bool LinkLayerTest::OnFrame(LinkFunction func, bool isMaster, bool fcb, bool fcvdfc, uint16_t dest, uint16_t source, const openpal::RSlice& userdata)
{
	LinkHeaderFields fields(func, isMaster, fcb, fcvdfc, dest, source);
	return link.OnFrame(fields, userdata);
}

std::string LinkLayerTest::PopLastWriteAsHex()
{
	if (writeQueue.empty())
	{
		return "";
	}

	while (writeQueue.size() > 1)
	{
		writeQueue.pop_front();
	}

	auto ret = writeQueue.front();
	writeQueue.pop_front();
	return ret;
}

uint32_t LinkLayerTest::NumTotalWrites()
{
	return numTotalWrites;
}

void LinkLayerTest::BeginTransmit(const openpal::RSlice& buffer, ILinkSession&)
{
	++numTotalWrites;
	this->writeQueue.push_back(ToHex(buffer));
}

LinkConfig LinkLayerTest::DefaultConfig()
{
	return LinkConfig(true, false);
}

}

