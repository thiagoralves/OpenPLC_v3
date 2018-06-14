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
#include "MockUpperLayer.h"

#include <testlib/BufferHelpers.h>

#include <openpal/util/ToHex.h>
#include <openpal/logging/LogMacros.h>

#include <memory>

using namespace openpal;
using namespace testlib;

namespace opendnp3
{

MockUpperLayer::MockUpperLayer() : isOnline(false)
{

}

bool MockUpperLayer::OnReceive(const openpal::RSlice& input)
{
	this->WriteToBuffer(input);

	if (mOnReceiveHandler)
	{
		mOnReceiveHandler(input);
	}

	return true;
}

bool MockUpperLayer::OnSendResult(bool isSuccess)
{
	if (isSuccess)
	{
		++mState.mSuccessCnt;
	}
	else
	{
		++mState.mFailureCnt;
	}

	return true;
}

bool MockUpperLayer::OnLowerLayerUp()
{
	isOnline = true;
	++mState.mNumLayerUp;
	return true;
}

bool MockUpperLayer::OnLowerLayerDown()
{
	isOnline = false;
	++mState.mNumLayerDown;
	return true;
}

bool MockUpperLayer::SendDown(const openpal::RSlice& buffer)
{
	return this->pLowerLayer ? pLowerLayer->BeginTransmit(buffer) : false;
}

bool MockUpperLayer::SendDown(const std::string& hex)
{
	HexSequence hs(hex);
	return this->SendDown(hs.ToRSlice());
}

}
