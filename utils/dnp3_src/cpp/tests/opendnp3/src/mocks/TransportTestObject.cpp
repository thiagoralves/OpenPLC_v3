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
#include "TransportTestObject.h"

#include <testlib/BufferHelpers.h>

#include <openpal/util/ToHex.h>
#include <opendnp3/app/AppConstants.h>

#include <memory>
#include <sstream>

using namespace std;
using namespace openpal;
using namespace testlib;

namespace opendnp3
{

TransportTestObject::TransportTestObject(bool openOnStart, uint32_t maxRxFragmentSize) :
	log(),
	exe(),
	transport(log.logger, maxRxFragmentSize)
{
	link.SetUpperLayer(transport);
	transport.SetLinkLayer(link);

	upper.SetLowerLayer(transport);
	transport.SetAppLayer(upper);

	if (openOnStart)
	{
		transport.OnLowerLayerUp();
	}
}

std::string TransportTestObject::GetData(const std::string& arHdr, uint8_t aSeed, uint32_t aLength)
{
	testlib::ByteStr buff(aLength);
	uint8_t val = aSeed;
	for(size_t i = 0; i < aLength; ++i)
	{
		buff[i] = val;
		++val;
	}

	ostringstream oss;
	if(arHdr.size() > 0) oss << arHdr << " ";
	oss << ToHex(buff, buff.Size(), true);
	return oss.str();
}

std::string TransportTestObject::GeneratePacketSequence(vector< std::string >& arVec, uint32_t aNumPackets, uint32_t aLastPacketLength)
{
	ostringstream oss;
	for(size_t i = 0; i < aNumPackets; ++i)
	{
		bool fir = i == 0;
		bool fin = i == (aNumPackets - 1);
		int seq = static_cast<int>(i % 64);
		uint32_t len = fin ? aLastPacketLength : MAX_TPDU_PAYLOAD;
		uint8_t hdr = TransportTx::GetHeader(fir, fin, seq);
		std::string data = this->GetData("", 0, len); //raw data with no header
		oss << ((i == 0) ? "" : " ") << data; //cache the data in the string stream
		arVec.push_back(ToHex(&hdr, 1, true) + " " + data);
	}

	return oss.str();
}

}

