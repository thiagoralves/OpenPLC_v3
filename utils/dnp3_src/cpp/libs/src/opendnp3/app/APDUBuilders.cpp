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
#include "APDUBuilders.h"

#include "opendnp3/objects/Group60.h"
#include "opendnp3/app/IINField.h"

#include <openpal/serialization/Serialization.h>


namespace opendnp3
{
namespace build
{

void ReadIntegrity(APDURequest& request, const ClassField& classes, uint8_t seq)
{
	ClassRequest(request, FunctionCode::READ, classes, seq);
}

void ReadAllObjects(APDURequest& request, GroupVariationID gvId, uint8_t seq)
{
	request.SetControl(AppControlField(true, true, false, false, seq));
	request.SetFunction(FunctionCode::READ);
	auto writer = request.GetWriter();
	writer.WriteHeader(gvId, QualifierCode::ALL_OBJECTS);
}

void ClassRequest(APDURequest& request, FunctionCode fc, const ClassField& classes, uint8_t seq)
{
	request.SetControl(AppControlField(true, true, false, false, seq));
	request.SetFunction(fc);
	auto writer = request.GetWriter();
	WriteClassHeaders(writer, classes);
}

bool WriteClassHeaders(HeaderWriter& writer, const ClassField& classes)
{
	if (classes.HasClass1())
	{
		if (!writer.WriteHeader(Group60Var2::ID(), QualifierCode::ALL_OBJECTS))
		{
			return false;
		}
	}
	if (classes.HasClass2())
	{
		if (!writer.WriteHeader(Group60Var3::ID(), QualifierCode::ALL_OBJECTS))
		{
			return false;
		}
	}
	if (classes.HasClass3())
	{
		if (!writer.WriteHeader(Group60Var4::ID(), QualifierCode::ALL_OBJECTS))
		{
			return false;
		}
	}
	if (classes.HasClass0())
	{
		if (!writer.WriteHeader(Group60Var1::ID(), QualifierCode::ALL_OBJECTS))
		{
			return false;
		}
	}

	return true;
}

void DisableUnsolicited(APDURequest& request, uint8_t seq)
{
	ClassRequest(request, FunctionCode::DISABLE_UNSOLICITED, ClassField::AllEventClasses(), seq);
}

void EnableUnsolicited(APDURequest& request, const ClassField& classes, uint8_t seq)
{
	ClassRequest(request, FunctionCode::ENABLE_UNSOLICITED, classes, seq);
}

void ClearRestartIIN(APDURequest& request, uint8_t seq)
{
	request.SetFunction(FunctionCode::WRITE);
	request.SetControl(AppControlField(true, true, false, false, seq));
	auto writer = request.GetWriter();
	auto iter = writer.IterateOverSingleBitfield<openpal::UInt8>(GroupVariationID(80, 1), QualifierCode::UINT8_START_STOP, static_cast<uint8_t>(IINBit::DEVICE_RESTART));
	iter.Write(false);
}

void MeasureDelay(APDURequest& request, uint8_t seq)
{
	request.SetFunction(FunctionCode::DELAY_MEASURE);
	request.SetControl(AppControlField::Request(seq));
}

void NullUnsolicited(APDUResponse& response, uint8_t seq, const IINField& iin)
{
	response.SetControl(AppControlField(true, true, true, true, seq));
	response.SetFunction(FunctionCode::UNSOLICITED_RESPONSE);
	response.SetIIN(iin);
}

}
}

