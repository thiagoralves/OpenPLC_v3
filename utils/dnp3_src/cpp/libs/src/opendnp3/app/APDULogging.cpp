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

#include "APDULogging.h"

#include "opendnp3/app/parsing/APDUHeaderParser.h"
#include "opendnp3/app/parsing/APDUParser.h"

#include <openpal/logging/LogMacros.h>

namespace opendnp3
{
namespace logging
{

void ParseAndLogRequestTx(openpal::Logger logger, const openpal::RSlice& apdu)
{

#ifndef OPENPAL_STRIP_LOGGING

	FORMAT_HEX_BLOCK(logger, flags::APP_HEX_TX, apdu, 18, 18);

	if (logger.IsEnabled(flags::APP_HEADER_TX))
	{
		APDUHeader header;
		if (APDUHeaderParser::ParseRequest(apdu, header, &logger))
		{
			FORMAT_LOG_BLOCK(logger, flags::APP_HEADER_TX,
			                 "FIR: %i FIN: %i CON: %i UNS: %i SEQ: %i FUNC: %s",
			                 header.control.FIR,
			                 header.control.FIN,
			                 header.control.CON,
			                 header.control.UNS,
			                 header.control.SEQ,
			                 FunctionCodeToString(header.function));

			if (logger.IsEnabled(flags::APP_OBJECT_TX))
			{
				auto expectsContents = header.function != FunctionCode::READ;
				APDUParser::ParseAndLogAll(apdu.Skip(APDU_REQUEST_HEADER_SIZE), &logger, ParserSettings::Create(expectsContents, flags::APP_OBJECT_TX));
			}
		}
	}

#endif

}

void ParseAndLogResponseTx(openpal::Logger logger, const openpal::RSlice& apdu)
{

#ifndef OPENPAL_STRIP_LOGGING

	FORMAT_HEX_BLOCK(logger, flags::APP_HEX_TX, apdu, 18, 18);

	if (logger.IsEnabled(flags::APP_HEADER_TX))
	{
		APDUResponseHeader header;
		if (APDUHeaderParser::ParseResponse(apdu, header, &logger))
		{
			FORMAT_LOG_BLOCK(logger, flags::APP_HEADER_TX,
			                 "FIR: %i FIN: %i CON: %i UNS: %i SEQ: %i FUNC: %s IIN: [0x%02x, 0x%02x]",
			                 header.control.FIR,
			                 header.control.FIN,
			                 header.control.CON,
			                 header.control.UNS,
			                 header.control.SEQ,
			                 FunctionCodeToString(header.function),
			                 header.IIN.LSB,
			                 header.IIN.MSB);

			if (logger.IsEnabled(flags::APP_OBJECT_TX))
			{
				APDUParser::ParseAndLogAll(apdu.Skip(APDU_RESPONSE_HEADER_SIZE), &logger, ParserSettings::Create(true, flags::APP_OBJECT_TX));
			}
		}
	}

#endif

}

}
}

