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
#include "StaticBuffers.h"

namespace opendnp3
{

StaticBuffers::StaticBuffers(const DatabaseSizes& dbSizes) :
	binaries(dbSizes.numBinary),
	doubleBinaries(dbSizes.numDoubleBinary),
	analogs(dbSizes.numAnalog),
	counters(dbSizes.numCounter),
	frozenCounters(dbSizes.numFrozenCounter),
	binaryOutputStatii(dbSizes.numBinaryOutputStatus),
	analogOutputStatii(dbSizes.numAnalogOutputStatus),
	timeAndIntervals(dbSizes.numTimeAndInterval)
{
	this->SetDefaultIndices<BinarySpec>();
	this->SetDefaultIndices<DoubleBitBinarySpec>();
	this->SetDefaultIndices<CounterSpec>();
	this->SetDefaultIndices<FrozenCounterSpec>();
	this->SetDefaultIndices<AnalogSpec>();
	this->SetDefaultIndices<BinaryOutputStatusSpec>();
	this->SetDefaultIndices<AnalogOutputStatusSpec>();
	this->SetDefaultIndices<TimeAndIntervalSpec>();
}

DatabaseConfigView StaticBuffers::GetView() const
{
	return DatabaseConfigView(
	           binaries.ToView(),
	           doubleBinaries.ToView(),
	           analogs.ToView(),
	           counters.ToView(),
	           frozenCounters.ToView(),
	           binaryOutputStatii.ToView(),
	           analogOutputStatii.ToView(),
	           timeAndIntervals.ToView()
	       );
}

template <>
openpal::ArrayView<Cell<BinarySpec>, uint16_t> StaticBuffers::GetArrayView()
{
	return binaries.ToView();
}

template <>
openpal::ArrayView<Cell<DoubleBitBinarySpec>, uint16_t> StaticBuffers::GetArrayView()
{
	return doubleBinaries.ToView();
}

template <>
openpal::ArrayView<Cell<CounterSpec>, uint16_t> StaticBuffers::GetArrayView()
{
	return counters.ToView();
}

template <>
openpal::ArrayView<Cell<FrozenCounterSpec>, uint16_t> StaticBuffers::GetArrayView()
{
	return frozenCounters.ToView();
}

template <>
openpal::ArrayView<Cell<AnalogSpec>, uint16_t> StaticBuffers::GetArrayView()
{
	return analogs.ToView();
}

template <>
openpal::ArrayView<Cell<BinaryOutputStatusSpec>, uint16_t> StaticBuffers::GetArrayView()
{
	return binaryOutputStatii.ToView();
}

template <>
openpal::ArrayView<Cell<AnalogOutputStatusSpec>, uint16_t> StaticBuffers::GetArrayView()
{
	return analogOutputStatii.ToView();
}

template <>
openpal::ArrayView<Cell<TimeAndIntervalSpec>, uint16_t> StaticBuffers::GetArrayView()
{
	return timeAndIntervals.ToView();
}

}


