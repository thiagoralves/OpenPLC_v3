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
#ifndef OPENDNP3_FREEFORMATPARSER_H
#define OPENDNP3_FREEFORMATPARSER_H

#include <openpal/util/Uncopyable.h>
#include <openpal/container/RSlice.h>
#include <openpal/logging/Logger.h>

#include "opendnp3/app/parsing/ObjectHeaderParser.h"
#include "opendnp3/app/parsing/ParseResult.h"
#include "opendnp3/app/parsing/IAPDUHandler.h"
#include "opendnp3/app/parsing/ParserSettings.h"

namespace opendnp3
{

/**
	Parser for free format (0x5B)
*/
class FreeFormatParser : private openpal::StaticOnly
{
public:

	static ParseResult ParseHeader(openpal::RSlice& buffer, const ParserSettings& settings, const HeaderRecord& record, openpal::Logger* pLogger, IAPDUHandler* pHandler);

private:

	typedef bool(&FreeFormatHandler)(const FreeFormatHeader& header, const openpal::RSlice& objects, IAPDUHandler* pHandler);

	static ParseResult ParseFreeFormat(FreeFormatHandler handler, const FreeFormatHeader& header, uint16_t size, openpal::RSlice& objects, IAPDUHandler* pHandler, openpal::Logger* pLogger);

	// Free format handlers

	template <class T>
	static bool ParseAny(const FreeFormatHeader& header, const openpal::RSlice& object, IAPDUHandler* pHandler)
	{
		T value;
		auto success = value.Read(object);
		if (success && pHandler)
		{
			pHandler->OnHeader(header, value, object);
		}
		return success;
	}
};

}

#endif

