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
#ifndef OPENDNP3_LINKLAYERPARSER_H
#define OPENDNP3_LINKLAYERPARSER_H


#include <openpal/container/WSlice.h>
#include <openpal/logging/Logger.h>

#include "opendnp3/link/ShiftableBuffer.h"
#include "opendnp3/link/LinkFrame.h"
#include "opendnp3/link/LinkHeader.h"
#include "opendnp3/link/LinkStatistics.h"
#include "opendnp3/link/IFrameSink.h"

namespace opendnp3
{

/// Parses FT3 frames
class LinkLayerParser
{
	enum class State
	{
		FindSync,
		ReadHeader,
		ReadBody,
		Complete
	};

public:

	/// @param logger_ Logger that the receiver is to use.
	/// @param pSink_ Completely parsed frames are sent to this interface
	LinkLayerParser(const openpal::Logger& logger);

	/// Called when valid data has been written to the current buffer write position
	/// Parses the new data and calls the specified frame sink
	/// @param numBytes Number of bytes written
	void OnRead(uint32_t numBytes, IFrameSink& sink);

	/// @return Buffer that can currently be used for writing
	openpal::WSlice WriteBuff() const;

	/// Resets the state of parser
	void Reset();

	const LinkStatistics::Parser& Statistics() const
	{
		return this->statistics;
	}

private:

	State ParseUntilComplete();
	State ParseOneStep();
	State ParseSync();
	State ParseHeader();
	State ParseBody();

	void PushFrame(IFrameSink& sink);

	bool ReadHeader();
	bool ValidateBody();
	bool ValidateHeaderParameters();
	bool ValidateFunctionCode();
	void FailFrame();

	void TransferUserData();

	openpal::Logger logger;
	LinkStatistics::Parser statistics;

	LinkHeader header;

	State state;
	uint32_t frameSize;
	openpal::RSlice userData;

	// buffer where received data is written
	uint8_t rxBuffer[LPDU_MAX_FRAME_SIZE];

	// facade over the rxBuffer that provides ability to "shift" as data is read
	ShiftableBuffer buffer;
};

}

#endif

