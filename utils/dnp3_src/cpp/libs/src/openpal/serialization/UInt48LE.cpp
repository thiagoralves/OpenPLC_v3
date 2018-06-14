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
#include "openpal/serialization/UInt48LE.h"

namespace openpal
{

UInt48Type UInt48LE::Read(const uint8_t* data)
{
	return UInt48Type(
	           (static_cast<int64_t>(data[0]) << 0) |
	           (static_cast<int64_t>(data[1]) << 8) |
	           (static_cast<int64_t>(data[2]) << 16) |
	           (static_cast<int64_t>(data[3]) << 24) |
	           (static_cast<int64_t>(data[4]) << 32) |
	           (static_cast<int64_t>(data[5]) << 40));
}

void UInt48LE::Write(uint8_t* data, UInt48Type value)
{
	if (value > MAX)
	{
		value = UInt48Type(MAX);
	}

	data[0] = static_cast<uint8_t>(value & 0xFF);
	data[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
	data[2] = static_cast<uint8_t>((value >> 16) & 0xFF);
	data[3] = static_cast<uint8_t>((value >> 24) & 0xFF);
	data[4] = static_cast<uint8_t>((value >> 32) & 0xFF);
	data[5] = static_cast<uint8_t>((value >> 40) & 0xFF);
}

}

