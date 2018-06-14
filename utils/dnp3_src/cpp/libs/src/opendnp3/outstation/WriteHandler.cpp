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
#include "WriteHandler.h"

#include "openpal/logging/LogMacros.h"

using namespace openpal;

namespace opendnp3
{

WriteHandler::WriteHandler(IOutstationApplication& application, IINField* pWriteIIN_) :
	pApplication(&application),
	pWriteIIN(pWriteIIN_),
	wroteTime(false),
	wroteIIN(false)
{}

IINField WriteHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<IINValue>>& values)
{
	Indexed<IINValue> pair;

	if (!values.ReadOnlyValue(pair))
	{
		return IINBit::PARAM_ERROR;
	}

	if (wroteIIN)
	{
		return IINBit::PARAM_ERROR;
	}

	if (pair.index != static_cast<uint16_t>(IINBit::DEVICE_RESTART))
	{
		return IINBit::PARAM_ERROR;
	}

	if (pair.value.value)
	{
		return IINBit::PARAM_ERROR;
	}

	wroteIIN = true;
	pWriteIIN->ClearBit(IINBit::DEVICE_RESTART);
	return IINField();
}

IINField WriteHandler::ProcessHeader(const CountHeader& header, const ICollection<Group50Var1>& values)
{
	if (wroteTime)
	{
		return IINBit::PARAM_ERROR;
	}
	else
	{
		Group50Var1 value;
		if (values.ReadOnlyValue(value))
		{
			if (pApplication->SupportsWriteAbsoluteTime())
			{
				wroteTime = true;
				return pApplication->WriteAbsoluteTime(UTCTimestamp(value.time)) ? IINField::Empty() : IINBit::PARAM_ERROR;
			}
			else
			{
				return IINBit::FUNC_NOT_SUPPORTED;
			}
		}
		else
		{
			return IINBit::PARAM_ERROR;
		}
	}
}

IINField  WriteHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<TimeAndInterval>>& values)
{
	if (!pApplication->SupportsWriteTimeAndInterval())
	{
		return IINBit::FUNC_NOT_SUPPORTED;
	}

	return pApplication->WriteTimeAndInterval(values) ? IINField::Empty() : IINBit::PARAM_ERROR;
}

}


