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
#ifndef __MOCK_LINK_LAYER_H_
#define __MOCK_LINK_LAYER_H_

#include <vector>

#include <openpal/util/ToHex.h>
#include <opendnp3/link/ILinkLayer.h>

#include <testlib/BufferHelpers.h>
#include <testlib/HexConversions.h>

namespace opendnp3
{

class MockLinkLayer : public ILinkLayer, public HasUpperLayer
{

public:

	virtual bool Send(ITransportSegment& segments) override final
	{
		while (segments.HasValue())
		{
			sends.push_back(testlib::ToHex(segments.GetSegment()));
			segments.Advance();
		}

		return true;
	}

	std::string PopWriteAsHex()
	{
		assert(!sends.empty());
		auto ret = sends.front();
		sends.erase(sends.begin());
		return ret;
	}

	bool SendUp(const std::string& hex)
	{
		testlib::HexSequence hs(hex);
		if (pUpperLayer)
		{
			auto buffer = hs.ToRSlice();
			return pUpperLayer->OnReceive(buffer);
		}
		return false;
	}

	std::vector<std::string> sends;
};

}

#endif
