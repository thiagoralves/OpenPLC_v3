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
#include "CopyableBuffer.h"

#include "HexConversions.h"

#include <memory.h>

#include <openpal/util/ToHex.h>
#include <openpal/container/WSlice.h>

using namespace testlib;

namespace testlib
{

std::ostream& operator<<(std::ostream& output, const CopyableBuffer& arBuff)
{
	output << "[" << ToHex(arBuff.ToRSlice(), true) << "]";
	return output;
}

CopyableBuffer::CopyableBuffer() :
	mpBuff(nullptr),
	mSize(0)
{

}

CopyableBuffer::CopyableBuffer(uint32_t aSize) :
	mpBuff(new uint8_t[aSize]),
	mSize(aSize)
{
	this->Zero();
}

CopyableBuffer::CopyableBuffer(const openpal::RSlice& buffer) :
	mpBuff(new uint8_t[buffer.Size()]),
	mSize(buffer.Size())
{
	openpal::WSlice dest(mpBuff, mSize);
	buffer.CopyTo(dest);
}

CopyableBuffer::CopyableBuffer(const uint8_t* apBuff, uint32_t aSize) :
	mpBuff(new uint8_t[aSize]),
	mSize(aSize)
{
	memcpy(mpBuff, apBuff, aSize);
}

CopyableBuffer::CopyableBuffer(const CopyableBuffer& arBuffer) :
	mpBuff(new uint8_t[arBuffer.Size()]),
	mSize(arBuffer.Size())
{
	memcpy(mpBuff, arBuffer, mSize);
}

void CopyableBuffer::Zero()
{
	memset(mpBuff, 0, mSize);
}

CopyableBuffer& CopyableBuffer::operator=(const CopyableBuffer& arRHS)
{
	//check for assignment to self
	if(this == &arRHS) return *this;

	if(arRHS.Size() != mSize)
	{
		mSize = arRHS.Size();
		delete mpBuff;
		mpBuff = new uint8_t[mSize];
	}

	memcpy(mpBuff, arRHS, mSize);

	return *this;
}

CopyableBuffer::~CopyableBuffer()
{
	delete [] mpBuff;
}

bool CopyableBuffer::operator==( const CopyableBuffer& other) const
{
	if(other.Size() != this->Size()) return false;
	else
	{
		for(size_t i = 0; i < this->Size(); ++i)
		{
			if(this->mpBuff[i] != other.mpBuff[i]) return false;
		}

		return true;
	}
}

}
