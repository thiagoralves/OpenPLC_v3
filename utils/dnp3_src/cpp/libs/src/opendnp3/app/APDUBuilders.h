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
#ifndef OPENDNP3_APDUBUILDERS_H
#define OPENDNP3_APDUBUILDERS_H

#include "opendnp3/app/APDURequest.h"
#include "opendnp3/app/APDUResponse.h"
#include "opendnp3/app/ClassField.h"

namespace opendnp3
{
namespace build
{
// -------- requests -------------

void ClassRequest(APDURequest& request, FunctionCode code, const ClassField& classes, uint8_t seq);

bool WriteClassHeaders(HeaderWriter& writer, const ClassField& classes);

void ReadIntegrity(APDURequest& request, const ClassField& classes, uint8_t seq = 0);

void ReadAllObjects(APDURequest& request, GroupVariationID gvId, uint8_t seq = 0);

void DisableUnsolicited(APDURequest& request, uint8_t seq = 0);

void EnableUnsolicited(APDURequest& request, const ClassField& classes, uint8_t seq = 0);

void ClearRestartIIN(APDURequest& request, uint8_t seq = 0);

void MeasureDelay(APDURequest& request, uint8_t seq = 0);

// -------- responses -------------

void NullUnsolicited(APDUResponse& response, uint8_t seq, const IINField& iin);

}
}

#endif
