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
#ifndef OPENDNP3_COUNTPARSER_H
#define OPENDNP3_COUNTPARSER_H

#include <openpal/container/RSlice.h>
#include <openpal/logging/Logger.h>

#include "opendnp3/app/parsing/Functions.h"
#include "opendnp3/app/parsing/ParseResult.h"
#include "opendnp3/app/parsing/IAPDUHandler.h"
#include "opendnp3/app/parsing/ParseResult.h"
#include "opendnp3/app/parsing/NumParser.h"
#include "opendnp3/app/parsing/ParserSettings.h"
#include "opendnp3/app/parsing/BufferedCollection.h"

namespace opendnp3
{

class CountParser
{
	typedef void (*HandleFun)(const HeaderRecord& record, uint16_t count, const openpal::RSlice& buffer, IAPDUHandler& handler);

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

	// Process the count handler against the buffer
	ParseResult Process(const HeaderRecord& record, openpal::RSlice& buffer, IAPDUHandler* pHandler, openpal::Logger* pLogger) const;

	// Create a count handler from a fixed size descriptor
	template <class Descriptor>
	static CountParser From(uint16_t count);

	static ParseResult ParseCountOfObjects(openpal::RSlice& buffer, const HeaderRecord& record, uint16_t count, openpal::Logger* pLogger, IAPDUHandler* pHandler);

	template <class Descriptor>
	static void InvokeCountOf(const HeaderRecord& record, uint16_t count, const openpal::RSlice& buffer, IAPDUHandler& handler);

	CountParser(uint16_t count, uint32_t requiredSize, HandleFun handler);

	uint16_t count;
	uint32_t requiredSize;
	HandleFun handler;

	CountParser() = delete;
};

template <class Descriptor>
CountParser CountParser::From(uint16_t count)
{
	uint32_t size = static_cast<uint32_t>(count) * Descriptor::Size();
	return CountParser(count, size, &InvokeCountOf<Descriptor>);
}

template <class T>
void CountParser::InvokeCountOf(const HeaderRecord& record, uint16_t count, const openpal::RSlice& buffer, IAPDUHandler& handler)
{
	auto read = [](openpal::RSlice & buffer, uint32_t) -> T
	{
		T value;
		T::Read(buffer, value);
		return value;
	};

	auto collection = CreateBufferedCollection<T>(buffer, count, read);
	handler.OnHeader(CountHeader(record, count), collection);
}

}

#endif
