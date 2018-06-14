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
#include "LinkHeader.h"

#include "CRC.h"

#include <openpal/serialization/Serialization.h>

using namespace std;

namespace opendnp3
{

LinkHeader::LinkHeader() : length(5), src(0), dest(0), ctrl(0)
{

}

LinkHeader::LinkHeader(uint8_t aLen, uint16_t aSrc, uint16_t aDest, bool aFromMaster, bool aFcvDfc, bool aFcb, LinkFunction aCode)
{
	this->Set(aLen, aSrc, aDest, aFromMaster, aFcvDfc, aFcb, aCode);
}

void LinkHeader::Set(uint8_t aLen, uint16_t aSrc, uint16_t aDest, bool aFromMaster, bool aFcvDfc, bool aFcb, LinkFunction aCode)
{
	length = aLen;
	src = aSrc;
	dest = aDest;
	ctrl = ControlByte(aFromMaster, aFcb, aFcvDfc, aCode);
}

void LinkHeader::ChangeFCB(bool aFCB)
{
	if(aFCB)
	{
		ctrl |= MASK_FCB;
	}
	else
	{
		ctrl &= ~MASK_FCB;
	}
}

uint8_t LinkHeader::ControlByte(bool aIsMaster, bool aFcb, bool aFcvDfc, LinkFunction aFunc)
{
	uint8_t ret = LinkFunctionToType(aFunc);

	if(aIsMaster) ret |= MASK_DIR;
	if(aFcb) ret |= MASK_FCB;
	if(aFcvDfc) ret |= MASK_FCV;

	return ret;
}

void LinkHeader::Read(const uint8_t* apBuff)
{
	length = apBuff[LI_LENGTH];
	dest = openpal::UInt16::Read(apBuff + LI_DESTINATION);
	src = openpal::UInt16::Read(apBuff + LI_SOURCE);
	ctrl = apBuff[LI_CONTROL];
}

void LinkHeader::Write(uint8_t* apBuff) const
{
	apBuff[LI_START_05] = 0x05;
	apBuff[LI_START_64] = 0x64;

	apBuff[LI_LENGTH] = length;
	openpal::UInt16::Write(apBuff + LI_DESTINATION, dest);
	openpal::UInt16::Write(apBuff + LI_SOURCE, src);
	apBuff[LI_CONTROL] = ctrl;

	CRC::AddCrc(apBuff, LI_CRC);
}

}

