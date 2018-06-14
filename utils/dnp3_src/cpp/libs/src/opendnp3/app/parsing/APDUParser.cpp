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
#include "APDUParser.h"

#include "opendnp3/LogLevels.h"

#include "opendnp3/gen/QualifierCode.h"
#include "opendnp3/app/GroupVariationRecord.h"
#include "opendnp3/app/MeasurementFactory.h"

#include "opendnp3/app/parsing/ObjectHeaderParser.h"
#include "opendnp3/app/parsing/CountParser.h"
#include "opendnp3/app/parsing/RangeParser.h"
#include "opendnp3/app/parsing/CountIndexParser.h"
#include "opendnp3/app/parsing/FreeFormatParser.h"

#include <openpal/logging/LogMacros.h>


using namespace openpal;

namespace opendnp3
{

ParseResult APDUParser::Parse(const openpal::RSlice& buffer, IAPDUHandler& handler, openpal::Logger& logger, ParserSettings settings)
{
	return Parse(buffer, handler, &logger, settings);
}

ParseResult APDUParser::Parse(const openpal::RSlice& buffer, IAPDUHandler& handler, openpal::Logger* pLogger, ParserSettings settings)
{
	// do two state parsing process with logging and white-listing first but no handling on the first pass
	auto result = ParseSinglePass(buffer, pLogger, nullptr, &handler, settings);
	// if the first pass was successful, do a 2nd pass with the handler but no logging or white-list
	return (result == ParseResult::OK) ? ParseSinglePass(buffer, nullptr, &handler, nullptr, settings) : result;
}

ParseResult APDUParser::ParseAndLogAll(const openpal::RSlice& buffer, openpal::Logger* pLogger, ParserSettings settings)
{
	return ParseSinglePass(buffer, pLogger, nullptr, nullptr, settings);
}

ParseResult APDUParser::ParseSinglePass(const openpal::RSlice& buffer, openpal::Logger* pLogger, IAPDUHandler* pHandler, IWhiteList* pWhiteList, const ParserSettings& settings)
{
	uint32_t count = 0;
	RSlice copy(buffer);
	while(copy.Size() > 0)
	{
		auto result = ParseHeader(copy, pLogger, count, settings, pHandler, pWhiteList);
		++count;
		if (result != ParseResult::OK)
		{
			return result;
		}
	}
	return ParseResult::OK;
}

ParseResult APDUParser::ParseHeader(RSlice& buffer, openpal::Logger* pLogger, uint32_t count, const ParserSettings& settings, IAPDUHandler* pHandler, IWhiteList* pWhiteList)
{
	ObjectHeader header;
	auto result = ObjectHeaderParser::ParseObjectHeader(header, buffer, pLogger);
	if (result != ParseResult::OK)
	{
		return result;
	}

	const auto GV = GroupVariationRecord::GetRecord(header.group, header.variation);

	if (GV.enumeration == GroupVariation::UNKNOWN)
	{
		FORMAT_LOGGER_BLOCK(pLogger, flags::WARN, "Unknown object %i / %i", GV.group, GV.variation);
		return ParseResult::UNKNOWN_OBJECT;
	}

	// if a white-list is defined and it doesn't validate, exit early
	if (pWhiteList && !pWhiteList->IsAllowed(count, GV.enumeration, QualifierCodeFromType(header.qualifier)))
	{
		FORMAT_LOGGER_BLOCK(
		    pLogger,
		    flags::WARN,
		    "Header (%i) w/ Object (%i,%i) and qualifier (%i) failed whitelist",
		    count,
		    header.group,
		    header.variation,
		    header.qualifier
		);

		return ParseResult::NOT_ON_WHITELIST;
	}


	return APDUParser::ParseQualifier(buffer, pLogger, HeaderRecord(GV, header.qualifier, count), settings, pHandler);
}

ParseResult APDUParser::ParseQualifier(RSlice& buffer, openpal::Logger* pLogger, const HeaderRecord& record, const ParserSettings& settings, IAPDUHandler* pHandler)
{
	switch (record.GetQualifierCode())
	{
	case(QualifierCode::ALL_OBJECTS) :
		return HandleAllObjectsHeader(pLogger, record, settings, pHandler);

	case(QualifierCode::UINT8_CNT) :
		return CountParser::ParseHeader(buffer, NumParser::OneByte(), settings, record, pLogger, pHandler);

	case(QualifierCode::UINT16_CNT) :
		return CountParser::ParseHeader(buffer, NumParser::TwoByte(), settings, record, pLogger, pHandler);

	case(QualifierCode::UINT8_START_STOP) :
		return RangeParser::ParseHeader(buffer, NumParser::OneByte(), settings, record, pLogger, pHandler);

	case(QualifierCode::UINT16_START_STOP) :
		return RangeParser::ParseHeader(buffer, NumParser::TwoByte(), settings, record, pLogger, pHandler);

	case(QualifierCode::UINT8_CNT_UINT8_INDEX) :
		return CountIndexParser::ParseHeader(buffer, NumParser::OneByte(), settings, record, pLogger, pHandler);

	case(QualifierCode::UINT16_CNT_UINT16_INDEX) :
		return CountIndexParser::ParseHeader(buffer, NumParser::TwoByte(), settings, record, pLogger, pHandler);

	case(QualifierCode::UINT16_FREE_FORMAT) :
		return FreeFormatParser::ParseHeader(buffer, settings, record, pLogger, pHandler);

	default:
		FORMAT_LOGGER_BLOCK(pLogger, flags::WARN, "Unknown qualifier %x", record.qualifier);
		return ParseResult::UNKNOWN_QUALIFIER;
	}
}

ParseResult APDUParser::HandleAllObjectsHeader(openpal::Logger* pLogger, const HeaderRecord& record, const ParserSettings& settings, IAPDUHandler* pHandler)
{
	FORMAT_LOGGER_BLOCK(pLogger, settings.Filters(),
	                    "%03u,%03u - %s - %s",
	                    record.group,
	                    record.variation,
	                    GroupVariationToString(record.enumeration),
	                    QualifierCodeToString(QualifierCode::ALL_OBJECTS));

	if (pHandler)
	{
		pHandler->OnHeader(AllObjectsHeader(record));
	}

	return ParseResult::OK;
}

}

