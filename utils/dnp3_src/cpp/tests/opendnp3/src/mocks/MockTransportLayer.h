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
#ifndef __MOCK_LINK_LAYER_USER_H_
#define __MOCK_LINK_LAYER_USER_H_

#include <opendnp3/LayerInterfaces.h>
#include <opendnp3/link/ILinkLayer.h>

#include <deque>
#include <string>

namespace opendnp3
{

class MockTransportLayer : public IUpperLayer
{

public:

	struct State
	{

		State()
		{
			Reset();
		}

		uint32_t successCnt;
		uint32_t failureCnt;
		uint32_t numLayerUp;
		uint32_t numLayerDown;

		void Reset()
		{
			successCnt = failureCnt = numLayerUp = numLayerDown = 0;
		}
	};

	MockTransportLayer();

	void SetLinkLayer(ILinkLayer& linkLayer);

	bool SendDown(ITransportSegment& segments);

	bool IsOnline() const
	{
		return isOnline;
	}

	bool CountersEqual(uint32_t success, uint32_t failure)
	{
		return state.successCnt == success && state.failureCnt == failure;
	}

	bool StateEquals(const State& s)
	{
		return (state.successCnt == s.successCnt)
		       && (state.failureCnt == s.failureCnt)
		       && (state.numLayerUp == s.numLayerUp)
		       && (state.numLayerDown == s.numLayerDown);
	}

	void Reset()
	{
		state.Reset();
	}

	State GetState()
	{
		return state;
	}

	// these are the NVII delegates
	virtual bool OnReceive(const openpal::RSlice& buffer) override final;
	virtual bool OnSendResult(bool isSuccess) override final;
	virtual bool OnLowerLayerUp() override final;
	virtual bool OnLowerLayerDown() override final;

	std::deque<std::string> receivedQueue;

private:
	ILinkLayer* pLinkLayer;
	bool isOnline;
	State state;
};



}

#endif
