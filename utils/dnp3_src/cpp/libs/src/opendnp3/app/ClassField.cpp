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
#include "opendnp3/app/ClassField.h"

namespace opendnp3
{

ClassField ClassField::None()
{
	return ClassField();
}

ClassField ClassField::AllClasses()
{
	return ClassField(ALL_CLASSES);
}

ClassField ClassField::AllEventClasses()
{
	return ClassField(EVENT_CLASSES);
}

ClassField::ClassField() : bitfield(0)
{}

ClassField::ClassField(PointClass pc) : bitfield(static_cast<uint8_t>(pc))
{}

ClassField::ClassField(bool class0, bool class1, bool class2, bool class3) : bitfield(0)
{
	bitfield = class0 ? ClassField::CLASS_0 : 0;
	bitfield |= class1 ? ClassField::CLASS_1 : 0;
	bitfield |= class2 ? ClassField::CLASS_2 : 0;
	bitfield |= class3 ? ClassField::CLASS_3 : 0;
}

ClassField::ClassField(uint8_t mask_) : bitfield(mask_ & ALL_CLASSES)
{}

bool ClassField::IsEmpty() const
{
	return (bitfield == 0);
}

bool ClassField::Intersects(const ClassField& other) const
{
	return (bitfield & other.bitfield) > 0;
}

ClassField ClassField::OnlyEventClasses() const
{
	return ClassField(bitfield & EVENT_CLASSES);
}

void ClassField::Set(PointClass pc)
{
	bitfield |= static_cast<uint8_t>(pc);
}

void ClassField::Clear(const ClassField& field)
{
	bitfield &= ~(field.bitfield);
}

void ClassField::Set(const ClassField& field)
{
	bitfield |= field.bitfield;
}

bool ClassField::HasEventType(EventClass ec) const
{
	switch (ec)
	{
	case(EventClass::EC1) :
		return HasClass1();
	case(EventClass::EC2) :
		return HasClass2();
	case(EventClass::EC3) :
		return HasClass3();
	default:
		return false;
	}
}

bool ClassField::HasClass0() const
{
	return (bitfield & CLASS_0) != 0;
}

bool ClassField::HasClass1() const
{
	return (bitfield & CLASS_1) != 0;
}

bool ClassField::HasClass2() const
{
	return (bitfield & CLASS_2) != 0;
}

bool ClassField::HasClass3() const
{
	return (bitfield & CLASS_3) != 0;
}

bool ClassField::HasEventClass() const
{
	return (bitfield & EVENT_CLASSES) != 0;
}

bool ClassField::HasAnyClass() const
{
	return bitfield != 0;
}

}



