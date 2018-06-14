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
#include "MockLowerLayer.h"

#include <testlib/BufferHelpers.h>
#include <testlib/HexConversions.h>

#include <assert.h>

using namespace openpal;
using namespace testlib;

namespace opendnp3
{

bool MockLowerLayer::HasNoData() const
{
	return sendQueue.empty();
}

size_t MockLowerLayer::NumWrites() const
{
	return sendQueue.size();
}

openpal::RSlice MockLowerLayer::PopWrite()
{
	auto ret = sendQueue.front();
	sendQueue.pop();
	return ret;
}

std::string MockLowerLayer::PopWriteAsHex()
{
	if (sendQueue.empty())
	{
		return "";
	}
	else
	{
		auto ret = sendQueue.front();
		sendQueue.pop();
		return ToHex(ret);
	}
}

bool MockLowerLayer::BeginTransmit(const openpal::RSlice& output)
{
	this->sendQueue.push(output);
	return true;
}

void MockLowerLayer::SendUp(const openpal::RSlice& arBuffer)
{
	if(pUpperLayer)
	{
		pUpperLayer->OnReceive(arBuffer);
	}
}

void MockLowerLayer::SendUp(const std::string& arHexData)
{
	HexSequence hs(arHexData);
	this->SendUp(hs.ToRSlice());
}

void MockLowerLayer::SendSuccess()
{
	if (pUpperLayer)
	{
		pUpperLayer->OnSendResult(true);
	}
}

void MockLowerLayer::SendFailure()
{
	if (pUpperLayer)
	{
		pUpperLayer->OnSendResult(false);
	}
}

void MockLowerLayer::ThisLayerUp()
{
	if (pUpperLayer)
	{
		pUpperLayer->OnLowerLayerUp();
	}
}
void MockLowerLayer::ThisLayerDown()
{
	if (pUpperLayer)
	{
		pUpperLayer->OnLowerLayerDown();
	}
}

}
