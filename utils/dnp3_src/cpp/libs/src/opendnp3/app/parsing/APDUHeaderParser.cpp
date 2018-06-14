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
#include "APDUHeaderParser.h"

#include <openpal/util/Uncopyable.h>
#include <openpal/container/RSlice.h>
#include <openpal/logging/LogMacros.h>

#include "opendnp3/LogLevels.h"
#include "opendnp3/app/APDUHeader.h"

namespace opendnp3
{

bool APDUHeaderParser::ParseRequest(const openpal::RSlice& apdu, APDUHeader& header, openpal::Logger* pLogger)
{
	if (apdu.Size() < APDU_REQUEST_HEADER_SIZE)
	{
		FORMAT_LOGGER_BLOCK(pLogger, flags::WARN, "Request fragment  with insufficient size of %u bytes", apdu.Size());
		return false;
	}
	else
	{
		header.control = AppControlField(apdu[0]);
		header.function = FunctionCodeFromType(apdu[1]);
		return true;
	}
}

bool APDUHeaderParser::ParseResponse(const openpal::RSlice& apdu, APDUResponseHeader& header, openpal::Logger* pLogger)
{
	if (apdu.Size() < APDU_RESPONSE_HEADER_SIZE)
	{
		FORMAT_LOGGER_BLOCK(pLogger, flags::WARN, "Response fragment  with insufficient size of %u bytes", apdu.Size());
		return false;
	}
	else
	{
		header.control = AppControlField(apdu[0]);
		header.function = FunctionCodeFromType(apdu[1]);
		header.IIN.LSB = apdu[2];
		header.IIN.MSB = apdu[3];
		return true;
	}
}

}

