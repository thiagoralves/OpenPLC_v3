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
#ifndef OPENDNP3_CLASSFIELD_H
#define OPENDNP3_CLASSFIELD_H

#include <cstdint>

#include "opendnp3/app/EventType.h"
#include "opendnp3/gen/PointClass.h"

namespace opendnp3
{

/**
* Specifies a set of event classes e.g. some subset of {0, 1, 2, 3}
*/
class ClassField
{
public:

	ClassField();

	ClassField(PointClass pc);

	ClassField(uint8_t mask_);

	ClassField(bool class0, bool class1, bool class2, bool class3);

	bool IsEmpty() const;

	bool Intersects(const ClassField& other) const;

	uint8_t GetBitfield() const
	{
		return bitfield;
	};

	ClassField OnlyEventClasses() const;

	void Clear(const ClassField& field);

	void Set(const ClassField& field);

	void Set(PointClass pc);

	static const uint8_t CLASS_0 = static_cast<uint8_t>(PointClass::Class0);
	static const uint8_t CLASS_1 = static_cast<uint8_t>(PointClass::Class1);
	static const uint8_t CLASS_2 = static_cast<uint8_t>(PointClass::Class2);
	static const uint8_t CLASS_3 = static_cast<uint8_t>(PointClass::Class3);
	static const uint8_t EVENT_CLASSES = CLASS_1 | CLASS_2 | CLASS_3;
	static const uint8_t ALL_CLASSES = EVENT_CLASSES | CLASS_0;

	bool HasEventType(EventClass ec) const;

	bool HasClass0() const;
	bool HasClass1() const;
	bool HasClass2() const;
	bool HasClass3() const;

	bool HasEventClass() const;
	bool HasAnyClass() const;

	static ClassField None();
	static ClassField AllClasses();
	static ClassField AllEventClasses();

private:
	uint8_t bitfield;
};


}

#endif

