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

#ifndef OPENDNP3_LOGLEVELS_H
#define OPENDNP3_LOGLEVELS_H

#include <openpal/logging/LogLevels.h>

namespace opendnp3
{

const char* LogFlagToString(int32_t flag);

namespace flags
{

// define most of these in terms of the base openpal filters
const int32_t EVENT = openpal::logflags::EVENT;
const int32_t ERR = openpal::logflags::ERR;
const int32_t WARN = openpal::logflags::WARN;
const int32_t INFO = openpal::logflags::INFO;
const int32_t DBG = openpal::logflags::DBG;

// up-shift the custom dnp3 filters

const int32_t LINK_RX = DBG << 1;
const int32_t LINK_RX_HEX = DBG << 2;

const int32_t LINK_TX = DBG << 3;
const int32_t LINK_TX_HEX = DBG << 4;

const int32_t TRANSPORT_RX = DBG << 5;
const int32_t TRANSPORT_TX = DBG << 6;

const int32_t APP_HEADER_RX = DBG << 7;
const int32_t APP_HEADER_TX = DBG << 8;

const int32_t APP_OBJECT_RX = DBG << 9;
const int32_t APP_OBJECT_TX = DBG << 10;

const int32_t APP_HEX_RX = DBG << 11;
const int32_t APP_HEX_TX = DBG << 12;

}

namespace levels
{

const int32_t NOTHING = 0;
const int32_t ALL = ~NOTHING;
const int32_t NORMAL = flags::EVENT | flags::ERR | flags::WARN | flags::INFO;
const int32_t ALL_APP_COMMS = flags::APP_HEADER_RX | flags::APP_HEADER_TX | flags::APP_OBJECT_RX | flags::APP_OBJECT_TX | flags::APP_HEX_RX | flags::APP_HEX_TX;
const int32_t ALL_COMMS = flags::LINK_RX | flags::LINK_TX | flags::TRANSPORT_RX | flags::TRANSPORT_TX | ALL_APP_COMMS;

}

}

#endif
