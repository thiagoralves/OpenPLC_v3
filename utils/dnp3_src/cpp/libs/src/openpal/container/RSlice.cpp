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
#include "openpal/container/RSlice.h"

#include "openpal/util/Comparisons.h"

#include "openpal/container/WSlice.h"

#include <cstring>

namespace openpal
{

RSlice RSlice::Empty()
{
	return RSlice();
}

RSlice::RSlice(): HasSize(0), pBuffer(nullptr)
{}

RSlice::RSlice(uint8_t const* pBuffer, uint32_t size) :
	HasSize(size),
	pBuffer(pBuffer)
{}

RSlice RSlice::CopyTo(WSlice& dest) const
{
	if (dest.Size() < size)
	{
		return RSlice::Empty();
	}
	else
	{
		WSlice copy(dest);
		memcpy(dest, pBuffer, size);
		dest.Advance(size);
		return copy.ToRSlice().Take(size);
	}
}

RSlice RSlice::Take(uint32_t count) const
{
	return RSlice(pBuffer, openpal::Min(size, count));
}

RSlice RSlice::Skip(uint32_t count) const
{
	auto num = openpal::Min(size, count);
	return RSlice(pBuffer + num, size - num);
}

void RSlice::Clear()
{
	pBuffer = nullptr;
	size = 0;
}

bool RSlice::Equals(const RSlice& rhs) const
{
	if (this->Size() == rhs.Size())
	{
		return memcmp(pBuffer, rhs.pBuffer, Size()) == 0;
	}
	else
	{
		return false;
	}
}

void RSlice::Advance(uint32_t count)
{
	auto num = openpal::Min(size, count);
	pBuffer += num;
	size -= num;
}

}
