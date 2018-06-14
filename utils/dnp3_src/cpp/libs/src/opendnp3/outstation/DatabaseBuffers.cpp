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
#include "DatabaseBuffers.h"

#include "openpal/logging/LogMacros.h"

#include <assert.h>

using namespace openpal;

namespace opendnp3
{

DatabaseBuffers::DatabaseBuffers(const DatabaseSizes& dbSizes, StaticTypeBitField allowedClass0Types, IndexMode indexMode) :
	buffers(dbSizes),
	class0(allowedClass0Types),
	indexMode(indexMode)
{

}

void DatabaseBuffers::Unselect()
{
	this->Deselect<BinarySpec>();
	this->Deselect<DoubleBitBinarySpec>();
	this->Deselect<CounterSpec>();
	this->Deselect<FrozenCounterSpec>();
	this->Deselect<AnalogSpec>();
	this->Deselect<BinaryOutputStatusSpec>();
	this->Deselect<AnalogOutputStatusSpec>();
	this->Deselect<TimeAndIntervalSpec>();
}

IINField DatabaseBuffers::SelectAll(GroupVariation gv)
{
	if (gv == GroupVariation::Group60Var1)
	{
		this->SelectAllClass0<BinarySpec>();
		this->SelectAllClass0<DoubleBitBinarySpec>();
		this->SelectAllClass0<CounterSpec>();
		this->SelectAllClass0<FrozenCounterSpec>();
		this->SelectAllClass0<AnalogSpec>();
		this->SelectAllClass0<BinaryOutputStatusSpec>();
		this->SelectAllClass0<AnalogOutputStatusSpec>();
		this->SelectAllClass0<TimeAndIntervalSpec>();

		return IINField::Empty();
	}
	else
	{
		switch (gv)
		{
		case(GroupVariation::Group1Var0):
			return this->SelectAll<BinarySpec>();
		case(GroupVariation::Group1Var1) :
			return this->SelectAllUsing<BinarySpec>(StaticBinaryVariation::Group1Var1);
		case(GroupVariation::Group1Var2) :
			return this->SelectAllUsing<BinarySpec>(StaticBinaryVariation::Group1Var2);

		case(GroupVariation::Group3Var0) :
			return this->SelectAll<DoubleBitBinarySpec>();
		case(GroupVariation::Group3Var2) :
			return this->SelectAllUsing<DoubleBitBinarySpec>(StaticDoubleBinaryVariation::Group3Var2);

		case(GroupVariation::Group10Var0) :
			return this->SelectAll<BinaryOutputStatusSpec>();
		case(GroupVariation::Group10Var2) :
			return this->SelectAllUsing<BinaryOutputStatusSpec>(StaticBinaryOutputStatusVariation::Group10Var2);

		case(GroupVariation::Group20Var0):
			return this->SelectAll<CounterSpec>();
		case(GroupVariation::Group20Var1):
			return this->SelectAllUsing<CounterSpec>(StaticCounterVariation::Group20Var1);
		case(GroupVariation::Group20Var2) :
			return this->SelectAllUsing<CounterSpec>(StaticCounterVariation::Group20Var2);
		case(GroupVariation::Group20Var5) :
			return this->SelectAllUsing<CounterSpec>(StaticCounterVariation::Group20Var5);
		case(GroupVariation::Group20Var6) :
			return this->SelectAllUsing<CounterSpec>(StaticCounterVariation::Group20Var6);

		case(GroupVariation::Group21Var0) :
			return this->SelectAll<FrozenCounterSpec>();
		case(GroupVariation::Group21Var1) :
			return this->SelectAllUsing<FrozenCounterSpec>(StaticFrozenCounterVariation::Group21Var1);
		case(GroupVariation::Group21Var2) :
			return this->SelectAllUsing<FrozenCounterSpec>(StaticFrozenCounterVariation::Group21Var2);
		case(GroupVariation::Group21Var5) :
			return this->SelectAllUsing<FrozenCounterSpec>(StaticFrozenCounterVariation::Group21Var5);
		case(GroupVariation::Group21Var6) :
			return this->SelectAllUsing<FrozenCounterSpec>(StaticFrozenCounterVariation::Group21Var6);
		case(GroupVariation::Group21Var9) :
			return this->SelectAllUsing<FrozenCounterSpec>(StaticFrozenCounterVariation::Group21Var9);
		case(GroupVariation::Group21Var10) :
			return this->SelectAllUsing<FrozenCounterSpec>(StaticFrozenCounterVariation::Group21Var10);

		case(GroupVariation::Group30Var0) :
			return this->SelectAll<AnalogSpec>();
		case(GroupVariation::Group30Var1) :
			return this->SelectAllUsing<AnalogSpec>(StaticAnalogVariation::Group30Var1);
		case(GroupVariation::Group30Var2) :
			return this->SelectAllUsing<AnalogSpec>(StaticAnalogVariation::Group30Var2);
		case(GroupVariation::Group30Var3) :
			return this->SelectAllUsing<AnalogSpec>(StaticAnalogVariation::Group30Var3);
		case(GroupVariation::Group30Var4) :
			return this->SelectAllUsing<AnalogSpec>(StaticAnalogVariation::Group30Var4);
		case(GroupVariation::Group30Var5) :
			return this->SelectAllUsing<AnalogSpec>(StaticAnalogVariation::Group30Var5);
		case(GroupVariation::Group30Var6) :
			return this->SelectAllUsing<AnalogSpec>(StaticAnalogVariation::Group30Var6);

		case(GroupVariation::Group40Var0) :
			return this->SelectAll<AnalogOutputStatusSpec>();
		case(GroupVariation::Group40Var1) :
			return this->SelectAllUsing<AnalogOutputStatusSpec>(StaticAnalogOutputStatusVariation::Group40Var1);
		case(GroupVariation::Group40Var2) :
			return this->SelectAllUsing<AnalogOutputStatusSpec>(StaticAnalogOutputStatusVariation::Group40Var2);
		case(GroupVariation::Group40Var3) :
			return this->SelectAllUsing<AnalogOutputStatusSpec>(StaticAnalogOutputStatusVariation::Group40Var3);
		case(GroupVariation::Group40Var4) :
			return this->SelectAllUsing<AnalogOutputStatusSpec>(StaticAnalogOutputStatusVariation::Group40Var4);

		case(GroupVariation::Group50Var4) :
			return this->SelectAllUsing<TimeAndIntervalSpec>(StaticTimeAndIntervalVariation::Group50Var4);

		default:
			return IINField(IINBit::FUNC_NOT_SUPPORTED);
		}
	}
}

IINField DatabaseBuffers::SelectRange(GroupVariation gv, const Range& range)
{
	switch (gv)
	{
	case(GroupVariation::Group1Var0) :
		return this->SelectRange<BinarySpec>(range);
	case(GroupVariation::Group1Var1) :
		return this->SelectRangeUsing<BinarySpec>(range, StaticBinaryVariation::Group1Var1);
	case(GroupVariation::Group1Var2) :
		return this->SelectRangeUsing<BinarySpec>(range, StaticBinaryVariation::Group1Var2);

	case(GroupVariation::Group3Var0) :
		return this->SelectRange<DoubleBitBinarySpec>(range);
	case(GroupVariation::Group3Var2) :
		return this->SelectRangeUsing<DoubleBitBinarySpec>(range, StaticDoubleBinaryVariation::Group3Var2);

	case(GroupVariation::Group10Var0) :
		return this->SelectRange<BinaryOutputStatusSpec>(range);
	case(GroupVariation::Group10Var2) :
		return this->SelectRangeUsing<BinaryOutputStatusSpec>(range, StaticBinaryOutputStatusVariation::Group10Var2);

	case(GroupVariation::Group20Var0) :
		return this->SelectRange<CounterSpec>(range);
	case(GroupVariation::Group20Var1) :
		return this->SelectRangeUsing<CounterSpec>(range, StaticCounterVariation::Group20Var1);
	case(GroupVariation::Group20Var2) :
		return this->SelectRangeUsing<CounterSpec>(range, StaticCounterVariation::Group20Var2);
	case(GroupVariation::Group20Var5) :
		return this->SelectRangeUsing<CounterSpec>(range, StaticCounterVariation::Group20Var5);
	case(GroupVariation::Group20Var6) :
		return this->SelectRangeUsing<CounterSpec>(range, StaticCounterVariation::Group20Var6);

	case(GroupVariation::Group21Var0) :
		return this->SelectRange<FrozenCounterSpec>(range);
	case(GroupVariation::Group21Var1) :
		return this->SelectRangeUsing<FrozenCounterSpec>(range, StaticFrozenCounterVariation::Group21Var1);
	case(GroupVariation::Group21Var2) :
		return this->SelectRangeUsing<FrozenCounterSpec>(range, StaticFrozenCounterVariation::Group21Var2);
	case(GroupVariation::Group21Var5) :
		return this->SelectRangeUsing<FrozenCounterSpec>(range, StaticFrozenCounterVariation::Group21Var5);
	case(GroupVariation::Group21Var6) :
		return this->SelectRangeUsing<FrozenCounterSpec>(range, StaticFrozenCounterVariation::Group21Var6);
	case(GroupVariation::Group21Var9) :
		return this->SelectRangeUsing<FrozenCounterSpec>(range, StaticFrozenCounterVariation::Group21Var9);
	case(GroupVariation::Group21Var10) :
		return this->SelectRangeUsing<FrozenCounterSpec>(range, StaticFrozenCounterVariation::Group21Var10);

	case(GroupVariation::Group30Var0) :
		return this->SelectRange<AnalogSpec>(range);
	case(GroupVariation::Group30Var1) :
		return this->SelectRangeUsing<AnalogSpec>(range, StaticAnalogVariation::Group30Var1);
	case(GroupVariation::Group30Var2) :
		return this->SelectRangeUsing<AnalogSpec>(range, StaticAnalogVariation::Group30Var2);
	case(GroupVariation::Group30Var3) :
		return this->SelectRangeUsing<AnalogSpec>(range, StaticAnalogVariation::Group30Var3);
	case(GroupVariation::Group30Var4) :
		return this->SelectRangeUsing<AnalogSpec>(range, StaticAnalogVariation::Group30Var4);
	case(GroupVariation::Group30Var5) :
		return this->SelectRangeUsing<AnalogSpec>(range, StaticAnalogVariation::Group30Var5);
	case(GroupVariation::Group30Var6) :
		return this->SelectRangeUsing<AnalogSpec>(range, StaticAnalogVariation::Group30Var6);

	case(GroupVariation::Group40Var0) :
		return this->SelectRange<AnalogOutputStatusSpec>(range);
	case(GroupVariation::Group40Var1) :
		return this->SelectRangeUsing<AnalogOutputStatusSpec>(range, StaticAnalogOutputStatusVariation::Group40Var1);
	case(GroupVariation::Group40Var2) :
		return this->SelectRangeUsing<AnalogOutputStatusSpec>(range, StaticAnalogOutputStatusVariation::Group40Var2);
	case(GroupVariation::Group40Var3) :
		return this->SelectRangeUsing<AnalogOutputStatusSpec>(range, StaticAnalogOutputStatusVariation::Group40Var3);
	case(GroupVariation::Group40Var4) :
		return this->SelectRangeUsing<AnalogOutputStatusSpec>(range, StaticAnalogOutputStatusVariation::Group40Var4);

	case(GroupVariation::Group50Var4) :
		return this->SelectRangeUsing<TimeAndIntervalSpec>(range, StaticTimeAndIntervalVariation::Group50Var4);

	default:
		return IINField(IINBit::FUNC_NOT_SUPPORTED);
	}
}

bool DatabaseBuffers::Load(HeaderWriter& writer)
{
	typedef bool (DatabaseBuffers::*LoadFun)(HeaderWriter & writer);

	const int NUM_TYPE = 8;

	LoadFun functions[NUM_TYPE] =
	{
		&DatabaseBuffers::LoadType<BinarySpec>,
		&DatabaseBuffers::LoadType<DoubleBitBinarySpec>,
		&DatabaseBuffers::LoadType<CounterSpec>,
		&DatabaseBuffers::LoadType<FrozenCounterSpec>,
		&DatabaseBuffers::LoadType<AnalogSpec>,
		&DatabaseBuffers::LoadType<BinaryOutputStatusSpec>,
		&DatabaseBuffers::LoadType<AnalogOutputStatusSpec>,
		&DatabaseBuffers::LoadType<TimeAndIntervalSpec>
	};

	for (int i = 0; i < NUM_TYPE; ++i)
	{
		if (!(this->*functions[i])(writer))
		{
			// return early because the APDU is full
			return false;
		}
	}

	return true;
}

Range DatabaseBuffers::AssignClassToAll(AssignClassType type, PointClass clazz)
{
	switch (type)
	{
	case(AssignClassType::BinaryInput) :
		return AssignClassToRange(type, clazz, RangeOf(buffers.GetArrayView<BinarySpec>().Size()));
	case(AssignClassType::DoubleBinaryInput) :
		return AssignClassToRange(type, clazz, RangeOf(buffers.GetArrayView<DoubleBitBinarySpec>().Size()));
	case(AssignClassType::Counter) :
		return AssignClassToRange(type, clazz, RangeOf(buffers.GetArrayView<CounterSpec>().Size()));
	case(AssignClassType::FrozenCounter) :
		return AssignClassToRange(type, clazz, RangeOf(buffers.GetArrayView<FrozenCounterSpec>().Size()));
	case(AssignClassType::AnalogInput) :
		return AssignClassToRange(type, clazz, RangeOf(buffers.GetArrayView<AnalogSpec>().Size()));
	case(AssignClassType::BinaryOutputStatus) :
		return AssignClassToRange(type, clazz, RangeOf(buffers.GetArrayView<BinaryOutputStatusSpec>().Size()));
	case(AssignClassType::AnalogOutputStatus) :
		return AssignClassToRange(type, clazz, RangeOf(buffers.GetArrayView<AnalogOutputStatusSpec>().Size()));
	default:
		return Range::Invalid();
	}
}

Range DatabaseBuffers::AssignClassToRange(AssignClassType type, PointClass clazz, const Range& range)
{
	switch (type)
	{
	case(AssignClassType::BinaryInput) :
		return AssignClassTo<BinarySpec>(clazz, range);
	case(AssignClassType::DoubleBinaryInput) :
		return AssignClassTo<DoubleBitBinarySpec>(clazz, range);
	case(AssignClassType::Counter) :
		return AssignClassTo<CounterSpec>(clazz, range);
	case(AssignClassType::FrozenCounter) :
		return AssignClassTo<FrozenCounterSpec>(clazz, range);
	case(AssignClassType::AnalogInput) :
		return AssignClassTo<AnalogSpec>(clazz, range);
	case(AssignClassType::BinaryOutputStatus) :
		return AssignClassTo<BinaryOutputStatusSpec>(clazz, range);
	case(AssignClassType::AnalogOutputStatus) :
		return AssignClassTo<AnalogOutputStatusSpec>(clazz, range);
	default:
		return Range::Invalid();
	}
}

template <>
StaticBinaryVariation DatabaseBuffers::CheckForPromotion<BinarySpec>(const Binary& value, StaticBinaryVariation variation)
{
	if (variation == StaticBinaryVariation::Group1Var1)
	{
		return BinarySpec::IsQualityOnlineOnly(value) ? variation : StaticBinaryVariation::Group1Var2;
	}
	else
	{
		return variation;
	}
}

Range DatabaseBuffers::RangeOf(uint16_t size)
{
	return size > 0 ? Range::From(0, size - 1) : Range::Invalid();
}


}

