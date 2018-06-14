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
#include "CountIndexParser.h"

#include <openpal/logging/LogMacros.h>

#include "opendnp3/objects/Group2.h"
#include "opendnp3/objects/Group4.h"
#include "opendnp3/objects/Group11.h"
#include "opendnp3/objects/Group12.h"
#include "opendnp3/objects/Group13.h"
#include "opendnp3/objects/Group22.h"
#include "opendnp3/objects/Group23.h"
#include "opendnp3/objects/Group32.h"
#include "opendnp3/objects/Group41.h"
#include "opendnp3/objects/Group42.h"
#include "opendnp3/objects/Group43.h"

#include "opendnp3/app/parsing/BufferedCollection.h"

using namespace openpal;

namespace opendnp3
{

CountIndexParser::CountIndexParser(uint16_t count_, uint32_t requiredSize_, const NumParser& numparser_, HandleFun handler_) :
	count(count_),
	requiredSize(requiredSize_),
	numparser(numparser_),
	handler(handler_)
{}

ParseResult CountIndexParser::ParseHeader(
    openpal::RSlice& buffer,
    const NumParser& numparser,
    const ParserSettings& settings,
    const HeaderRecord& record,
    openpal::Logger* pLogger,
    IAPDUHandler* pHandler)
{
	uint16_t count;
	auto res = numparser.ParseCount(buffer, count, pLogger);
	if (res == ParseResult::OK)
	{
		FORMAT_LOGGER_BLOCK(pLogger, settings.Filters(),
		                    "%03u,%03u %s, %s [%u]",
		                    record.group,
		                    record.variation,
		                    GroupVariationToString(record.enumeration),
		                    QualifierCodeToString(record.GetQualifierCode()),
		                    count);

		return ParseCountOfObjects(buffer, record, numparser, count, pLogger, pHandler);
	}
	else
	{
		return res;
	}
}

ParseResult CountIndexParser::Process(const HeaderRecord& record, openpal::RSlice& buffer, IAPDUHandler* pHandler, openpal::Logger* pLogger) const
{
	if (buffer.Size() < requiredSize)
	{
		SIMPLE_LOGGER_BLOCK(pLogger, flags::WARN, "Not enough data for specified objects");
		return ParseResult::NOT_ENOUGH_DATA_FOR_OBJECTS;
	}
	else
	{
		if (pHandler)
		{
			handler(record, count, numparser, buffer, *pHandler);
		}
		buffer.Advance(requiredSize);
		return ParseResult::OK;
	}
}


ParseResult CountIndexParser::ParseCountOfObjects(openpal::RSlice& buffer, const HeaderRecord& record, const NumParser& numparser, uint16_t count, openpal::Logger* pLogger, IAPDUHandler* pHandler)
{
	switch (record.enumeration)
	{
	case(GroupVariation::Group2Var1) :
		return CountIndexParser::From<Group2Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group2Var2) :
		return CountIndexParser::From<Group2Var2>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group2Var3) :
		return CountIndexParser::From<Group2Var3>(count, numparser).Process(record, buffer, pHandler, pLogger);

