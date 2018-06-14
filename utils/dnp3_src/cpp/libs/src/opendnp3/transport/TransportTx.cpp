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
#include "TransportTx.h"

#include "opendnp3/transport/TransportLayer.h"
#include "opendnp3/LogLevels.h"

#include <openpal/logging/LogMacros.h>

#include <assert.h>

using namespace std;
using namespace openpal;

namespace opendnp3
{

TransportTx::TransportTx(const openpal::Logger& logger) : logger(logger)
{}

void TransportTx::Configure(const openpal::RSlice& output)
{
	assert(output.IsNotEmpty());
	txSegment.Clear();
	this->apdu = output;
	this->tpduCount = 0;
}

bool TransportTx::HasValue() const
{
	return apdu.Size() > 0;
}

openpal::RSlice TransportTx::GetSegment()
{
	if (txSegment.IsSet())
	{
		return txSegment.Get();
	}
	else
	{
		uint32_t numToSend = (apdu.Size() < MAX_TPDU_PAYLOAD) ? apdu.Size() : MAX_TPDU_PAYLOAD;

		auto dest = tpduBuffer.GetWSlice().Skip(1);
		apdu.Take(numToSend).CopyTo(dest);

		bool fir = (tpduCount == 0);
		bool fin = (numToSend == apdu.Size());
		tpduBuffer()[0] = GetHeader(fir, fin, sequence);

		FORMAT_LOG_BLOCK(logger, flags::TRANSPORT_TX, "FIR: %d FIN: %d SEQ: %u LEN: %u", fir, fin, sequence.Get(), numToSend);

		++statistics.numTransportTx;

		auto segment = tpduBuffer.ToRSlice(numToSend + 1);
		txSegment.Set(segment);
		return segment;
	}

}

bool TransportTx::Advance()
{
	txSegment.Clear();
	uint32_t numToSend = apdu.Size() < MAX_TPDU_PAYLOAD ? apdu.Size() : MAX_TPDU_PAYLOAD;
	apdu.Advance(numToSend);
	++tpduCount;
	sequence.Increment();
	return apdu.IsNotEmpty();
}

uint8_t TransportTx::GetHeader(bool fir, bool fin, uint8_t sequence)
{
	uint8_t hdr = 0;

	if (fir)
	{
		hdr |= TL_HDR_FIR;
	}

	if (fin)
	{
		hdr |= TL_HDR_FIN;
	}

	// Only the lower 6 bits of the sequence number
	hdr |= TL_HDR_SEQ & sequence;

	return hdr;
}

}

