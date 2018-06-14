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
#ifndef ASIODNP3_PRINTINGSOEHANDLER_H
#define ASIODNP3_PRINTINGSOEHANDLER_H

#include <opendnp3/master/ISOEHandler.h>

#include <iostream>
#include <sstream>
#include <memory>

namespace asiodnp3
{

/**
*	ISOEHandler singleton that prints to the console.
*/
class PrintingSOEHandler final : public opendnp3::ISOEHandler
{

public:

	PrintingSOEHandler()
	{}

	static std::shared_ptr<ISOEHandler> Create()
	{
		return std::make_shared<PrintingSOEHandler>();
	}

	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::Binary>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::DoubleBitBinary>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::Analog>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::Counter>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::FrozenCounter>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::BinaryOutputStatus>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::AnalogOutputStatus>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::OctetString>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::TimeAndInterval>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::BinaryCommandEvent>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::AnalogCommandEvent>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<opendnp3::SecurityStat>>& values) override;
	virtual void Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::DNPTime>& values) override;


protected:

	void Start() final {}
	void End() final {}

private:

	template <class T>
	static void PrintAll(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<T>>& values)
	{
		auto print = [&](const opendnp3::Indexed<T>& pair)
		{
			Print<T>(info, pair.value, pair.index);
		};
		values.ForeachItem(print);
	}

	template <class T>
	static void Print(const opendnp3::HeaderInfo& info, const T& value, uint16_t index)
	{
		std::cout << "[" << index << "] : " <<
		          ValueToString(value) << " : " <<
		          static_cast<int>(value.flags.value) << " : " <<
		          value.time << std::endl;
	}

	template <class T>
	static std::string ValueToString(const T& meas)
	{
		std::ostringstream oss;
		oss << meas.value;
		return oss.str();
	}

	static std::string GetTimeString(opendnp3::TimestampMode tsmode)
	{
		std::ostringstream oss;
		switch (tsmode)
		{
		case(opendnp3::TimestampMode::SYNCHRONIZED) :
			return "synchronized";
			break;
		case(opendnp3::TimestampMode::UNSYNCHRONIZED) :
			oss << "unsynchronized";
			break;
		default:
			oss << "no timestamp";
			break;
		}

		return oss.str();
	}

	static std::string ValueToString(const opendnp3::DoubleBitBinary& meas)
	{
		return opendnp3::DoubleBitToString(meas.value);
	}

};

}

#endif
