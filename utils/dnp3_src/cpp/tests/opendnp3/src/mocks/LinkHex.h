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
#ifndef OPENDNP3_LINK_HEX_H
#define OPENDNP3_LINK_HEX_H

#include "opendnp3/gen/FunctionCode.h"
#include "opendnp3/gen/LinkFunction.h"

#include <openpal/container/RSlice.h>
#include <openpal/util/Uncopyable.h>

#include <string>

namespace opendnp3
{

class LinkHex : private openpal::StaticOnly
{

public:

	////////////////////////////////////////////////
	//	Functions for formatting outgoing Sec to Pri frames
	////////////////////////////////////////////////

	static std::string Ack(bool master, bool isRxBuffFull, uint16_t dest, uint16_t src);
	static std::string Nack(bool master, bool isRxBuffFull, uint16_t dest, uint16_t src);
	static std::string LinkStatus(bool master, bool isRxBuffFull, uint16_t dest, uint16_t src);
	static std::string NotSupported(bool master, bool isRxBuffFull, uint16_t dest, uint16_t src);

	////////////////////////////////////////////////
	//	Functions for formatting outgoing Pri to Sec frames
	////////////////////////////////////////////////

	static std::string TestLinkStatus(bool master, bool fcb, uint16_t dest, uint16_t src);
	static std::string ResetLinkStates(bool master, uint16_t dest, uint16_t src);
	static std::string RequestLinkStatus(bool master, uint16_t dest, uint16_t src);
	static std::string ConfirmedUserData(bool master, bool fcb, uint16_t dest, uint16_t src, const std::string& userDataHex);
	static std::string UnconfirmedUserData(bool master, uint16_t dest, uint16_t src, const std::string& userDataHex);
};

}

#endif

