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
#ifndef OPENDNP3_COMMAND_SET_H
#define OPENDNP3_COMMAND_SET_H

#include "opendnp3/master/ICommandCollection.h"

#include "opendnp3/app/ControlRelayOutputBlock.h"
#include "opendnp3/app/AnalogOutput.h"
#include "opendnp3/app/Indexed.h"

#include <vector>
#include <initializer_list>

namespace opendnp3
{

class ICommandHeader;

/**
* Provides a mechanism for building a set of one or more command headers
*/
class CommandSet final
{
	// friend class used to hide some implementation details while keeping the headers private
	friend class CommandSetOps;

public:

	typedef std::vector<ICommandHeader*> HeaderVector;

	/// Contrsuct an empty command set
	CommandSet() {}

	/// Construct a new command set and take ownership of the headers in argument
	CommandSet(CommandSet&& other);

	/// Destruct the command set and free any referenced headers
	~CommandSet();

	/// Construct a command set from a list of CROB
	CommandSet(std::initializer_list<Indexed<ControlRelayOutputBlock>> items);

	/// Construct a command set from a list of AOInt16
	CommandSet(std::initializer_list<Indexed<AnalogOutputInt16>> items);

	/// Construct a command set from a list of AOInt32
	CommandSet(std::initializer_list<Indexed<AnalogOutputInt32>> items);

	/// Construct a command set from a list of AOFloat32
	CommandSet(std::initializer_list<Indexed<AnalogOutputFloat32>> items);

	/// Construct a command set from a list of AODouble64
	CommandSet(std::initializer_list<Indexed<AnalogOutputDouble64>> items);

	/// Convenience functions that can build an entire header in one call
	template <class T>
	void Add(std::initializer_list<Indexed<T>> items)
	{
		if (!items.size() == 0)
		{
			auto& header = this->StartHeader<T>();
			for (auto& command : items)
			{
				header.Add(command.value, command.index);
			}
		}
	}

	/// Convenience functions that can build an entire header in one call
	template <class T>
	void Add(std::vector<Indexed<T>> items)
	{
		if (!items.empty())
		{
			auto& header = this->StartHeader<T>();
			for (auto& command : items)
			{
				header.Add(command.value, command.index);
			}
		}
	}

	/// Begin a header of the parameterized type
	template <class T>
	ICommandCollection<T>& StartHeader();


private:

	template <class T>
	void AddAny(std::initializer_list<Indexed<T>> items);

	ICommandCollection<ControlRelayOutputBlock>& StartHeaderCROB();
	ICommandCollection<AnalogOutputInt32>& StartHeaderAOInt32();
	ICommandCollection<AnalogOutputInt16>& StartHeaderAOInt16();
	ICommandCollection<AnalogOutputFloat32>& StartHeaderAOFloat32();
	ICommandCollection<AnalogOutputDouble64>& StartHeaderAODouble64();

	CommandSet(const CommandSet&) = delete;
	CommandSet& operator= (const CommandSet& other) = delete;

	HeaderVector m_headers;
};

template <>
inline ICommandCollection<ControlRelayOutputBlock>& CommandSet::StartHeader()
{
	return this->StartHeaderCROB();
}

template <>
inline ICommandCollection<AnalogOutputInt16>& CommandSet::StartHeader()
{
	return this->StartHeaderAOInt16();
}

template <>
inline ICommandCollection<AnalogOutputInt32>& CommandSet::StartHeader()
{
	return this->StartHeaderAOInt32();
}

template <>
inline ICommandCollection<AnalogOutputFloat32>& CommandSet::StartHeader()
{
	return this->StartHeaderAOFloat32();
}

template <>
inline ICommandCollection<AnalogOutputDouble64>& CommandSet::StartHeader()
{
	return this->StartHeaderAODouble64();
}

}

#endif
