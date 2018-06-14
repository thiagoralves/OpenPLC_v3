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
#include "AppControlField.h"

namespace opendnp3
{

const AppControlField AppControlField::DEFAULT(true, true, false, false, 0);

AppControlField::AppControlField(uint8_t byte) :
	FIR((byte & FIR_MASK) != 0),
	FIN((byte & FIN_MASK) != 0),
	CON((byte & CON_MASK) != 0),
	UNS((byte & UNS_MASK) != 0),
	SEQ(byte & SEQ_MASK)
{
}

AppControlField AppControlField::Request(uint8_t seq)
{
	return AppControlField(true, true, false, false, seq);
}

AppControlField::AppControlField() :
	FIR(true),
	FIN(true),
	CON(false),
	UNS(false),
	SEQ(0)
{}

AppControlField::AppControlField(bool fir, bool fin, bool con, bool uns, uint8_t seq) :
	FIR(fir),
	FIN(fin),
	CON(con),
	UNS(uns),
	SEQ(seq)
{}

uint8_t AppControlField::ToByte() const
{
	uint8_t ret = 0;

	if (FIR)
	{
		ret |= FIR_MASK;
	}
	if (FIN)
	{
		ret |= FIN_MASK;
	}
	if (CON)
	{
		ret |= CON_MASK;
	}
	if (UNS)
	{
		ret |= UNS_MASK;
	}

	uint8_t seq = SEQ % 16;

	return ret | seq;
}

}

