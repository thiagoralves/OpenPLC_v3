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
/**
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

#include "FreeFormatParser.h"

#include "opendnp3/app/parsing/ObjectHeaderParser.h"
#include "opendnp3/app/GroupVariationRecord.h"
#include "opendnp3/LogLevels.h"

#include <openpal/serialization/Serialization.h>
#include <openpal/logging/LogMacros.h>


using namespace openpal;

namespace opendnp3
{
ParseResult FreeFormatParser::ParseHeader(openpal::RSlice& buffer, const ParserSettings& settings, const HeaderRecord& record, openpal::Logger* pLogger, IAPDUHandler* pHandler)
{
	if (buffer.Size() < 3)
	{
		SIMPLE_LOGGER_BLOCK(pLogger, flags::WARN, "Not enough data for free format count and size");
		return ParseResult::NOT_ENOUGH_DATA_FOR_HEADER;
	}

	const uint8_t FREE_FORMAT_COUNT = UInt8::ReadBuffer(buffer);
	const uint16_t FREE_FORMAT_SIZE = UInt16::ReadBuffer(buffer);

	FORMAT_LOGGER_BLOCK(pLogger, settings.Filters(),
	                    "%03u,%03u %s, %s, count: %u size: %u",
	                    record.group,
	                    record.variation,
	                    GroupVariationToString(record.enumeration),
	                    QualifierCodeToString(record.GetQualifierCode()),
	                    FREE_FORMAT_COUNT,
	                    FREE_FORMAT_SIZE
	                   );

	if (FREE_FORMAT_COUNT != 1)
	{
		FORMAT_LOGGER_BLOCK(pLogger, flags::WARN, "Unsupported free-format count of %u", FREE_FORMAT_COUNT);
		return ParseResult::UNREASONABLE_OBJECT_COUNT;
	}

	if (buffer.Size() < FREE_FORMAT_SIZE)
	{
		FORMAT_LOGGER_BLOCK(pLogger, flags::WARN, "Insufficient data (%u) for free format object of size (%u)", buffer.Size(), FREE_FORMAT_SIZE);
		return ParseResult::NOT_ENOUGH_DATA_FOR_OBJECTS;
	}

	RSlice copy(buffer.Take(FREE_FORMAT_SIZE));
	buffer.Advance(FREE_FORMAT_SIZE);

	FreeFormatHeader header(record, FREE_FORMAT_COUNT);

	switch (record.enumeration)
	{
	case(GroupVariation::Group120Var1) :
		return ParseFreeFormat(ParseAny<Group120Var1>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var2) :
		return ParseFreeFormat(ParseAny<Group120Var2>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var5) :
		return ParseFreeFormat(ParseAny<Group120Var5>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var6) :
		return ParseFreeFormat(ParseAny<Group120Var6>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var7) :
		return ParseFreeFormat(ParseAny<Group120Var7>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var9) :
		return ParseFreeFormat(ParseAny<Group120Var9>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var8) :
		return ParseFreeFormat(ParseAny<Group120Var8>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var10) :
		return ParseFreeFormat(ParseAny<Group120Var10>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var11) :
		return ParseFreeFormat(ParseAny<Group120Var11>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var12) :
		return ParseFreeFormat(ParseAny<Group120Var12>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var13) :
		return ParseFreeFormat(ParseAny<Group120Var13>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var14) :
		return ParseFreeFormat(ParseAny<Group120Var14>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	case(GroupVariation::Group120Var15) :
		return ParseFreeFormat(ParseAny<Group120Var15>, header, FREE_FORMAT_SIZE, copy, pHandler, pLogger);

	default:
		FORMAT_LOGGER_BLOCK(pLogger, flags::WARN,
		                    "Unsupported qualifier/object - %s - %i / %i",
		                    QualifierCodeToString(record.GetQualifierCode()), record.group, record.variation
		                   );

		return ParseResult::INVALID_OBJECT_QUALIFIER;
	}

}

ParseResult FreeFormatParser::ParseFreeFormat(FreeFormatHandler parser, const FreeFormatHeader& header, uint16_t size, openpal::RSlice& objects, IAPDUHandler* pHandler, openpal::Logger* pLogger)
{
	if (parser(header, objects, pHandler))
	{
		return ParseResult::OK;
	}
	else
	{
		FORMAT_LOGGER_BLOCK(pLogger, flags::WARN, "Insufficient data for free-format object: (%i, %i)", header.group, header.variation);
		return ParseResult::NOT_ENOUGH_DATA_FOR_OBJECTS;
	}
}

}
