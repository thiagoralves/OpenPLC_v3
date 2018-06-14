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
#ifndef OPENDNP3_TRANSPORTRX_H
#define OPENDNP3_TRANSPORTRX_H

#include "opendnp3/StackStatistics.h"
#include "opendnp3/transport/TransportConstants.h"
#include "opendnp3/transport/TransportSeqNum.h"

#include <openpal/container/RSlice.h>
#include <openpal/container/Buffer.h>
#include <openpal/logging/Logger.h>

namespace opendnp3
{

/**
State/validation for the DNP3 transport layer's receive channel.
*/
class TransportRx
{

public:
	TransportRx(const openpal::Logger&, uint32_t maxRxFragSize);

	openpal::RSlice ProcessReceive(const openpal::RSlice& input);

	void Reset();

	const StackStatistics::Transport::Rx& Statistics() const
	{
		return statistics;
	}

private:

	openpal::WSlice GetAvailable();

	void ClearRxBuffer();

	bool ValidateHeader(bool fir, uint8_t sequence);

	openpal::Logger logger;
	StackStatistics::Transport::Rx statistics;

	openpal::Buffer rxBuffer;
	uint32_t numBytesRead;

	TransportSeqNum sequence;
};

}

#endif

