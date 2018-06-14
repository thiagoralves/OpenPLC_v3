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
#ifndef OPENDNP3_STATICLOADFUNCTIONS_H
#define OPENDNP3_STATICLOADFUNCTIONS_H

#include "opendnp3/app/Range.h"
#include "opendnp3/app/HeaderWriter.h"
#include "opendnp3/app/MeasurementTypeSpecs.h"
#include "opendnp3/app/SecurityStat.h"
#include "opendnp3/outstation/Cell.h"

#include "opendnp3/gen/StaticBinaryVariation.h"
#include "opendnp3/gen/StaticDoubleBinaryVariation.h"
#include "opendnp3/gen/StaticCounterVariation.h"
#include "opendnp3/gen/StaticFrozenCounterVariation.h"
#include "opendnp3/gen/StaticAnalogVariation.h"
#include "opendnp3/gen/StaticAnalogOutputStatusVariation.h"
#include "opendnp3/gen/StaticBinaryOutputStatusVariation.h"

#include <openpal/container/ArrayView.h>

namespace opendnp3
{

template <class Spec>
struct StaticWriter
{
	typedef bool (*Function)(openpal::ArrayView<Cell<Spec>, uint16_t>& view, HeaderWriter& writer, Range& range);
};

StaticWriter<BinarySpec>::Function GetStaticWriter(StaticBinaryVariation variation);

StaticWriter<DoubleBitBinarySpec>::Function GetStaticWriter(StaticDoubleBinaryVariation variation);

StaticWriter<CounterSpec>::Function GetStaticWriter(StaticCounterVariation variation);

StaticWriter<FrozenCounterSpec>::Function GetStaticWriter(StaticFrozenCounterVariation variation);

StaticWriter<AnalogSpec>::Function GetStaticWriter(StaticAnalogVariation variation);

StaticWriter<AnalogOutputStatusSpec>::Function GetStaticWriter(StaticAnalogOutputStatusVariation variation);

StaticWriter<BinaryOutputStatusSpec>::Function GetStaticWriter(StaticBinaryOutputStatusVariation variation);

StaticWriter<TimeAndIntervalSpec>::Function GetStaticWriter(StaticTimeAndIntervalVariation variation);

StaticWriter<SecurityStatSpec>::Function GetStaticWriter(StaticSecurityStatVariation variation);

template <class Spec, class IndexType >
bool LoadWithRangeIterator(openpal::ArrayView<Cell<Spec>, uint16_t>& view, RangeWriteIterator<IndexType, typename Spec::meas_t>& iterator, Range& range)
{
	const Cell<Spec>& start = view[range.start];
	uint16_t nextIndex = start.config.vIndex;

	while (
	    range.IsValid() &&
	    view[range.start].selection.selected &&
	    (view[range.start].selection.variation == start.selection.variation) &&
	    (view[range.start].config.vIndex == nextIndex)
	)
	{
		if (iterator.Write(view[range.start].selection.value))
		{
			// deselect the value and advance the range
			view[range.start].selection.selected = false;
			range.Advance();
			++nextIndex;
		}
		else
		{
			return false;
		}
	}

	return true;
}

template <class Spec, class IndexType>
bool LoadWithBitfieldIterator(openpal::ArrayView<Cell<Spec>, uint16_t>& view, BitfieldRangeWriteIterator<IndexType>& iterator, Range& range)
{
	const Cell<Spec>& start = view[range.start];

	uint16_t nextIndex = start.config.vIndex;

	while (
	    range.IsValid() &&
	    view[range.start].selection.selected &&
	    (view[range.start].selection.variation == start.selection.variation) &&
	    (view[range.start].config.vIndex == nextIndex)
	)
	{
		if (iterator.Write(view[range.start].selection.value.value))
		{
			// deselect the value and advance the range
			view[range.start].selection.selected = false;
			range.Advance();
			++nextIndex;
		}
		else
		{
			return false;
		}
	}

	return true;
}

template <class Spec, class GV>
bool WriteSingleBitfield(openpal::ArrayView<Cell<Spec>, uint16_t>& view, HeaderWriter& writer, Range& range)
{
	auto start = view[range.start].config.vIndex;
	auto stop = view[range.stop].config.vIndex;
	auto mapped = Range::From(start, stop);

	if (mapped.IsOneByte())
	{
		auto iter = writer.IterateOverSingleBitfield<openpal::UInt8>(GV::ID(), QualifierCode::UINT8_START_STOP, static_cast<uint8_t>(mapped.start));
		return LoadWithBitfieldIterator<Spec, openpal::UInt8>(view, iter, range);
	}
	else
	{
		auto iter = writer.IterateOverSingleBitfield<openpal::UInt16>(GV::ID(), QualifierCode::UINT16_START_STOP, mapped.start);
		return LoadWithBitfieldIterator<Spec, openpal::UInt16>(view, iter, range);
	}
}


template <class Spec, class Serializer>
bool WriteWithSerializer(openpal::ArrayView<Cell<Spec>, uint16_t>& view, HeaderWriter& writer, Range& range)
{
	auto start = view[range.start].config.vIndex;
	auto stop = view[range.stop].config.vIndex;
	auto mapped = Range::From(start, stop);

	if (mapped.IsOneByte())
	{
		auto iter = writer.IterateOverRange<openpal::UInt8, typename Serializer::Target>(QualifierCode::UINT8_START_STOP, Serializer::Inst(), static_cast<uint8_t>(mapped.start));
		return LoadWithRangeIterator<Spec, openpal::UInt8>(view, iter, range);
	}
	else
	{
		auto iter = writer.IterateOverRange<openpal::UInt16, typename Serializer::Target>(QualifierCode::UINT16_START_STOP, Serializer::Inst(), mapped.start);
		return LoadWithRangeIterator<Spec, openpal::UInt16>(view, iter, range);
	}
}



}

#endif

