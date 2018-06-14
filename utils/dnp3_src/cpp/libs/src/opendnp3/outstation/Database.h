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
#ifndef OPENDNP3_DATABASE_H
#define OPENDNP3_DATABASE_H

#include "opendnp3/gen/IndexMode.h"
#include "opendnp3/gen/AssignClassType.h"

#include "opendnp3/outstation/IDatabase.h"
#include "opendnp3/outstation/IEventReceiver.h"
#include "opendnp3/outstation/DatabaseBuffers.h"

namespace opendnp3
{

/**
The database coordinates all updates of measurement data
*/
class Database final : public IDatabase, private openpal::Uncopyable
{
public:

	Database(const DatabaseSizes&, IEventReceiver& eventReceiver, IndexMode indexMode, StaticTypeBitField allowedClass0Types);

	// ------- IDatabase --------------

	virtual bool Update(const Binary&, uint16_t, EventMode = EventMode::Detect) override;
	virtual bool Update(const DoubleBitBinary&, uint16_t, EventMode = EventMode::Detect) override;
	virtual bool Update(const Analog&, uint16_t, EventMode = EventMode::Detect) override;
	virtual bool Update(const Counter&, uint16_t, EventMode = EventMode::Detect) override;
	virtual bool Update(const FrozenCounter&, uint16_t, EventMode = EventMode::Detect) override;
	virtual bool Update(const BinaryOutputStatus&, uint16_t, EventMode = EventMode::Detect) override;
	virtual bool Update(const AnalogOutputStatus&, uint16_t, EventMode = EventMode::Detect) override;
	virtual bool Update(const TimeAndInterval&, uint16_t) override;
	virtual bool Modify(FlagsType type, uint16_t start, uint16_t stop, uint8_t flags) override;

	// ------- Misc ---------------

	IResponseLoader& GetResponseLoader() override final
	{
		return buffers;
	}
	IStaticSelector& GetStaticSelector() override final
	{
		return buffers;
	}
	IClassAssigner& GetClassAssigner() override final
	{
		return buffers;
	}

	/**
	* @return A view of all the static data for configuration purposes
	*/
	DatabaseConfigView GetConfigView()
	{
		return buffers.buffers.GetView();
	}

private:

	template <class Spec>
	uint16_t GetRawIndex(uint16_t index);

	IEventReceiver* eventReceiver;
	IndexMode indexMode;

	static bool ConvertToEventClass(PointClass pc, EventClass& ec);

	template <class Spec>
	bool UpdateEvent(const typename Spec::meas_t& value, uint16_t index, EventMode mode);

	template <class Spec>
	bool UpdateAny(Cell<Spec>& cell, const typename Spec::meas_t& value, EventMode mode);

	template <class Spec>
	bool Modify(uint16_t start, uint16_t stop, uint8_t flags);

	// stores the most recent values, selected values, and metadata
	DatabaseBuffers buffers;
};



}

#endif

