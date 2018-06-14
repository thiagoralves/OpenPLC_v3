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
#ifndef OPENDNP3_HEADERTYPES_H
#define OPENDNP3_HEADERTYPES_H

#include <cstdint>

#include "opendnp3/gen/QualifierCode.h"
#include "opendnp3/app/GroupVariationID.h"
#include "opendnp3/gen/PointClass.h"

namespace opendnp3
{

class HeaderWriter;

/**
* An enumeration that defines the type of object header
*/
enum class HeaderType : uint8_t
{
	AllObjects,
	Ranged8,
	Ranged16,
	LimitedCount8,
	LimitedCount16
};

/**
* A template for a integer range
*/
template <class T>
struct StartStopRange
{
	T start;
	T stop;
};

/**
* A template for an integer count
*/
template <class T>
struct Count
{
	T value;
};

/**
* Union type that holds information for a single header type
*/
union HeaderUnion
{
	StartStopRange<uint8_t> range8;
	StartStopRange<uint16_t> range16;
	Count<uint8_t> count8;
	Count<uint16_t> count16;
};

/**
* Class used to specify a header type
*/
class Header
{
public:

	bool WriteTo(opendnp3::HeaderWriter& writer) const;

	/**
	* Create an all objects (0x06) header
	*/
	static Header AllObjects(uint8_t group, uint8_t variation);

	/**
	* Create an all objects (0x06) header
	*/
	static Header From(PointClass pc);

	/**
	* Create a 8-bit start stop header (0x00)
	*/
	static Header Range8(uint8_t group, uint8_t variation, uint8_t start, uint8_t stop);

	/**
	* Create a 16-bit start stop header (0x01)
	*/
	static Header Range16(uint8_t group, uint8_t variation, uint16_t start, uint16_t stop);

	/**
	* Create a 8-bit count header (0x07)
	*/
	static Header Count8(uint8_t group, uint8_t variation, uint8_t count);

	/**
	* Create a 16-bit count header (0x08)
	*/
	static Header Count16(uint8_t group, uint8_t variation, uint16_t count);

	Header() {}

private:

	opendnp3::GroupVariationID id;
	HeaderType type = HeaderType::AllObjects;
	HeaderUnion value;

	Header(uint8_t group, uint8_t var);

	Header(uint8_t group, uint8_t var, uint8_t start, uint8_t stop);

	Header(uint8_t group, uint8_t var, uint16_t start, uint16_t stop);

	Header(uint8_t group, uint8_t var, uint8_t count);

	Header(uint8_t group, uint8_t var, uint16_t count);
};


}

#endif

