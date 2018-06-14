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
#include "openpal/container/WSlice.h"

#include "openpal/util/Comparisons.h"
#include "openpal/container/RSlice.h"

#include <cstring>

namespace openpal
{

WSlice WSlice::Empty()
{
	return WSlice();
}

WSlice::WSlice():
	HasSize(0),
	pBuffer(nullptr)
{}

void WSlice::SetAllTo(uint8_t value)
{
	memset(pBuffer, value, size);
}

WSlice::WSlice(uint8_t* pBuffer_, uint32_t size) :
	HasSize(size),
	pBuffer(pBuffer_)
{}

void WSlice::Clear()
{
	pBuffer = nullptr;
	size = 0;
}

uint32_t WSlice::Advance(uint32_t count)
{
	auto num = openpal::Min(count, size);
	pBuffer += num;
	size -= num;
	return num;
}

WSlice WSlice::Skip(uint32_t count) const
{
	auto num = openpal::Min(count, size);
	return WSlice(pBuffer + num, size - num);
}

RSlice WSlice::ToRSlice() const
{
	return RSlice(pBuffer, size);
}

}


