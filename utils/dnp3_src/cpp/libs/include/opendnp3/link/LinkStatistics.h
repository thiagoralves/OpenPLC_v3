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
#ifndef OPENDNP3_LINKSTATISTICS_H
#define OPENDNP3_LINKSTATISTICS_H

#include <cstdint>

namespace opendnp3
{

/**
* Counters for the channel and the DNP3 link layer
*/
struct LinkStatistics
{
	struct Parser
	{
		/// Number of frames discarded due to header CRC errors
		uint32_t numHeaderCrcError = 0;

		/// Number of frames discarded due to body CRC errors
		uint32_t numBodyCrcError = 0;

		/// Number of frames received
		uint32_t numLinkFrameRx = 0;

		/// number of bad LEN fields received (malformed frame)
		uint32_t numBadLength = 0;

		/// number of bad function codes (malformed frame)
		uint32_t numBadFunctionCode = 0;

		/// number of FCV / function code mismatches (malformed frame)
		uint32_t numBadFCV = 0;

		/// number of frames w/ unexpected FCB bit set (malformed frame)
		uint32_t numBadFCB = 0;
	};

	struct Channel
	{
		/// The number of times the channel has successfully opened
		uint32_t numOpen = 0;

		/// The number of times the channel has failed to open
		uint32_t numOpenFail = 0;

		/// The number of times the channel has closed either due to user intervention or an error
		uint32_t numClose = 0;

		/// The number of bytes received
		uint32_t numBytesRx = 0;

		/// The number of bytes transmitted
		uint32_t numBytesTx = 0;

		/// Number of frames transmitted
		uint32_t numLinkFrameTx = 0;
	};

	LinkStatistics() = default;

	LinkStatistics(const Channel& channel, const Parser& parser) : channel(channel), parser(parser)
	{}

	/// statistics for the communicaiton channel
	Channel channel;

	/// statistics for the link parser
	Parser parser;

};

}

#endif
