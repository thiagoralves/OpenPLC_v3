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
#ifndef OPENDNP3_LINKFRAME_H
#define OPENDNP3_LINKFRAME_H

#include "opendnp3/gen/FunctionCode.h"
#include "opendnp3/gen/LinkFunction.h"

#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include <openpal/util/Uncopyable.h>
#include <openpal/logging/Logger.h>

namespace opendnp3
{

class LinkFrame : private openpal::StaticOnly
{

public:

	////////////////////////////////////////////////
	//	Functions for formatting outgoing Sec to Pri frames
	////////////////////////////////////////////////

	static openpal::RSlice FormatAck(openpal::WSlice& output, bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger);
	static openpal::RSlice FormatNack(openpal::WSlice& output, bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger);
	static openpal::RSlice FormatLinkStatus(openpal::WSlice& output, bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger);
	static openpal::RSlice FormatNotSupported(openpal::WSlice& output, bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger);

	////////////////////////////////////////////////
	//	Functions for formatting outgoing Pri to Sec frames
	////////////////////////////////////////////////

	static openpal::RSlice FormatTestLinkStatus(openpal::WSlice& output, bool aIsMaster, bool aFcb, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger);
	static openpal::RSlice FormatResetLinkStates(openpal::WSlice& output, bool aIsMaster, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger);
	static openpal::RSlice FormatRequestLinkStatus(openpal::WSlice& output, bool aIsMaster, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger);
	static openpal::RSlice FormatConfirmedUserData(openpal::WSlice& output, bool aIsMaster, bool aFcb, uint16_t aDest, uint16_t aSrc, const uint8_t* apData, uint8_t aDataLength, openpal::Logger* pLogger);
	static openpal::RSlice FormatUnconfirmedUserData(openpal::WSlice& output, bool aIsMaster, uint16_t aDest, uint16_t aSrc, const uint8_t* apData, uint8_t aDataLength, openpal::Logger* pLogger);

	////////////////////////////////////////////////
	//	Reusable static formatting functions to any buffer
	////////////////////////////////////////////////

	/** Reads data from src to dest removing 2 byte CRC checks every 16 data bytes
	@param apSrc Source buffer with crc checks. Must begin at data, not header
	@param apDest Destination buffer to which the data is extracted
	@param aLength Length of user data to read to the dest buffer. The source buffer must be larger b/c of crc bytes.
	*/
	static void ReadUserData(const uint8_t* apSrc, uint8_t* apDest, uint32_t aLength);

	/** Validates FT3 user data integriry
	@param apBody Beginning of the FT3 user data
	@param aLength Number of user bytes to verify, not user + crc.
	@return True if the body CRC is correct */
	static bool ValidateBodyCRC(const uint8_t* apBody, uint32_t aLength);

	// @return Total frame size based on user data length
	static uint32_t CalcFrameSize(uint8_t dataLength);

private:

	static uint32_t CalcUserDataSize(uint8_t dataLength);


	/** Writes data from src to dest interlacing 2 byte CRC checks every 16 data bytes
		@param apSrc Source buffer full of user data
		@param apDest Destination buffer where the data + CRC is written
		@param length Number of user data bytes
	*/
	static void WriteUserData(const uint8_t* pSrc, uint8_t* pDest, uint8_t length);

	/** Write 10 header bytes to to buffer including 0x0564, all fields, and CRC */
	static openpal::RSlice FormatHeader(openpal::WSlice& output, uint8_t aDataLength, bool aIsMaster, bool aFcb, bool aFcvDfc, LinkFunction aCode, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger);

};

}

#endif

