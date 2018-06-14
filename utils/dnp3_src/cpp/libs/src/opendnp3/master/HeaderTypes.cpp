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
#include "opendnp3/master/HeaderTypes.h"

#include <opendnp3/app/HeaderWriter.h>


namespace opendnp3
{

Header Header::AllObjects(uint8_t group, uint8_t variation)
{
	return Header(group, variation);
}

Header Header::From(PointClass pc)
{
	switch (pc)
	{
	case(PointClass::Class0):
		return Header::AllObjects(60, 1);
	case(PointClass::Class1):
		return Header::AllObjects(60, 2);
	case(PointClass::Class2):
		return Header::AllObjects(60, 3);
	default:
		return Header::AllObjects(60, 4);
	}
}

Header Header::Range8(uint8_t group, uint8_t variation, uint8_t start, uint8_t stop)
{
	return Header(group, variation, start, stop);
}

Header Header::Range16(uint8_t group, uint8_t variation, uint16_t start, uint16_t stop)
{
	return Header(group, variation, start, stop);
}

Header Header::Count8(uint8_t group, uint8_t variation, uint8_t count)
{
	return Header(group, variation, count);
}

Header Header::Count16(uint8_t group, uint8_t variation, uint16_t count)
{
	return Header(group, variation, count);
}

Header::Header(uint8_t group, uint8_t var) : id(group, var), type(HeaderType::AllObjects) {}

Header::Header(uint8_t group, uint8_t var, uint8_t start, uint8_t stop) : id(group, var), type(HeaderType::Ranged8)
{
	value.range8 = { start, stop };
}

Header::Header(uint8_t group, uint8_t var, uint16_t start, uint16_t stop) : id(group, var), type(HeaderType::Ranged16)
{
	value.range16 = { start, stop };
}

Header::Header(uint8_t group, uint8_t var, uint8_t count) : id(group, var), type(HeaderType::LimitedCount8)
{
	value.count8.value = count;
}

Header::Header(uint8_t group, uint8_t var, uint16_t count) : id(group, var), type(HeaderType::LimitedCount16)
{
	value.count16.value = count;
}

bool Header::WriteTo(opendnp3::HeaderWriter& writer) const
{
	switch (type)
	{
	case(HeaderType::AllObjects) :
		return writer.WriteHeader(id, QualifierCode::ALL_OBJECTS);
	case(HeaderType::Ranged8) :
		return writer.WriteRangeHeader<openpal::UInt8>(QualifierCode::UINT8_START_STOP, id, value.range8.start, value.range8.stop);
	case(HeaderType::Ranged16) :
		return writer.WriteRangeHeader<openpal::UInt16>(QualifierCode::UINT16_START_STOP, id, value.range16.start, value.range16.stop);
	case(HeaderType::LimitedCount8) :
		return writer.WriteCountHeader<openpal::UInt8>(QualifierCode::UINT8_CNT, id, value.count8.value);
	case(HeaderType::LimitedCount16) :
		return writer.WriteCountHeader<openpal::UInt16>(QualifierCode::UINT16_CNT, id, value.count16.value);
	default:
		return false;
	}
}
}



