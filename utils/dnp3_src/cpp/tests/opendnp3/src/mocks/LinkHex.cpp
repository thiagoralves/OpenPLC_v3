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
#include "LinkHex.h"

#include <openpal/container/StaticBuffer.h>
#include <opendnp3/link/LinkFrame.h>

#include <testlib/HexConversions.h>
#include <testlib/BufferHelpers.h>

using namespace openpal;
using namespace testlib;

namespace opendnp3
{
std::string LinkHex::Ack(bool master, bool isRxBuffFull, uint16_t dest, uint16_t src)
{
	StaticBuffer<292> buffer;
	auto ouput = buffer.GetWSlice();
	return ToHex(LinkFrame::FormatAck(ouput, master, isRxBuffFull, dest, src, nullptr));
}

std::string LinkHex::Nack(bool master, bool isRxBuffFull, uint16_t dest, uint16_t src)
{
	StaticBuffer<292> buffer;
	auto ouput = buffer.GetWSlice();
	return ToHex(LinkFrame::FormatNack(ouput, master, isRxBuffFull, dest, src, nullptr));
}

std::string LinkHex::LinkStatus(bool master, bool isRxBuffFull, uint16_t dest, uint16_t src)
{
	StaticBuffer<292> buffer;
	auto ouput = buffer.GetWSlice();
	return ToHex(LinkFrame::FormatLinkStatus(ouput, master, isRxBuffFull, dest, src, nullptr));
}

std::string LinkHex::NotSupported(bool master, bool isRxBuffFull, uint16_t dest, uint16_t src)
{
	StaticBuffer<292> buffer;
	auto ouput = buffer.GetWSlice();
	return ToHex(LinkFrame::FormatNotSupported(ouput, master, isRxBuffFull, dest, src, nullptr));
}

std::string LinkHex::TestLinkStatus(bool master, bool fcb, uint16_t dest, uint16_t src)
{
	StaticBuffer<292> buffer;
	auto ouput = buffer.GetWSlice();
	return ToHex(LinkFrame::FormatTestLinkStatus(ouput, master, fcb, dest, src, nullptr));
}

std::string LinkHex::ResetLinkStates(bool master, uint16_t dest, uint16_t src)
{
	StaticBuffer<292> buffer;
	auto ouput = buffer.GetWSlice();
	return ToHex(LinkFrame::FormatResetLinkStates(ouput, master, dest, src, nullptr));
}

std::string LinkHex::RequestLinkStatus(bool master, uint16_t dest, uint16_t src)
{
	StaticBuffer<292> buffer;
	auto ouput = buffer.GetWSlice();
	return ToHex(LinkFrame::FormatRequestLinkStatus(ouput, master, dest, src, nullptr));
}

std::string LinkHex::ConfirmedUserData(bool master, bool fcb, uint16_t dest, uint16_t src, const std::string& userDataHex)
{
	HexSequence hs(userDataHex);
	auto data = hs.ToRSlice();

	StaticBuffer<292> buffer;
	auto ouput = buffer.GetWSlice();
	return ToHex(LinkFrame::FormatConfirmedUserData(ouput, master, fcb, dest, src, data, data.Size(), nullptr));
}

std::string LinkHex::UnconfirmedUserData(bool master, uint16_t dest, uint16_t src, const std::string& userDataHex)
{
	HexSequence hs(userDataHex);
	auto data = hs.ToRSlice();

	StaticBuffer<292> buffer;
	auto ouput = buffer.GetWSlice();
	return ToHex(LinkFrame::FormatUnconfirmedUserData(ouput, master, dest, src, data, data.Size(), nullptr));
}

}



