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
#include "EventWriter.h"

#include "opendnp3/objects/Group2.h"
#include "opendnp3/objects/Group4.h"
#include "opendnp3/objects/Group11.h"
#include "opendnp3/objects/Group22.h"
#include "opendnp3/objects/Group23.h"
#include "opendnp3/objects/Group32.h"
#include "opendnp3/objects/Group42.h"
#include "opendnp3/objects/Group51.h"
#include "opendnp3/objects/Group122.h"

using namespace openpal;

namespace opendnp3
{
bool EventWriter::Write(HeaderWriter& writer, IEventRecorder& recorder, openpal::LinkedListIterator<SOERecord> iterator)
{
	while (iterator.HasNext() && recorder.HasMoreUnwrittenEvents())
	{
		auto pCurrent = iterator.Next();

		if (IsWritable(pCurrent->value))
		{
			auto result = LoadHeader(writer, recorder, pCurrent);
			iterator = result.location;

			if (result.isFragmentFull)
			{
				return false;
			}
		}
	}

	return true;
}

EventWriter::Result EventWriter::LoadHeader(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation)
{
	switch (pLocation->value.type)
	{
	case(EventType::Binary) :
		return LoadHeaderBinary(writer, recorder, pLocation);
	case(EventType::DoubleBitBinary) :
		return LoadHeaderDoubleBinary(writer, recorder, pLocation);
	case(EventType::Counter):
		return LoadHeaderCounter(writer, recorder, pLocation);
	case(EventType::FrozenCounter):
		return LoadHeaderFrozenCounter(writer, recorder, pLocation);
	case(EventType::Analog):
		return LoadHeaderAnalog(writer, recorder, pLocation);
	case(EventType::BinaryOutputStatus):
		return LoadHeaderBinaryOutputStatus(writer, recorder, pLocation);
	case(EventType::AnalogOutputStatus) :
		return LoadHeaderAnalogOutputStatus(writer, recorder, pLocation);
	case(EventType::SecurityStat) :
		return LoadHeaderSecurityStat(writer, recorder, pLocation);
	default:
		return Result(false, LinkedListIterator<SOERecord>::Undefined());
	}
}

EventWriter::Result EventWriter::LoadHeaderBinary(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation)
{
	auto variation = pLocation->value.GetValue<BinarySpec>().selectedVariation;

	switch (variation)
	{
	case(EventBinaryVariation::Group2Var1):
		return WriteTypeWithSerializer<BinarySpec>(writer, recorder, pLocation, Group2Var1::Inst(), variation);
	case(EventBinaryVariation::Group2Var2):
		return WriteTypeWithSerializer<BinarySpec>(writer, recorder, pLocation, Group2Var2::Inst(), variation);
	case(EventBinaryVariation::Group2Var3) :
		return WriteCTOTypeWithSerializer<BinarySpec, Group51Var1>(writer, recorder, pLocation, Group2Var3::Inst(), variation);
	default:
		return WriteTypeWithSerializer<BinarySpec>(writer, recorder, pLocation, Group2Var1::Inst(), variation);
	}
}

EventWriter::Result EventWriter::LoadHeaderDoubleBinary(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation)
{
	auto variation = pLocation->value.GetValue<DoubleBitBinarySpec>().selectedVariation;

	switch (variation)
	{
	case(EventDoubleBinaryVariation::Group4Var1) :
		return WriteTypeWithSerializer<DoubleBitBinarySpec>(writer, recorder, pLocation, Group4Var1::Inst(), variation);
	case(EventDoubleBinaryVariation::Group4Var2) :
		return WriteTypeWithSerializer<DoubleBitBinarySpec>(writer, recorder, pLocation, Group4Var2::Inst(), variation);
	case(EventDoubleBinaryVariation::Group4Var3) :
		return WriteCTOTypeWithSerializer<DoubleBitBinarySpec, Group51Var1>(writer, recorder, pLocation, Group4Var3::Inst(), variation);
	default:
		return WriteTypeWithSerializer<DoubleBitBinarySpec>(writer, recorder, pLocation, Group4Var1::Inst(), variation);
	}
}

EventWriter::Result EventWriter::LoadHeaderCounter(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation)
{
	auto variation = pLocation->value.GetValue<CounterSpec>().selectedVariation;

	switch (variation)
	{
	case(EventCounterVariation::Group22Var1) :
		return WriteTypeWithSerializer<CounterSpec>(writer, recorder, pLocation, Group22Var1::Inst(), variation);
	case(EventCounterVariation::Group22Var2) :
		return WriteTypeWithSerializer<CounterSpec>(writer, recorder, pLocation, Group22Var2::Inst(), variation);
	case(EventCounterVariation::Group22Var5) :
		return WriteTypeWithSerializer<CounterSpec>(writer, recorder, pLocation, Group22Var5::Inst(), variation);
	case(EventCounterVariation::Group22Var6) :
		return WriteTypeWithSerializer<CounterSpec>(writer, recorder, pLocation, Group22Var6::Inst(), variation);
	default:
		return WriteTypeWithSerializer<CounterSpec>(writer, recorder, pLocation, Group22Var1::Inst(), variation);
	}
}

EventWriter::Result EventWriter::LoadHeaderFrozenCounter(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation)
{
	auto variation = pLocation->value.GetValue<FrozenCounterSpec>().selectedVariation;

	switch (variation)
	{
	case(EventFrozenCounterVariation::Group23Var1) :
		return WriteTypeWithSerializer<FrozenCounterSpec>(writer, recorder, pLocation, Group23Var1::Inst(), variation);
	case(EventFrozenCounterVariation::Group23Var2) :
		return WriteTypeWithSerializer<FrozenCounterSpec>(writer, recorder, pLocation, Group23Var2::Inst(), variation);
	case(EventFrozenCounterVariation::Group23Var5) :
		return WriteTypeWithSerializer<FrozenCounterSpec>(writer, recorder, pLocation, Group23Var5::Inst(), variation);
	case(EventFrozenCounterVariation::Group23Var6) :
		return WriteTypeWithSerializer<FrozenCounterSpec>(writer, recorder, pLocation, Group23Var6::Inst(), variation);
	default:
		return WriteTypeWithSerializer<FrozenCounterSpec>(writer, recorder, pLocation, Group23Var1::Inst(), variation);
	}
}

EventWriter::Result EventWriter::LoadHeaderAnalog(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation)
{
	auto variation = pLocation->value.GetValue<AnalogSpec>().selectedVariation;

	switch (variation)
	{
	case(EventAnalogVariation::Group32Var1) :
		return WriteTypeWithSerializer<AnalogSpec>(writer, recorder, pLocation, Group32Var1::Inst(), variation);
	case(EventAnalogVariation::Group32Var2) :
		return WriteTypeWithSerializer<AnalogSpec>(writer, recorder, pLocation, Group32Var2::Inst(), variation);
	case(EventAnalogVariation::Group32Var3) :
		return WriteTypeWithSerializer<AnalogSpec>(writer, recorder, pLocation, Group32Var3::Inst(), variation);
	case(EventAnalogVariation::Group32Var4) :
		return WriteTypeWithSerializer<AnalogSpec>(writer, recorder, pLocation, Group32Var4::Inst(), variation);
	case(EventAnalogVariation::Group32Var5) :
		return WriteTypeWithSerializer<AnalogSpec>(writer, recorder, pLocation, Group32Var5::Inst(), variation);
	case(EventAnalogVariation::Group32Var6) :
		return WriteTypeWithSerializer<AnalogSpec>(writer, recorder, pLocation, Group32Var6::Inst(), variation);
	case(EventAnalogVariation::Group32Var7) :
		return WriteTypeWithSerializer<AnalogSpec>(writer, recorder, pLocation, Group32Var7::Inst(), variation);
	case(EventAnalogVariation::Group32Var8) :
		return WriteTypeWithSerializer<AnalogSpec>(writer, recorder, pLocation, Group32Var8::Inst(), variation);
	default:
		return WriteTypeWithSerializer<AnalogSpec>(writer, recorder, pLocation, Group32Var1::Inst(), variation);
	}
}

EventWriter::Result EventWriter::LoadHeaderBinaryOutputStatus(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation)
{
	auto variation = pLocation->value.GetValue<BinaryOutputStatusSpec>().selectedVariation;

	switch (variation)
	{
	case(EventBinaryOutputStatusVariation::Group11Var1) :
		return WriteTypeWithSerializer<BinaryOutputStatusSpec>(writer, recorder, pLocation, Group11Var1::Inst(), variation);
	case(EventBinaryOutputStatusVariation::Group11Var2) :
		return WriteTypeWithSerializer<BinaryOutputStatusSpec>(writer, recorder, pLocation, Group11Var2::Inst(), variation);
	default:
		return WriteTypeWithSerializer<BinaryOutputStatusSpec>(writer, recorder, pLocation, Group11Var1::Inst(), variation);
	}
}

EventWriter::Result EventWriter::LoadHeaderAnalogOutputStatus(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation)
{
	auto variation = pLocation->value.GetValue<AnalogOutputStatusSpec>().selectedVariation;

	switch (variation)
	{
	case(EventAnalogOutputStatusVariation::Group42Var1) :
		return WriteTypeWithSerializer<AnalogOutputStatusSpec>(writer, recorder, pLocation, Group42Var1::Inst(), variation);
	case(EventAnalogOutputStatusVariation::Group42Var2) :
		return WriteTypeWithSerializer<AnalogOutputStatusSpec>(writer, recorder, pLocation, Group42Var2::Inst(), variation);
	case(EventAnalogOutputStatusVariation::Group42Var3) :
		return WriteTypeWithSerializer<AnalogOutputStatusSpec>(writer, recorder, pLocation, Group42Var3::Inst(), variation);
	case(EventAnalogOutputStatusVariation::Group42Var4) :
		return WriteTypeWithSerializer<AnalogOutputStatusSpec>(writer, recorder, pLocation, Group42Var4::Inst(), variation);
	case(EventAnalogOutputStatusVariation::Group42Var5) :
		return WriteTypeWithSerializer<AnalogOutputStatusSpec>(writer, recorder, pLocation, Group42Var5::Inst(), variation);
	case(EventAnalogOutputStatusVariation::Group42Var6) :
		return WriteTypeWithSerializer<AnalogOutputStatusSpec>(writer, recorder, pLocation, Group42Var6::Inst(), variation);
	case(EventAnalogOutputStatusVariation::Group42Var7) :
		return WriteTypeWithSerializer<AnalogOutputStatusSpec>(writer, recorder, pLocation, Group42Var7::Inst(), variation);
	case(EventAnalogOutputStatusVariation::Group42Var8) :
		return WriteTypeWithSerializer<AnalogOutputStatusSpec>(writer, recorder, pLocation, Group42Var8::Inst(), variation);
	default:
		return WriteTypeWithSerializer<AnalogOutputStatusSpec>(writer, recorder, pLocation, Group42Var1::Inst(), variation);
	}
}

EventWriter::Result EventWriter::LoadHeaderSecurityStat(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation)
{
	auto variation = pLocation->value.GetValue<SecurityStatSpec>().selectedVariation;

	switch (variation)
	{
	case(EventSecurityStatVariation::Group122Var1) :
		return WriteTypeWithSerializer<SecurityStatSpec>(writer, recorder, pLocation, Group122Var1::Inst(), variation);
	case(EventSecurityStatVariation::Group122Var2) :
		return WriteTypeWithSerializer<SecurityStatSpec>(writer, recorder, pLocation, Group122Var2::Inst(), variation);
	default:
		return WriteTypeWithSerializer<SecurityStatSpec>(writer, recorder, pLocation, Group122Var1::Inst(), variation);
	}
}

}
