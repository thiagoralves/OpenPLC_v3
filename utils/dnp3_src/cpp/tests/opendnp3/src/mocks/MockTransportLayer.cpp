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
#include "MockTransportLayer.h"

#include <testlib/BufferHelpers.h>
#include <testlib/HexConversions.h>

#include <openpal/util/ToHex.h>

using namespace openpal;
using namespace testlib;

namespace opendnp3
{

MockTransportLayer::MockTransportLayer() : pLinkLayer(nullptr), isOnline(false)
{}

void MockTransportLayer::SetLinkLayer(ILinkLayer& linkLayer)
{
	this->pLinkLayer = &linkLayer;
}

bool MockTransportLayer::SendDown(ITransportSegment& segments)
{
	return pLinkLayer->Send(segments);
}

bool MockTransportLayer::OnReceive(const openpal::RSlice& buffer)
{
	receivedQueue.push_back(ToHex(buffer));
	return true;
}

bool MockTransportLayer::OnSendResult(bool isSuccess)
{
	if (isSuccess)
	{
		++state.successCnt;
	}
	else
	{
		++state.failureCnt;
	}

	return true;
}

bool MockTransportLayer::OnLowerLayerUp()
{
	assert(!isOnline);
	isOnline = true;
	++state.numLayerUp;
	return true;
}

bool MockTransportLayer::OnLowerLayerDown()
{
	assert(isOnline);
	isOnline = false;
	++state.numLayerDown;
	return true;
}

}

