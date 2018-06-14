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
#ifndef __MOCK_UPPER_LAYER_H_
#define __MOCK_UPPER_LAYER_H_


#include <opendnp3/LayerInterfaces.h>

#include <functional>

#include <testlib/BufferTestObject.h>

namespace opendnp3
{

class MockUpperLayer : public IUpperLayer, public HasLowerLayer, public testlib::BufferTestObject
{
public:

	typedef std::function<void (const openpal::RSlice&)> OnReceiveHandler;

	struct State
	{

		State()
		{
			Reset();
		}

		size_t mSuccessCnt;
		size_t mFailureCnt;
		size_t mNumLayerUp;
		size_t mNumLayerDown;

		void Reset()
		{
			mSuccessCnt = mFailureCnt = mNumLayerUp = mNumLayerDown = 0;
		}
	};

	MockUpperLayer();

	bool IsOnline() const
	{
		return isOnline;
	}

	bool SendDown(const std::string&);
	bool SendDown(const openpal::RSlice& arBuffer);

	bool CountersEqual(size_t success, size_t failure)
	{
		return mState.mSuccessCnt == success && mState.mFailureCnt == failure;
	}

	bool StateEquals(const State& s)
	{
		return (mState.mSuccessCnt == s.mSuccessCnt)
		       && (mState.mFailureCnt == s.mFailureCnt)
		       && (mState.mNumLayerUp == s.mNumLayerUp)
		       && (mState.mNumLayerDown == s.mNumLayerDown);
	}

	void Reset()
	{
		mState.Reset();
	}
	State GetState()
	{
		return mState;
	}

	void SetReceiveHandler(const OnReceiveHandler& arHandler)
	{
		mOnReceiveHandler = arHandler;
	}

	//these are the NVII delegates
	virtual bool OnReceive(const openpal::RSlice& buffer) override final;
	virtual bool OnSendResult(bool isSuccess) override final;
	virtual bool OnLowerLayerUp() override final;
	virtual bool OnLowerLayerDown() override final;

private:

	bool isOnline;

	OnReceiveHandler mOnReceiveHandler;
	State mState;


};



}

#endif
