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
#ifndef __MOCK_FRAME_SINK_H_
#define __MOCK_FRAME_SINK_H_

#include <opendnp3/link/ILinkSession.h>
#include <opendnp3/link/LinkLayerConstants.h>

#include <testlib/BufferTestObject.h>

#include <opendnp3/gen/LinkFunction.h>

#include <functional>
#include <queue>

namespace opendnp3
{


class MockFrameSink : public ILinkSession, public testlib::BufferTestObject
{
public:

	MockFrameSink();

	// ILinkSession members
	bool OnLowerLayerUp() override;
	bool OnLowerLayerDown() override;
	bool OnTransmitResult(bool success) override;

	virtual bool OnFrame(const LinkHeaderFields& header, const openpal::RSlice& userdata) override final;

	void Reset();


	bool CheckLast(LinkFunction aCode, bool aIsMaster, uint16_t aDest, uint16_t aSrc);
	bool CheckLastWithFCB(LinkFunction aCode, bool aIsMaster, bool aFcb, uint16_t aDest, uint16_t aSrc);
	bool CheckLastWithDFC(LinkFunction aCode, bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc);

	// Last frame information
	size_t m_num_frames;
	LinkHeaderFields m_last_header;

	bool mLowerOnline;

	// Add a function to execute the next time a frame is received
	// This allows us to test re-entrant behaviors
	void AddAction(std::function<void ()> fun);

private:

	// Executes one action, if one is available
	void ExecuteAction();

	std::deque< std::function<void ()> > m_actions;

	void Update(LinkFunction aCode, bool aIsMaster, uint16_t aSrc, uint16_t aDest);
};

}

#endif

