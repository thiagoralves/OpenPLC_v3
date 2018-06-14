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
#ifndef OPENDNP3_IAPDUHANDLER_H
#define OPENDNP3_IAPDUHANDLER_H

#include "opendnp3/app/parsing/IWhiteList.h"

#include "opendnp3/app/GroupVariationRecord.h"
#include "opendnp3/app/parsing/ICollection.h"

#include "opendnp3/app/MeasurementTypes.h"
#include "opendnp3/app/IINValue.h"
#include "opendnp3/app/ControlRelayOutputBlock.h"
#include "opendnp3/app/AnalogOutput.h"
#include "opendnp3/app/BinaryCommandEvent.h"
#include "opendnp3/app/AnalogCommandEvent.h"
#include "opendnp3/app/Indexed.h"
#include "opendnp3/app/OctetString.h"
#include "opendnp3/app/Range.h"

#include "opendnp3/gen/QualifierCode.h"

#include "opendnp3/objects/Group50.h"
#include "opendnp3/objects/Group51.h"
#include "opendnp3/objects/Group52.h"

#include "opendnp3/objects/Group120.h"
#include "opendnp3/objects/Group121.h"
#include "opendnp3/objects/Group122.h"
#include "opendnp3/app/IINField.h"

namespace opendnp3
{

/**
 * Base class used to handle APDU object headers
 */
class IAPDUHandler : public IWhiteList
{
public:

	IAPDUHandler();

	// read any accumulated errors
	IINField Errors() const;

	void OnHeader(const AllObjectsHeader& header);
	void OnHeader(const RangeHeader& header);
	void OnHeader(const CountHeader& header);

	void OnHeader(const FreeFormatHeader& header, const Group120Var1& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var2& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var5& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var6& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var7& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var8& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var9& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var10& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var11& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var12& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var13& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var14& value, const openpal::RSlice& object);
	void OnHeader(const FreeFormatHeader& header, const Group120Var15& value, const openpal::RSlice& object);

	void OnHeader(const CountHeader& header, const ICollection<Group50Var1>& values);
	void OnHeader(const CountHeader& header, const ICollection<Group51Var1>& values);
	void OnHeader(const CountHeader& header, const ICollection<Group51Var2>& values);
	void OnHeader(const CountHeader& header, const ICollection<Group52Var1>& values);
	void OnHeader(const CountHeader& header, const ICollection<Group52Var2>& values);
	void OnHeader(const CountHeader& header, const ICollection<Group120Var3>& values);
	void OnHeader(const CountHeader& header, const ICollection<Group120Var4>& values);

	void OnHeader(const RangeHeader& header, const ICollection<Indexed<IINValue>>& values);
	void OnHeader(const RangeHeader& header, const ICollection<Indexed<Binary>>& values);
	void OnHeader(const RangeHeader& header, const ICollection<Indexed<DoubleBitBinary>>& values);
	void OnHeader(const RangeHeader& header, const ICollection<Indexed<BinaryOutputStatus>>& values);
	void OnHeader(const RangeHeader& header, const ICollection<Indexed<Counter>>& values);
	void OnHeader(const RangeHeader& header, const ICollection<Indexed<FrozenCounter>>& values);
	void OnHeader(const RangeHeader& header, const ICollection<Indexed<Analog>>& values) ;
	void OnHeader(const RangeHeader& header, const ICollection<Indexed<AnalogOutputStatus>>& values);
	void OnHeader(const RangeHeader& header, const ICollection<Indexed<OctetString>>& values);
	void OnHeader(const RangeHeader& header, const ICollection<Indexed<TimeAndInterval>>& values);
	void OnHeader(const RangeHeader& header, const ICollection<Indexed<Group121Var1>>& values);

	// events

	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<Binary>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<BinaryOutputStatus>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<DoubleBitBinary>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<Counter>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<FrozenCounter>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<Analog>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputStatus>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<OctetString>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<TimeAndInterval>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<BinaryCommandEvent>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogCommandEvent>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<Group122Var1>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<Group122Var2>>& values);

	// commands

	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<ControlRelayOutputBlock>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputInt16>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputInt32>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputFloat32>>& values);
	void OnHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputDouble64>>& values);

protected:

	void Reset();

	uint32_t NumIgnoredHeaders() const
	{
		return numIgnoredHeaders;
	}

	inline uint32_t GetCurrentHeader()
	{
		return numTotalHeaders;
	}

	inline bool IsFirstHeader()
	{
		return numTotalHeaders == 0;
	}


	virtual IINField ProcessHeader(const AllObjectsHeader& record);
	virtual IINField ProcessHeader(const RangeHeader& header);
	virtual IINField ProcessHeader(const CountHeader& header);

	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var1& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var2& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var5& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var6& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var8& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var7& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var9& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var10& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var11& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var12& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var13& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var14& value, const openpal::RSlice& object);
	virtual IINField ProcessHeader(const FreeFormatHeader& header, const Group120Var15& value, const openpal::RSlice& object);


	virtual IINField ProcessHeader(const CountHeader& header, const ICollection<Group50Var1>& values);
	virtual IINField ProcessHeader(const CountHeader& header, const ICollection<Group51Var1>& values);
	virtual IINField ProcessHeader(const CountHeader& header, const ICollection<Group51Var2>& values);
	virtual IINField ProcessHeader(const CountHeader& header, const ICollection<Group52Var1>& values);
	virtual IINField ProcessHeader(const CountHeader& header, const ICollection<Group52Var2>& values);
	virtual IINField ProcessHeader(const CountHeader& header, const ICollection<Group120Var3>& values);
	virtual IINField ProcessHeader(const CountHeader& header, const ICollection<Group120Var4>& values);

	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<IINValue>>& values);
	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Binary>>& values);
	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<DoubleBitBinary>>& values);
	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<BinaryOutputStatus>>& values);
	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Counter>>& values);
	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<FrozenCounter>>& values);
	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Analog>>& values);
	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<AnalogOutputStatus>>& values);
	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<OctetString>>& values);
	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<TimeAndInterval>>& values);
	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Group121Var1>>& values);

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Binary>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<BinaryOutputStatus>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<DoubleBitBinary>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Counter>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<FrozenCounter>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Analog>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputStatus>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<OctetString>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<TimeAndInterval>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<BinaryCommandEvent>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogCommandEvent>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Group122Var1>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Group122Var2>>& values);

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<ControlRelayOutputBlock>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputInt16>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputInt32>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputFloat32>>& values);
	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputDouble64>>& values);

protected:

	// overridable to receive post processing events for every header
	virtual void OnHeaderResult(const HeaderRecord& record, const IINField& result) {}

private:

	inline void Record(const HeaderRecord& record, const IINField& result)
	{
		errors |= result;
		++numTotalHeaders;
		this->OnHeaderResult(record, result);
	}

	inline IINField ProcessUnsupportedHeader()
	{
		++numIgnoredHeaders;
		return IINField(IINBit::FUNC_NOT_SUPPORTED);
	}

	IINField errors;
	uint32_t numTotalHeaders;
	uint32_t numIgnoredHeaders;
};

}



#endif

