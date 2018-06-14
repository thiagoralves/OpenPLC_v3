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
#ifndef OPENDNP3_OBJECTHEADERPARSER_H
#define OPENDNP3_OBJECTHEADERPARSER_H

#include <openpal/util/Uncopyable.h>

#include "opendnp3/app/parsing/ParseResult.h"
#include <openpal/container/RSlice.h>
#include <openpal/logging/Logger.h>

#include <opendnp3/gen/GroupVariation.h>

namespace opendnp3
{

class ObjectHeader
{
public:
	ObjectHeader();

	uint8_t group;
	uint8_t variation;
	uint8_t qualifier;
};

class ObjectHeaderParser : private openpal::StaticOnly
{
public:

	static ParseResult ParseObjectHeader(ObjectHeader& header, openpal::RSlice& buffer, openpal::Logger* pLogger);

	// Provides stateless inspection of the first object header, provided that it exists
	// return false on failure (e.g. < 3 bytes)
	static bool ReadFirstGroupVariation(const openpal::RSlice& objects, GroupVariation& gv);
};

}

#endif
