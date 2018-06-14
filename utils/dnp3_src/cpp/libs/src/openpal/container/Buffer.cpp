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
#include "openpal/container/Buffer.h"

namespace openpal
{
Buffer::Buffer() : Array<uint8_t, uint32_t>(0)
{}

Buffer::Buffer(uint32_t size) : Array<uint8_t, uint32_t>(size)
{}

Buffer::Buffer(const RSlice& input) : Array<uint8_t, uint32_t>(input.Size())
{
	auto dest = this->GetWSlice();
	input.CopyTo(dest);
}

RSlice Buffer::ToRSlice() const
{
	return RSlice(this->buffer, this->size);
}

WSlice Buffer::GetWSlice()
{
	return WSlice(this->buffer, this->Size());
}

WSlice Buffer::GetWSlice(uint32_t maxSize)
{
	if (maxSize <= this->Size())
	{
		return WSlice(this->buffer, maxSize);
	}
	else
	{
		return GetWSlice();
	}
}
}

