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
#include "MeasurementHandler.h"

#include "opendnp3/app/parsing/APDUParser.h"

#include <openpal/logging/LogMacros.h>

using namespace openpal;

namespace opendnp3
{

ParseResult MeasurementHandler::ProcessMeasurements(const openpal::RSlice& objects, openpal::Logger& logger, ISOEHandler* pHandler)
{
	MeasurementHandler handler(logger, pHandler);
	return APDUParser::Parse(objects, handler, &logger);
}

MeasurementHandler::MeasurementHandler(const openpal::Logger& logger_, ISOEHandler* pSOEHandler_) :
	logger(logger_),
	txInitiated(false),
	pSOEHandler(pSOEHandler_),
	ctoMode(TimestampMode::INVALID),
	commonTimeOccurence(0)
{

}

MeasurementHandler::~MeasurementHandler()
{
	if (txInitiated)
	{
		Transaction::End(pSOEHandler);
	}
}

TimestampMode MeasurementHandler::ModeFromType(GroupVariation gv)
{
	return HasAbsoluteTime(gv) ? TimestampMode::SYNCHRONIZED : TimestampMode::INVALID;
}


void MeasurementHandler::CheckForTxStart()
{
	if (!txInitiated)
	{
		txInitiated = true;
		Transaction::Start(pSOEHandler);
	}
}

IINField MeasurementHandler::ProcessHeader(const CountHeader& header, const ICollection<Group50Var1>& values)
{
	this->CheckForTxStart();

	auto transform = [](const Group50Var1 & input) -> DNPTime
	{
		return input.time;
	};

	auto collection = Map<Group50Var1, DNPTime>(values, transform);

	HeaderInfo info(header.enumeration, header.GetQualifierCode(), TimestampMode::INVALID, header.headerIndex);
	this->pSOEHandler->Process(info, collection);

	return IINField();
}

IINField MeasurementHandler::ProcessHeader(const CountHeader& header, const ICollection<Group51Var1>& values)
{
	Group51Var1 cto;
	if (values.ReadOnlyValue(cto))
	{
		ctoMode = TimestampMode::SYNCHRONIZED;
		commonTimeOccurence = cto.time;
	}
	return IINField::Empty();
}

IINField MeasurementHandler::ProcessHeader(const CountHeader& header, const ICollection<Group51Var2>& values)
{
	Group51Var2 cto;
	if (values.ReadOnlyValue(cto))
	{
		ctoMode = TimestampMode::UNSYNCHRONIZED;
		commonTimeOccurence = cto.time;
	}

	return IINField::Empty();
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Binary>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<DoubleBitBinary>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<BinaryOutputStatus>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Counter>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<FrozenCounter>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Analog>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<AnalogOutputStatus>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<OctetString>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<TimeAndInterval>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Group121Var1>>& values)
{
	return this->LoadValuesWithTransformTo<SecurityStat>(header, values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Binary>>& values)
{
	if (header.enumeration == GroupVariation::Group2Var3)
	{
		return this->ProcessWithCTO(header, values);
	}
	else
	{
		return this->LoadValues(header, ModeFromType(header.enumeration), values);
	}
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<BinaryOutputStatus>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<DoubleBitBinary>>& values)
{
	if (header.enumeration == GroupVariation::Group4Var3)
	{
		return this->ProcessWithCTO(header, values);
	}
	else
	{
		return this->LoadValues(header, ModeFromType(header.enumeration), values);
	}
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Counter>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<FrozenCounter>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Analog>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputStatus>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<OctetString>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<BinaryCommandEvent>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogCommandEvent>>& values)
{
	return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Group122Var1>>& values)
{
	return this->LoadValuesWithTransformTo<SecurityStat>(header, values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Group122Var2>>& values)
{
	return this->LoadValuesWithTransformTo<SecurityStat>(header, values);
}

// ----------- Conversion routines from DNP3 types to API types -----------

SecurityStat MeasurementHandler::Convert(const Group121Var1& meas)
{
	return SecurityStat(meas.flags, meas.assocId, meas.value);
}

SecurityStat MeasurementHandler::Convert(const Group122Var1& meas)
{
	return SecurityStat(meas.flags, meas.assocId, meas.value);
}

SecurityStat MeasurementHandler::Convert(const Group122Var2& meas)
{
	return SecurityStat(meas.flags, meas.assocId, meas.value, meas.time);
}

}


