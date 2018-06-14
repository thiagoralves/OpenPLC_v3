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
#ifndef ASIODNP3_UPDATEBUILDER_H
#define ASIODNP3_UPDATEBUILDER_H

#include "asiodnp3/Updates.h"

namespace asiodnp3
{

class UpdateBuilder
{

public:

	UpdateBuilder& Update(const opendnp3::Binary& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	UpdateBuilder& Update(const opendnp3::DoubleBitBinary& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	UpdateBuilder& Update(const opendnp3::Analog& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	UpdateBuilder& Update(const opendnp3::Counter& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	UpdateBuilder& Update(const opendnp3::FrozenCounter& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	UpdateBuilder& Update(const opendnp3::BinaryOutputStatus& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	UpdateBuilder& Update(const opendnp3::AnalogOutputStatus& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	UpdateBuilder& Update(const opendnp3::TimeAndInterval& meas, uint16_t index);
	UpdateBuilder& Modify(opendnp3::FlagsType type, uint16_t start, uint16_t stop, uint8_t flags);

	Updates Build() const;

private:

	template <class T>
	UpdateBuilder& AddMeas(const T& meas, uint16_t index, opendnp3::EventMode mode);

	void Add(const update_func_t& fun);

	std::shared_ptr<shared_updates_t> updates;
};

}

#endif
