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
#ifndef OPENDNP3_PREFIXFIELDS_H
#define OPENDNP3_PREFIXFIELDS_H

#include <openpal/container/WSlice.h>
#include <openpal/util/Uncopyable.h>

namespace opendnp3
{

class PrefixFields : private openpal::StaticOnly
{
public:

	template <typename... Args>
	static bool Read(openpal::RSlice& input, Args& ... fields)
	{
		if (input.Size() < (sizeof...(Args)*openpal::UInt16::SIZE))
		{
			// not enough in the buffer to even read the length prefixes
			return false;
		}

		uint16_t lengths[sizeof...(Args)];

		for (size_t i = 0; i < sizeof...(Args); ++i)
		{
			lengths[i] = openpal::UInt16::ReadBuffer(input);
		}

		uint32_t sum = 0;

		for (size_t i = 0; i < sizeof...(Args); ++i)
		{
			sum += lengths[i];
		}

		if (input.Size() < sum)
		{
			// not enough for the defined fields
			return false;
		}

		ReadFields(input, lengths, fields...);

		return true;
	}

	template <typename... Args>
	static bool Write(openpal::WSlice& dest, const Args& ... fields)
	{
		const uint32_t TOTAL_SIZE = (sizeof...(Args) * openpal::UInt16::SIZE) + SumSizes(fields...);

		if (dest.Size() < TOTAL_SIZE)
		{
			return false;
		}

		WriteLengths(dest, fields...);
		WriteFields(dest, fields...);

		return true;
	}

	template <typename... Args>
	static bool LengthFitsInUInt16(const openpal::RSlice& arg1, Args& ... fields)
	{
		return (arg1.Size() <= openpal::MaxValue<uint16_t>()) && LengthFitsInUInt16(fields...);
	}

private:

	template <typename... Args>
	static uint32_t SumSizes(const openpal::RSlice& arg1, Args& ... fields)
	{
		return arg1.Size() + SumSizes(fields...);
	}

	static bool LengthFitsInUInt16()
	{
		return true;
	}

	static uint32_t SumSizes()
	{
		return 0;
	}

	template <typename... Args>
	static void ReadFields(openpal::RSlice& input, uint16_t* pLength, openpal::RSlice& output, Args& ... fields)
	{
		output = input.Take(*pLength);
		input.Advance(*pLength);
		ReadFields(input, ++pLength, fields...);
	}

	static void ReadFields(openpal::RSlice& input, uint16_t* pLength) {}

	template <typename... Args>
	static void WriteLengths(openpal::WSlice& dest, const openpal::RSlice& arg1, Args& ... fields)
	{
		openpal::UInt16::WriteBuffer(dest, static_cast<uint16_t>(arg1.Size()));
		WriteLengths(dest, fields...);
	}

	static void WriteLengths(openpal::WSlice& dest) {}

	template <typename... Args>
	static void WriteFields(openpal::WSlice& dest, const openpal::RSlice& arg1, Args& ... fields)
	{
		arg1.CopyTo(dest);
		WriteFields(dest, fields...);
	}

	static void WriteFields(openpal::WSlice& dest) {}


};


}

#endif