	case(GroupVariation::Group4Var1) :
		return CountIndexParser::From<Group4Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group4Var2) :
		return CountIndexParser::From<Group4Var2>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group4Var3) :
		return CountIndexParser::From<Group4Var3>(count, numparser).Process(record, buffer, pHandler, pLogger);


	case(GroupVariation::Group11Var1) :
		return CountIndexParser::From<Group11Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group11Var2) :
		return CountIndexParser::From<Group11Var2>(count, numparser).Process(record, buffer, pHandler, pLogger);

	case(GroupVariation::Group12Var1) :
		return CountIndexParser::From<Group12Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);

	case(GroupVariation::Group13Var1) :
		return CountIndexParser::From<Group13Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group13Var2) :
		return CountIndexParser::From<Group13Var2>(count, numparser).Process(record, buffer, pHandler, pLogger);

	case(GroupVariation::Group22Var1) :
		return CountIndexParser::From<Group22Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group22Var2) :
		return CountIndexParser::From<Group22Var2>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group22Var5) :
		return CountIndexParser::From<Group22Var5>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group22Var6) :
		return CountIndexParser::From<Group22Var6>(count, numparser).Process(record, buffer, pHandler, pLogger);

	case(GroupVariation::Group23Var1) :
		return CountIndexParser::From<Group23Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group23Var2) :
		return CountIndexParser::From<Group23Var2>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group23Var5) :
		return CountIndexParser::From<Group23Var5>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group23Var6) :
		return CountIndexParser::From<Group23Var6>(count, numparser).Process(record, buffer, pHandler, pLogger);

	case(GroupVariation::Group32Var1) :
		return CountIndexParser::From<Group32Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group32Var2) :
		return CountIndexParser::From<Group32Var2>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group32Var3) :
		return CountIndexParser::From<Group32Var3>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group32Var4) :
		return CountIndexParser::From<Group32Var4>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group32Var5) :
		return CountIndexParser::From<Group32Var5>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group32Var6) :
		return CountIndexParser::From<Group32Var6>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group32Var7) :
		return CountIndexParser::From<Group32Var7>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group32Var8) :
		return CountIndexParser::From<Group32Var8>(count, numparser).Process(record, buffer, pHandler, pLogger);

	case(GroupVariation::Group41Var1) :
		return CountIndexParser::From<Group41Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group41Var2) :
		return CountIndexParser::From<Group41Var2>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group41Var3) :
		return CountIndexParser::From<Group41Var3>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group41Var4) :
		return CountIndexParser::From<Group41Var4>(count, numparser).Process(record, buffer, pHandler, pLogger);

	case(GroupVariation::Group42Var1) :
		return CountIndexParser::From<Group42Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group42Var2) :
		return CountIndexParser::From<Group42Var2>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group42Var3) :
		return CountIndexParser::From<Group42Var3>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group42Var4) :
		return CountIndexParser::From<Group42Var4>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group42Var5) :
		return CountIndexParser::From<Group42Var5>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group42Var6) :
		return CountIndexParser::From<Group42Var6>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group42Var7) :
		return CountIndexParser::From<Group42Var7>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group42Var8) :
		return CountIndexParser::From<Group42Var8>(count, numparser).Process(record, buffer, pHandler, pLogger);

	case(GroupVariation::Group43Var1) :
		return CountIndexParser::From<Group43Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group43Var2) :
		return CountIndexParser::From<Group43Var2>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group43Var3) :
		return CountIndexParser::From<Group43Var3>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group43Var4) :
		return CountIndexParser::From<Group43Var4>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group43Var5) :
		return CountIndexParser::From<Group43Var5>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group43Var6) :
		return CountIndexParser::From<Group43Var6>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group43Var7) :
		return CountIndexParser::From<Group43Var7>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group43Var8) :
		return CountIndexParser::From<Group43Var8>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group50Var4) :
		return CountIndexParser::From<Group50Var4>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group111Var0) :
		return ParseIndexPrefixedOctetData(buffer, record, numparser, count, pLogger, pHandler);

	case(GroupVariation::Group122Var1) :
		return CountIndexParser::FromType<Group122Var1>(count, numparser).Process(record, buffer, pHandler, pLogger);
	case(GroupVariation::Group122Var2) :
		return CountIndexParser::FromType<Group122Var2>(count, numparser).Process(record, buffer, pHandler, pLogger);

	default:

		FORMAT_LOGGER_BLOCK(pLogger, flags::WARN,
		                    "Unsupported qualifier/object - %s - %i / %i",
		                    QualifierCodeToString(record.GetQualifierCode()), record.group, record.variation);

		return ParseResult::INVALID_OBJECT_QUALIFIER;
	}
}

ParseResult CountIndexParser::ParseIndexPrefixedOctetData(openpal::RSlice& buffer, const HeaderRecord& record, const NumParser& numparser, uint32_t count, openpal::Logger* pLogger, IAPDUHandler* pHandler)
{
	if (record.variation == 0)
	{
		SIMPLE_LOGGER_BLOCK(pLogger, flags::WARN, "Octet string variation 0 may only be used in requests");
		return ParseResult::INVALID_OBJECT;
	}

	const uint32_t TOTAL_SIZE = count * (numparser.NumBytes() + record.variation);

	if (buffer.Size() < TOTAL_SIZE)
	{
		SIMPLE_LOGGER_BLOCK(pLogger, flags::WARN, "Not enough data for specified bitfield objects");
		return ParseResult::NOT_ENOUGH_DATA_FOR_OBJECTS;
	}

	if (pHandler)
	{
		auto read = [&numparser, record](RSlice & buffer, uint32_t pos) -> Indexed<OctetString>
		{
			auto index = numparser.ReadNum(buffer);
			OctetString octets(buffer.Take(record.variation));
			buffer.Advance(record.variation);
			return WithIndex(octets, index);
		};

		auto collection = CreateBufferedCollection<Indexed<OctetString>>(buffer, count, read);
		pHandler->OnHeader(PrefixHeader(record, count), collection);
	}

	buffer.Advance(TOTAL_SIZE);
	return ParseResult::OK;
}

}