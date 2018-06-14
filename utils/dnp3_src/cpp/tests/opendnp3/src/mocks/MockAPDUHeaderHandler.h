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
#ifndef __MOCK_APDU_HEADER_HANDLER_H_
#define __MOCK_APDU_HEADER_HANDLER_H_

#include <opendnp3/app/parsing/IAPDUHandler.h>

#include <vector>

namespace opendnp3
{

class MockApduHeaderHandler : public IAPDUHandler
{
public:

	virtual bool IsAllowed(uint32_t headerCount, GroupVariation gv, QualifierCode qc) override final
	{
		return true;
	}

	virtual void OnHeaderResult(const HeaderRecord& record, const IINField& result) override final
	{
		records.push_back(record);
	}

	virtual IINField ProcessHeader(const CountHeader& header, const ICollection<Group120Var4>& values) override final
	{
		return ProcessAny(header, values, authStatusRequests);
	}

	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<IINValue>>& values) override final
	{
		return ProcessAny(header, values, iinBits);
	}

	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Binary>>& values) override final
	{
		return ProcessAny(header, values, staticBinaries);
	}

	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<DoubleBitBinary>>& values) override final
	{
		return ProcessAny(header, values, staticDoubleBinaries);
	}

	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<BinaryOutputStatus>>& values) override final
	{
		return ProcessAny(header, values, staticControlStatii);
	}

	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Counter>>& values) override final
	{
		return ProcessAny(header, values, staticCounters);
	}

	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<FrozenCounter>>& values) override final
	{
		return ProcessAny(header, values, staticFrozenCounters);
	}

	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Analog>>& values) override final
	{
		return ProcessAny(header, values, eventAnalogs);
	}

	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<AnalogOutputStatus>>& values) override final
	{
		return ProcessAny(header, values, staticSetpointStatii);
	}

	virtual IINField ProcessHeader(const RangeHeader& header, const ICollection<Indexed<OctetString>>& values) override final
	{
		return ProcessAny(header, values, rangedOctets);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Binary>>& values) override final
	{
		return this->ProcessAny(header, values, eventBinaries);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<DoubleBitBinary>>& values) override final
	{
		return this->ProcessAny(header, values, eventDoubleBinaries);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Counter>>& values) override final
	{
		return this->ProcessAny(header, values, eventCounters);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<FrozenCounter>>& values) override final
	{
		return this->ProcessAny(header, values, eventFrozenCounters);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Analog>>& values) override final
	{
		return this->ProcessAny(header, values, eventAnalogs);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<OctetString>>& values) override final
	{
		return this->ProcessAny(header, values, indexPrefixedOctets);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<BinaryCommandEvent>>& values) override final
	{
		return this->ProcessAny(header, values, binaryCommandEvents);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogCommandEvent>>& values) override final
	{
		return this->ProcessAny(header, values, analogCommandEvents);
	}

	/// --- controls ----

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<ControlRelayOutputBlock>>& meas) override final
	{
		return this->ProcessAny(header, meas, crobRequests);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputInt16>>& meas) override final
	{
		return this->ProcessAny(header, meas, aoInt16Requests);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputInt32>>& meas) override final
	{
		return this->ProcessAny(header, meas, aoInt32Requests);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputFloat32>>& meas) override final
	{
		return this->ProcessAny(header, meas, aoFloat32Requests);
	}

	virtual IINField ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputDouble64>>& meas) override final
	{
		return this->ProcessAny(header, meas, aoDouble64Requests);
	}

	virtual IINField ProcessHeader(const opendnp3::FreeFormatHeader& header, const Group120Var1& value, const openpal::RSlice& object) override final
	{
		authChallenges.push_back(value);
		return IINField::Empty();
	}

	virtual IINField ProcessHeader(const opendnp3::FreeFormatHeader& header, const Group120Var2& value, const openpal::RSlice& object) override final
	{
		authReplys.push_back(value);
		return IINField::Empty();
	}

	virtual IINField ProcessHeader(const opendnp3::FreeFormatHeader& header, const Group120Var5& value, const openpal::RSlice& object) override final
	{
		authKeyStatusResponses.push_back(value);
		return IINField::Empty();
	}

	virtual IINField ProcessHeader(const opendnp3::FreeFormatHeader& header, const Group120Var6& value, const openpal::RSlice& object) override final
	{
		authChanges.push_back(value);
		return IINField::Empty();
	}

	std::vector<HeaderRecord> records;

	std::vector<Group120Var1> authChallenges;
	std::vector<Group120Var2> authReplys;
	std::vector<Group120Var4> authStatusRequests;
	std::vector<Group120Var5> authKeyStatusResponses;
	std::vector<Group120Var6> authChanges;

	std::vector<Indexed<IINValue>> iinBits;

	std::vector<Indexed<Binary>> eventBinaries;
	std::vector<Indexed<Binary>> staticBinaries;

	std::vector<Indexed<DoubleBitBinary>> eventDoubleBinaries;
	std::vector<Indexed<DoubleBitBinary>> staticDoubleBinaries;

	std::vector<Indexed<BinaryOutputStatus>> staticControlStatii;

	std::vector<Indexed<Counter>> eventCounters;
	std::vector<Indexed<Counter>> staticCounters;

	std::vector<Indexed<FrozenCounter>> eventFrozenCounters;
	std::vector<Indexed<FrozenCounter>> staticFrozenCounters;

	std::vector<Indexed<Analog>> eventAnalogs;
	std::vector<Indexed<Analog>> staticAnalogs;

	std::vector<Indexed<AnalogOutputStatus>> staticSetpointStatii;

	std::vector<Indexed<ControlRelayOutputBlock>> crobRequests;

	std::vector<Indexed<AnalogOutputInt16>> aoInt16Requests;
	std::vector<Indexed<AnalogOutputInt32>> aoInt32Requests;
	std::vector<Indexed<AnalogOutputFloat32>> aoFloat32Requests;
	std::vector<Indexed<AnalogOutputDouble64>> aoDouble64Requests;

	std::vector<Indexed<OctetString>> indexPrefixedOctets;
	std::vector<Indexed<OctetString>> rangedOctets;

	std::vector<Indexed<BinaryCommandEvent>> binaryCommandEvents;

	std::vector<Indexed<AnalogCommandEvent>> analogCommandEvents;

private:

	template <class T>
	IINField ProcessAny(const HeaderRecord& record, const ICollection<T>& meas, std::vector<T>& items)
	{
		auto add = [&items](const T & v)
		{
			items.push_back(v);
		};
		meas.ForeachItem(add);
		return IINField::Empty();
	}
};

}

#endif
