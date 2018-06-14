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
#include "LinkFrame.h"

#include "opendnp3/link/CRC.h"
#include "opendnp3/link/LinkHeader.h"
#include "opendnp3/LogLevels.h"

#include <openpal/serialization/Serialization.h>
#include <openpal/logging/LogMacros.h>
#include <openpal/Configure.h>

#include <assert.h>

using namespace openpal;

namespace opendnp3
{

void LinkFrame::ReadUserData(const uint8_t* pSrc, uint8_t* pDest, uint32_t length_)
{
	uint32_t length = length_;
	uint8_t const* pRead = pSrc;
	uint8_t* pWrite = pDest;

	while (length > 0)
	{
		uint32_t max = LPDU_DATA_BLOCK_SIZE;
		uint32_t num = (length <= max) ? length : max;
		uint32_t num_with_crc = num + 2;
		memmove(pWrite, pRead, num);
		pRead += num_with_crc;
		pWrite += num;
		length -= num;
	}
}


bool LinkFrame::ValidateBodyCRC(const uint8_t* pBody, uint32_t length)
{
	while(length > 0)
	{
		uint32_t max = LPDU_DATA_BLOCK_SIZE;
		uint32_t num = (length <= max) ? length : max;

		if (CRC::IsCorrectCRC(pBody, num))
		{
			pBody += (num + 2);
			length -= num;
		}
		else
		{
			return false;
		}
	}
	return true;
}

uint32_t LinkFrame::CalcFrameSize(uint8_t dataLength)
{
	return LPDU_HEADER_SIZE + CalcUserDataSize(dataLength);
}

uint32_t LinkFrame::CalcUserDataSize(uint8_t dataLength)
{
	if (dataLength > 0)
	{
		uint32_t mod16 = dataLength % LPDU_DATA_BLOCK_SIZE;
		uint32_t size = (dataLength / LPDU_DATA_BLOCK_SIZE) * LPDU_DATA_PLUS_CRC_SIZE; //complete blocks
		return (mod16 > 0) ? (size + mod16 + LPDU_CRC_SIZE) : size; //possible partial block
	}
	else
	{
		return 0;
	}
}

////////////////////////////////////////////////
//
//	Outgoing frame formatting functions for Sec to Pri transactions
//
////////////////////////////////////////////////

RSlice LinkFrame::FormatAck(WSlice& buffer, bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger)
{
	return FormatHeader(buffer, 0, aIsMaster, false, aIsRcvBuffFull, LinkFunction::SEC_ACK, aDest, aSrc, pLogger);
}

RSlice LinkFrame::FormatNack(WSlice& buffer, bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger)
{
	return FormatHeader(buffer, 0, aIsMaster, false, aIsRcvBuffFull, LinkFunction::SEC_NACK, aDest, aSrc, pLogger);
}

RSlice LinkFrame::FormatLinkStatus(WSlice& buffer, bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger)
{
	return FormatHeader(buffer, 0, aIsMaster, false, aIsRcvBuffFull, LinkFunction::SEC_LINK_STATUS, aDest, aSrc, pLogger);
}

RSlice LinkFrame::FormatNotSupported(WSlice& buffer, bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger)
{
	return FormatHeader(buffer, 0, aIsMaster, false, aIsRcvBuffFull, LinkFunction::SEC_NOT_SUPPORTED, aDest, aSrc, pLogger);
}

////////////////////////////////////////////////
//
//	Outgoing frame formatting functions for Pri to Sec transactions
//
////////////////////////////////////////////////

RSlice LinkFrame::FormatResetLinkStates(WSlice& buffer, bool aIsMaster, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger)
{
	return FormatHeader(buffer, 0, aIsMaster, false, false, LinkFunction::PRI_RESET_LINK_STATES, aDest, aSrc, pLogger);
}

RSlice LinkFrame::FormatRequestLinkStatus(WSlice& buffer, bool aIsMaster, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger)
{
	return FormatHeader(buffer, 0, aIsMaster, false, false, LinkFunction::PRI_REQUEST_LINK_STATUS, aDest, aSrc, pLogger);
}

RSlice LinkFrame::FormatTestLinkStatus(WSlice& buffer, bool aIsMaster, bool aFcb, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger)
{
	return FormatHeader(buffer, 0, aIsMaster, aFcb, true, LinkFunction::PRI_TEST_LINK_STATES, aDest, aSrc, pLogger);
}

RSlice LinkFrame::FormatConfirmedUserData(WSlice& buffer, bool aIsMaster, bool aFcb, uint16_t aDest, uint16_t aSrc, const uint8_t* apData, uint8_t dataLength, openpal::Logger* pLogger)
{
	assert(dataLength > 0);
	assert(dataLength <= LPDU_MAX_USER_DATA_SIZE);
	auto userDataSize = CalcUserDataSize(dataLength);
	auto ret = buffer.ToRSlice().Take(userDataSize + LPDU_HEADER_SIZE);
	FormatHeader(buffer, dataLength, aIsMaster, aFcb, true, LinkFunction::PRI_CONFIRMED_USER_DATA, aDest, aSrc, pLogger);
	WriteUserData(apData, buffer, dataLength);
	buffer.Advance(userDataSize);
	return ret;
}

RSlice LinkFrame::FormatUnconfirmedUserData(WSlice& buffer, bool aIsMaster, uint16_t aDest, uint16_t aSrc, const uint8_t* apData, uint8_t dataLength, openpal::Logger* pLogger)
{
	assert(dataLength > 0);
	assert(dataLength <= LPDU_MAX_USER_DATA_SIZE);
	auto userDataSize = CalcUserDataSize(dataLength);
	auto ret = buffer.ToRSlice().Take(userDataSize + LPDU_HEADER_SIZE);
	FormatHeader(buffer, dataLength, aIsMaster, false, false, LinkFunction::PRI_UNCONFIRMED_USER_DATA, aDest, aSrc, pLogger);
	WriteUserData(apData, buffer, dataLength);
	buffer.Advance(userDataSize);
	return ret;
}

RSlice LinkFrame::FormatHeader(WSlice& buffer, uint8_t aDataLength, bool aIsMaster, bool aFcb, bool aFcvDfc, LinkFunction aFuncCode, uint16_t aDest, uint16_t aSrc, openpal::Logger* pLogger)
{
	assert(buffer.Size() >= LPDU_HEADER_SIZE);
	LinkHeader header(aDataLength + LPDU_MIN_LENGTH, aSrc, aDest, aIsMaster, aFcvDfc, aFcb, aFuncCode);

	FORMAT_LOGGER_BLOCK(pLogger, flags::LINK_TX,
	                    "Function: %s Dest: %u Source: %u Length: %u",
	                    LinkFunctionToString(aFuncCode),
	                    aDest,
	                    aSrc,
	                    aDataLength);

	header.Write(buffer);
	auto ret = buffer.ToRSlice().Take(10);
	buffer.Advance(10);
	return ret;
}

void LinkFrame::WriteUserData(const uint8_t* pSrc, uint8_t* pDest, uint8_t length)
{
	while (length > 0)
	{
		uint8_t max = LPDU_DATA_BLOCK_SIZE;
		uint8_t num = length > max ? max : length;
		memcpy(pDest, pSrc, num);
		CRC::AddCrc(pDest, num);
		pSrc += num;
		pDest += (num + 2);
		length -= num;
	}
}

} //end namespace

