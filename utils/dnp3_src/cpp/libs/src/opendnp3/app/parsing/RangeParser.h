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
#ifndef OPENDNP3_RANGEPARSER_H
#define OPENDNP3_RANGEPARSER_H

#include <openpal/container/RSlice.h>
#include <openpal/logging/Logger.h>

#include "opendnp3/app/parsing/ParseResult.h"
#include "opendnp3/app/parsing/IAPDUHandler.h"
#include "opendnp3/app/parsing/ParseResult.h"
#include "opendnp3/app/parsing/NumParser.h"
#include "opendnp3/app/parsing/ParserSettings.h"
#include "opendnp3/app/parsing/BitReader.h"

#include "opendnp3/app/Range.h"
#include "opendnp3/app/parsing/BufferedCollection.h"


namespace opendnp3
{

class RangeParser
{
	typedef void (*HandleFun)(const HeaderRecord& record, const Range& range, const openpal::RSlice& buffer, IAPDUHandler& handler);



public:

	static ParseResult ParseHeader(
	    openpal::RSlice& buffer,
	    const NumParser& numparser,
	    const ParserSettings& settings,
	    const HeaderRecord& record,
	    openpal::Logger* pLogger,
	    IAPDUHandler* pHandler
	);

private:

	// Process the range against the buffer
	ParseResult Process(const HeaderRecord& record, openpal::RSlice& buffer, IAPDUHandler* pHandler, openpal::Logger* pLogger) const;

	// Create a range parser from a fixed size descriptor
	template <class Descriptor>
	static RangeParser FromFixedSize(const Range& range);

	template <class Type>
	static RangeParser FromFixedSizeType(const Range& range);

	// Create a range parser from a bitfield and a function to map the bitfield to values
	template <class Type>
	static RangeParser FromBitfieldType(const Range& range);

	template <class Type>
	static RangeParser FromDoubleBitfieldType(const Range& range);

	static ParseResult ParseRangeOfObjects(openpal::RSlice& buffer, const HeaderRecord& record, const Range& range, openpal::Logger* pLogger, IAPDUHandler* pHandler);

	static ParseResult ParseRangeOfOctetData(openpal::RSlice& buffer, const HeaderRecord& record, const Range& range, openpal::Logger* pLogger, IAPDUHandler* pHandler);

	template <class Descriptor>
	static void InvokeRangeOf(const HeaderRecord& record, const Range& range, const openpal::RSlice& buffer, IAPDUHandler& handler);

	template <class Type>
	static void InvokeRangeOfType(const HeaderRecord& record, const Range& range, const openpal::RSlice& buffer, IAPDUHandler& handler);

	template <class Type>
	static void InvokeRangeBitfieldType(const HeaderRecord& record, const Range& range, const openpal::RSlice& buffer, IAPDUHandler& handler);

	template <class Type>
	static void InvokeRangeDoubleBitfieldType(const HeaderRecord& record, const Range& range, const openpal::RSlice& buffer, IAPDUHandler& handler);

	RangeParser(const Range& range, uint32_t requiredSize, HandleFun handler);

	Range range;
	uint32_t requiredSize;
	HandleFun handler;

	RangeParser() = delete;
};

template <class Descriptor>
RangeParser RangeParser::FromFixedSize(const Range& range)
{
	uint32_t size = range.Count() * Descriptor::Size();
	return RangeParser(range, size, &InvokeRangeOf<Descriptor>);
}

template <class Type>
RangeParser RangeParser::FromFixedSizeType(const Range& range)
{
	uint32_t size = range.Count() * Type::Size();
	return RangeParser(range, size, &InvokeRangeOfType<Type>);
}

template <class Descriptor>
void RangeParser::InvokeRangeOf(const HeaderRecord& record, const Range& range, const openpal::RSlice& buffer, IAPDUHandler& handler)
{
	const auto COUNT = range.Count();

	auto read = [range](openpal::RSlice & buffer, uint32_t pos)
	{
		typename Descriptor::Target target;
		Descriptor::ReadTarget(buffer, target);
		return WithIndex(target, range.start + pos);
	};

	auto collection = CreateBufferedCollection<Indexed<typename Descriptor::Target>>(buffer, COUNT, read);

	handler.OnHeader(RangeHeader(record, range), collection);
}

template <class Type>
void RangeParser::InvokeRangeOfType(const HeaderRecord& record, const Range& range, const openpal::RSlice& buffer, IAPDUHandler& handler)
{
	const auto COUNT = range.Count();

	auto read = [range](openpal::RSlice & buffer, uint32_t pos) -> Indexed<Type>
	{
		Type target;
		Type::Read(buffer, target);
		return WithIndex(target, range.start + pos);
	};

	auto collection = CreateBufferedCollection<Indexed<Type>>(buffer, COUNT, read);

	handler.OnHeader(RangeHeader(record, range), collection);
}


template <class Type>
RangeParser RangeParser::FromBitfieldType(const Range& range)
{
	const uint32_t SIZE = NumBytesInBits(range.Count());
	return RangeParser(range, SIZE, &InvokeRangeBitfieldType<Type>);
}

template <class Type>
void RangeParser::InvokeRangeBitfieldType(const HeaderRecord& record, const Range& range, const openpal::RSlice& buffer, IAPDUHandler& handler)
{
	const uint32_t COUNT = range.Count();

	auto read = [range](openpal::RSlice & buffer, uint32_t pos) -> Indexed<Type>
	{
		Type value(GetBit(buffer, pos));
		return WithIndex(value, range.start + pos);
	};

	auto collection = CreateBufferedCollection<Indexed<Type>>(buffer, COUNT, read);

	handler.OnHeader(RangeHeader(record, range), collection);
}

template <class Type>
RangeParser RangeParser::FromDoubleBitfieldType(const Range& range)
{
	uint32_t size = NumBytesInDoubleBits(range.Count());
	return RangeParser(range, size, &InvokeRangeDoubleBitfieldType<Type>);
}

template <class Type>
void RangeParser::InvokeRangeDoubleBitfieldType(const HeaderRecord& record, const Range& range, const openpal::RSlice& buffer, IAPDUHandler& handler)
{
	const uint32_t COUNT = range.Count();

	auto read = [range](openpal::RSlice & buffer, uint32_t pos) -> Indexed<Type>
	{
		Type value(GetDoubleBit(buffer, pos));
		return WithIndex(value, range.start + pos);
	};

	auto collection = CreateBufferedCollection<Indexed<Type>>(buffer, COUNT, read);

	handler.OnHeader(RangeHeader(record, range), collection);
}

}

#endif
