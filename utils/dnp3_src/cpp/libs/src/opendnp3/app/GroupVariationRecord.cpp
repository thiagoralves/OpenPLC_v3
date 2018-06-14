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
#include "GroupVariationRecord.h"

namespace opendnp3
{

GroupVariationRecord::GroupVariationRecord(uint8_t group_, uint8_t variation_, GroupVariation enumeration_, GroupVariationType type_) :
	enumeration(enumeration_),
	type(type_),
	group(group_),
	variation(variation_)
{

}

HeaderRecord::HeaderRecord(const GroupVariationRecord& gv, uint8_t qualifier_, uint32_t headerIndex_) :
	GroupVariationRecord(gv),
	qualifier(qualifier_),
	headerIndex(headerIndex_)
{

}

QualifierCode HeaderRecord::GetQualifierCode() const
{
	return QualifierCodeFromType(qualifier);
}

GroupVariationRecord GroupVariationRecord::GetRecord(uint8_t group, uint8_t variation)
{
	auto pair = GetEnumAndType(group, variation);
	return GroupVariationRecord(group, variation, pair.enumeration, pair.type);
}

uint16_t GroupVariationRecord::GetGroupVar(uint8_t group, uint8_t variation)
{
	return (group << 8) | variation;
}

EnumAndType GroupVariationRecord::GetEnumAndType(uint8_t group, uint8_t variation)
{
	auto type = GetType(group, variation);
	auto enumeration = GroupVariationFromType(GetGroupVar(group, variation));

	if (enumeration == GroupVariation::UNKNOWN)
	{
		switch (group)
		{
		case(110) :
			enumeration = GroupVariation::Group110Var0;
			break;
		case(111) :
			enumeration = GroupVariation::Group111Var0;
			break;
		case(112) :
			enumeration = GroupVariation::Group112Var0;
			break;
		case(113) :
			enumeration = GroupVariation::Group113Var0;
			break;
		default:
			break;
		}
	}

	return EnumAndType(enumeration, type);
}

GroupVariationType GroupVariationRecord::GetType(uint8_t group, uint8_t variation)
{
	switch (group)
	{
	case(1) :
		return GroupVariationType::STATIC;

	case(2) :
		return GroupVariationType::EVENT;

	case(3) :
		return GroupVariationType::STATIC;

	case(4) :
		return GroupVariationType::EVENT;

	case(10) :
		return GroupVariationType::STATIC;

	case(11) :
		return GroupVariationType::EVENT;

	case(13) :
		return GroupVariationType::EVENT;

	case(20) :
		return GroupVariationType::STATIC;

	case(21) :
		return GroupVariationType::STATIC;

	case(22) :
		return GroupVariationType::EVENT;

	case(23) :
		return GroupVariationType::EVENT;

	case(30) :
		return GroupVariationType::STATIC;

	case(32) :
		return GroupVariationType::EVENT;

	case(40) :
		return GroupVariationType::STATIC;

	case(41) :
		return GroupVariationType::EVENT;

	case(42) :
		return GroupVariationType::EVENT;

	case(43) :
		return GroupVariationType::EVENT;

	case(50) :
		switch (variation)
		{
		case(4) :
			return GroupVariationType::STATIC;
		default:
			return GroupVariationType::OTHER;
		}

	case(60):
		switch (variation)
		{
		case(1) :
			return GroupVariationType::STATIC;
		default:
			return GroupVariationType::EVENT;
		}

	case(110):
		return GroupVariationType::STATIC;

	case(111):
		return GroupVariationType::EVENT;

	case(121) :
		return GroupVariationType::STATIC;
	case(122) :
		return GroupVariationType::EVENT;

	default:
		return GroupVariationType::OTHER;
	}
}

} //end ns
