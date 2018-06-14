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
#ifndef OPENDNP3_MOCKSOEHANDLER_H_
#define OPENDNP3_MOCKSOEHANDLER_H_

#include <opendnp3/master/ISOEHandler.h>

#include <map>


namespace opendnp3
{

// simple measurement handler for testing purposes
class MockSOEHandler : public ISOEHandler
{
public:

	template <class T>
	class Record
	{
	public:

		Record() : sequence(0)
		{}

		Record(const T& meas_, const HeaderInfo& info_, uint32_t sequence_) :
			meas(meas_),
			info(info_),
			sequence(sequence_)
		{}


		T meas;
		HeaderInfo info;
		uint32_t sequence;
	};

	MockSOEHandler() : soeCount(0)
	{}

	uint32_t TotalReceived() const
	{
		return soeCount;
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Binary>>& values) override final
	{
		this->RecordAny(info, values, this->binarySOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<DoubleBitBinary>>& values) override final
	{
		this->RecordAny(info, values, this->doubleBinarySOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Analog>>& values) override final
	{
		this->RecordAny(info, values, this->analogSOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Counter>>& values) override final
	{
		this->RecordAny(info, values, this->counterSOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<FrozenCounter>>& values) override final
	{
		this->RecordAny(info, values, this->frozenCounterSOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryOutputStatus>>& values) override final
	{
		this->RecordAny(info, values, this->binaryOutputStatusSOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogOutputStatus>>& values) override final
	{
		this->RecordAny(info, values, this->analogOutputStatusSOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<OctetString>>& values) override final
	{
		this->RecordAny(info, values, this->octetStringSOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<TimeAndInterval>>& values) override final
	{
		this->RecordAny(info, values, this->timeAndIntervalSOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryCommandEvent>>& values) override final
	{
		this->RecordAny(info, values, this->binaryCommandEventSOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogCommandEvent>>& values) override final
	{
		this->RecordAny(info, values, this->analogCommandEventSOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<Indexed<SecurityStat>>& values) override final
	{
		this->RecordAny(info, values, this->securityStatSOE);
	}

	virtual void Process(const HeaderInfo& info, const ICollection<DNPTime>& values) override final
	{
		values.ForeachItem([this](const DNPTime & value)
		{
			++this->soeCount;
			this->timeSOE.push_back(value);
		});
	}

	void Clear()
	{
		soeCount = 0;

		binarySOE.clear();
		doubleBinarySOE.clear();
		analogSOE.clear();
		counterSOE.clear();
		frozenCounterSOE.clear();
		binaryOutputStatusSOE.clear();
		analogOutputStatusSOE.clear();
		octetStringSOE.clear();
		timeAndIntervalSOE.clear();
		binaryCommandEventSOE.clear();
		analogCommandEventSOE.clear();
		securityStatSOE.clear();
		timeSOE.clear();
	}

	std::map<uint16_t, Record<Binary>> binarySOE;
	std::map<uint16_t, Record<DoubleBitBinary>> doubleBinarySOE;
	std::map<uint16_t, Record<Analog>> analogSOE;
	std::map<uint16_t, Record<Counter>> counterSOE;
	std::map<uint16_t, Record<FrozenCounter>> frozenCounterSOE;
	std::map<uint16_t, Record<BinaryOutputStatus>> binaryOutputStatusSOE;
	std::map<uint16_t, Record<AnalogOutputStatus>> analogOutputStatusSOE;
	std::map<uint16_t, Record<OctetString>> octetStringSOE;
	std::map<uint16_t, Record<TimeAndInterval>> timeAndIntervalSOE;
	std::map<uint16_t, Record<BinaryCommandEvent>> binaryCommandEventSOE;
	std::map<uint16_t, Record<AnalogCommandEvent>> analogCommandEventSOE;
	std::map<uint16_t, Record<SecurityStat>> securityStatSOE;
	std::vector<DNPTime> timeSOE;

protected:

	void Start() override {}
	void End() override {}

private:

	uint32_t soeCount;

	template <class T>
	void RecordAny(const HeaderInfo& info, const ICollection<Indexed<T>>& values, std::map<uint16_t, Record<T> >& records)
	{
		auto process = [this, info, &records](const Indexed<T>& pair)
		{
			Record<T> record(pair.value, info, soeCount);
			records[pair.index] = record;
			++this->soeCount;
		};

		values.ForeachItem(process);
	}




};

}

#endif

