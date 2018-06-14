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
#ifndef __TRANSPORT_TEST_OBJECT_H_
#define __TRANSPORT_TEST_OBJECT_H_

#include "opendnp3/transport/TransportLayer.h"
#include "opendnp3/app/AppConstants.h"
#include "opendnp3/LogLevels.h"

#include "MockLinkLayer.h"
#include "dnp3mocks/MockUpperLayer.h"

#include "testlib/MockExecutor.h"
#include "testlib/MockLogHandler.h"

#include <vector>
#include <string>

namespace opendnp3
{

class TransportTestObject
{
public:
	TransportTestObject(bool openOnStart = false, uint32_t maxRxFragmentSize = DEFAULT_MAX_APDU_SIZE);

	// Generate a complete packet sequence inside the vector and
	// return the corresponding reassembled APDU
	std::string GeneratePacketSequence(std::vector<std::string>&, uint32_t aNumPackets, uint32_t aLastPacketLength);

	// Get a Sequence of data w/ optional header
	std::string GetData(const std::string& arHdr, uint8_t aSeed = 0, uint32_t aLength = MAX_TPDU_PAYLOAD);

	testlib::MockLogHandler log;
	testlib::MockExecutor exe;
	TransportLayer transport;
	MockLinkLayer link;
	MockUpperLayer upper;
};

}

#endif

