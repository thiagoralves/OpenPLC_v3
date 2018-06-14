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
#include "StaticLoadFunctions.h"

#include "opendnp3/objects/Group1.h"
#include "opendnp3/objects/Group3.h"
#include "opendnp3/objects/Group10.h"
#include "opendnp3/objects/Group20.h"
#include "opendnp3/objects/Group21.h"
#include "opendnp3/objects/Group30.h"
#include "opendnp3/objects/Group40.h"
#include "opendnp3/objects/Group50.h"
#include "opendnp3/objects/Group121.h"

using namespace openpal;

namespace opendnp3
{


StaticWriter<BinarySpec>::Function GetStaticWriter(StaticBinaryVariation variation)
{
	switch (variation)
	{
	case(StaticBinaryVariation::Group1Var1) :
		return &WriteSingleBitfield<BinarySpec, Group1Var1>;
	default:
		return &WriteWithSerializer<BinarySpec, Group1Var2>;
	}
}

StaticWriter<DoubleBitBinarySpec>::Function GetStaticWriter(StaticDoubleBinaryVariation variation)
{
	switch (variation)
	{
	case(StaticDoubleBinaryVariation::Group3Var2) :
		return &WriteWithSerializer<DoubleBitBinarySpec, Group3Var2>;
	default:
		return &WriteWithSerializer<DoubleBitBinarySpec, Group3Var2>;
	}
}

StaticWriter<AnalogSpec>::Function GetStaticWriter(StaticAnalogVariation variation)
{
	switch (variation)
	{
	case(StaticAnalogVariation::Group30Var1): return &WriteWithSerializer<AnalogSpec, Group30Var1>;
	case(StaticAnalogVariation::Group30Var2): return &WriteWithSerializer<AnalogSpec, Group30Var2>;
	case(StaticAnalogVariation::Group30Var3): return &WriteWithSerializer<AnalogSpec, Group30Var3>;
	case(StaticAnalogVariation::Group30Var4): return &WriteWithSerializer<AnalogSpec, Group30Var4>;
	case(StaticAnalogVariation::Group30Var5): return &WriteWithSerializer<AnalogSpec, Group30Var5>;
	case(StaticAnalogVariation::Group30Var6): return &WriteWithSerializer<AnalogSpec, Group30Var6>;
	default:
		return &WriteWithSerializer<AnalogSpec, Group30Var1>;
	}
}

StaticWriter<CounterSpec>::Function GetStaticWriter(StaticCounterVariation variation)
{
	switch (variation)
	{
	case(StaticCounterVariation::Group20Var1): return &WriteWithSerializer<CounterSpec, Group20Var1>;
	case(StaticCounterVariation::Group20Var2): return &WriteWithSerializer<CounterSpec, Group20Var2>;
	case(StaticCounterVariation::Group20Var5): return &WriteWithSerializer<CounterSpec, Group20Var5>;
	case(StaticCounterVariation::Group20Var6): return &WriteWithSerializer<CounterSpec, Group20Var6>;
	default:
		return &WriteWithSerializer<CounterSpec, Group20Var1>;
	}
}

StaticWriter<FrozenCounterSpec>::Function GetStaticWriter(StaticFrozenCounterVariation variation)
{
	switch (variation)
	{
	case(StaticFrozenCounterVariation::Group21Var1): return &WriteWithSerializer<FrozenCounterSpec, Group21Var1>;
	default:
		return &WriteWithSerializer<FrozenCounterSpec, Group21Var1>;
	}
}

StaticWriter<BinaryOutputStatusSpec>::Function GetStaticWriter(StaticBinaryOutputStatusVariation variation)
{
	switch (variation)
	{
	case(StaticBinaryOutputStatusVariation::Group10Var2): return &WriteWithSerializer<BinaryOutputStatusSpec, Group10Var2>;
	default:
		return &WriteWithSerializer<BinaryOutputStatusSpec, Group10Var2>;
	}
}

StaticWriter<AnalogOutputStatusSpec>::Function GetStaticWriter(StaticAnalogOutputStatusVariation variation)
{
	switch (variation)
	{
	case(StaticAnalogOutputStatusVariation::Group40Var1): return &WriteWithSerializer<AnalogOutputStatusSpec, Group40Var1>;
	case(StaticAnalogOutputStatusVariation::Group40Var2): return &WriteWithSerializer<AnalogOutputStatusSpec, Group40Var2>;
	case(StaticAnalogOutputStatusVariation::Group40Var3): return &WriteWithSerializer<AnalogOutputStatusSpec, Group40Var3>;
	case(StaticAnalogOutputStatusVariation::Group40Var4): return &WriteWithSerializer<AnalogOutputStatusSpec, Group40Var4>;
	default:
		return &WriteWithSerializer<AnalogOutputStatusSpec, Group40Var1>;
	}
}

StaticWriter<TimeAndIntervalSpec>::Function GetStaticWriter(StaticTimeAndIntervalVariation variation)
{
	return &WriteWithSerializer<TimeAndIntervalSpec, Group50Var4>;
}

StaticWriter<SecurityStatSpec>::Function GetStaticWriter(StaticSecurityStatVariation variation)
{
	return &WriteWithSerializer < SecurityStatSpec, Group121Var1 > ;
}

}

