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

#include "DecoderImpl.h"

#include "dnp3decode/Indent.h"

#include "opendnp3/app/parsing/APDUHeaderParser.h"
#include "opendnp3/app/parsing/APDUParser.h"
#include "dnp3decode/LoggingHandler.h"

#include <openpal/logging/LogMacros.h>

using namespace openpal;

namespace opendnp3
{

DecoderImpl::DecoderImpl(IDecoderCallbacks& callbacks, const openpal::Logger& logger) :
	callbacks(&callbacks),
	logger(logger),
	link(logger),
	transportRx(logger, 2048)
{}

void DecoderImpl::DecodeLPDU(const openpal::RSlice& data)
{
	Indent i(*callbacks);

	RSlice remaining(data);

	while (remaining.IsNotEmpty())
	{
		auto dest = this->link.WriteBuff();

		const auto NUM = (remaining.Size() > dest.Size()) ? dest.Size() : remaining.Size();

		remaining.Take(NUM).CopyTo(dest);
		link.OnRead(NUM, *this);

		remaining.Advance(NUM);
	}

}

void DecoderImpl::DecodeTPDU(const openpal::RSlice& data)
{
	Indent i(*callbacks);
	FORMAT_HEX_BLOCK(logger, flags::TRANSPORT_RX, data, 18, 18);

	auto asdu = transportRx.ProcessReceive(data);
	if (asdu.IsNotEmpty())
	{
		this->DecodeAPDU(asdu);
	}
}

void DecoderImpl::DecodeAPDU(const openpal::RSlice& data)
{
	Indent i(*callbacks);

	FORMAT_HEX_BLOCK(this->logger, flags::APP_HEX_RX, data, 18, 18);

	if (IsResponse(data))
	{
		APDUResponseHeader header;
		if (APDUHeaderParser::ParseResponse(data, header, &logger))
		{
			FORMAT_LOG_BLOCK(this->logger, flags::APP_HEADER_RX,
			                 "FIR: %i FIN: %i CON: %i UNS: %i SEQ: %i FUNC: %s IIN: [0x%02x, 0x%02x]",
			                 header.control.FIR,
			                 header.control.FIN,
			                 header.control.CON,
			                 header.control.UNS,
			                 header.control.SEQ,
			                 FunctionCodeToString(header.function),
			                 header.IIN.LSB,
			                 header.IIN.MSB);
			if (header.IIN.LSB & 0x01) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN1.0 - All stations");
			if (header.IIN.LSB & 0x02) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN1.1 - Class 1 events");
			if (header.IIN.LSB & 0x04) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN1.2 - Class 2 events");
			if (header.IIN.LSB & 0x08) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN1.3 - Class 3 events");
			if (header.IIN.LSB & 0x10) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN1.4 - Need time");
			if (header.IIN.LSB & 0x20) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN1.5 - Local control");
			if (header.IIN.LSB & 0x40) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN1.6 - Device trouble");
			if (header.IIN.LSB & 0x80) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN1.7 - Device restart");
			if (header.IIN.MSB & 0x01) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN2.0 - Function code not supported");
			if (header.IIN.MSB & 0x02) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN2.1 - Object unknown");
			if (header.IIN.MSB & 0x04) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN2.2 - Parameter error");
			if (header.IIN.MSB & 0x08) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN2.3 - Event buffer overflow");
			if (header.IIN.MSB & 0x10) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN2.4 - Already executing");
			if (header.IIN.MSB & 0x20) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN2.5 - Configuration corrupt");
			if (header.IIN.MSB & 0x40) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN2.6 - Reserved 1");
			if (header.IIN.MSB & 0x80) SIMPLE_LOG_BLOCK(this->logger, flags::APP_HEADER_RX, "IIN2.7 - Reserved 2");

			Indent i(*callbacks);
			LoggingHandler handler(logger, *callbacks);
			APDUParser::ParseSinglePass(data.Skip(4), &logger, &handler, nullptr, ParserSettings::Default());
		}
	}
	else
	{
		APDUHeader header;
		if (APDUHeaderParser::ParseRequest(data, header, &logger))
		{

			FORMAT_LOG_BLOCK(this->logger, flags::APP_HEADER_RX,
			                 "FIR: %i FIN: %i CON: %i UNS: %i SEQ: %i FUNC: %s",
			                 header.control.FIR,
			                 header.control.FIN,
			                 header.control.CON,
			                 header.control.UNS,
			                 header.control.SEQ,
			                 FunctionCodeToString(header.function));


			Indent i(*callbacks);
			LoggingHandler handler(logger, *callbacks);
			auto settings = (header.function == FunctionCode::READ) ? ParserSettings::NoContents() : ParserSettings::Default();
			APDUParser::ParseSinglePass(data.Skip(2), &logger, &handler, nullptr, settings);
		}
	}
}

bool DecoderImpl::IsResponse(const openpal::RSlice& data)
{
	if (data.Size() < 2)
	{
		return false;
	}

	switch (FunctionCodeFromType(data[1]))
	{
	case(FunctionCode::RESPONSE) :
	case(FunctionCode::UNSOLICITED_RESPONSE) :
	case(FunctionCode::AUTH_RESPONSE) :
		return true;
	default:
		return false;
	}
}

bool DecoderImpl::OnFrame(const LinkHeaderFields& header, const openpal::RSlice& userdata)
{
	if (header.func == LinkFunction::PRI_CONFIRMED_USER_DATA || header.func == LinkFunction::PRI_UNCONFIRMED_USER_DATA)
	{
		this->DecodeTPDU(userdata);
	}

	return true;
}

}


