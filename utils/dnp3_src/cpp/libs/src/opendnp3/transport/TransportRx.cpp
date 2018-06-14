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
#include "TransportRx.h"
#include "TransportConstants.h"
#include "opendnp3/LogLevels.h"

#include <openpal/logging/LogMacros.h>
#include <openpal/util/ToHex.h>

#include <cstring>

using namespace std;
using namespace openpal;

namespace opendnp3
{

TransportRx::TransportRx(const Logger& logger, uint32_t maxRxFragSize) :
	logger(logger),
	rxBuffer(maxRxFragSize),
	numBytesRead(0)
{

}

void TransportRx::Reset()
{
	sequence.Reset();
	this->ClearRxBuffer();
}

void TransportRx::ClearRxBuffer()
{
	numBytesRead = 0;
}

openpal::WSlice TransportRx::GetAvailable()
{
	return rxBuffer.GetWSlice().Skip(numBytesRead);
}

RSlice TransportRx::ProcessReceive(const RSlice& input)
{
	++statistics.numTransportRx;

	if (input.IsEmpty())
	{
		FORMAT_LOG_BLOCK(logger, flags::WARN, "Received tpdu with no header");
		++statistics.numTransportErrorRx;
		return RSlice::Empty();
	}

	const uint8_t HDR = input[0];
	const bool FIR = (HDR & TL_HDR_FIR) != 0;
	const bool FIN = (HDR & TL_HDR_FIN) != 0;
	const int SEQ = HDR & TL_HDR_SEQ;

	auto payload = input.Skip(1);

	FORMAT_LOG_BLOCK(logger, flags::TRANSPORT_RX, "FIR: %d FIN: %d SEQ: %u LEN: %u", FIR, FIN, SEQ, payload.Size());

	if (!this->ValidateHeader(FIR, SEQ))
	{
		++statistics.numTransportErrorRx;
		return RSlice::Empty();
	}

	auto available = this->GetAvailable();

	if (payload.Size() > available.Size())
	{
		++statistics.numTransportBufferOverflow;
		SIMPLE_LOG_BLOCK(logger, flags::WARN, "Exceeded the buffer size before a complete fragment was read");
		this->ClearRxBuffer();
		return RSlice::Empty();
	}

	payload.CopyTo(available);

	this->numBytesRead += payload.Size();
	this->sequence.Increment();

	if(FIN)
	{
		RSlice ret = rxBuffer.ToRSlice().Take(numBytesRead);
		this->ClearRxBuffer();
		return ret;
	}
	else
	{
		return RSlice::Empty();
	}
}

bool TransportRx::ValidateHeader(bool fir, uint8_t sequence_)
{
	if(fir)
	{
		sequence = sequence_; //always accept the sequence on FIR
		if (numBytesRead > 0)
		{
			++statistics.numTransportDiscard;
			// drop existing received bytes from segment
			SIMPLE_LOG_BLOCK(logger, flags::WARN, "FIR received mid-fragment, discarding previous bytes");
			numBytesRead = 0;
		}
		return true;
	}

	if(numBytesRead == 0)   //non-first packet with 0 prior bytes
	{
		++statistics.numTransportIgnore;
		SIMPLE_LOG_BLOCK(logger, flags::WARN, "non-FIR packet with 0 prior bytes");
		return false;
	}

	if(sequence_ != sequence)
	{
		++statistics.numTransportIgnore;
		FORMAT_LOG_BLOCK(logger, flags::WARN, "Ignoring bad sequence, got %u, expected %u", sequence_, sequence.Get());
		return false;
	}

	return true;
}

}

