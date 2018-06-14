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
/**
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

#include "opendnp3/app/BinaryCommandEvent.h"

namespace opendnp3
{

BinaryCommandEvent::BinaryCommandEvent() :
	value(false),
	status(CommandStatus::SUCCESS),
	time(0)
{}

BinaryCommandEvent::BinaryCommandEvent(Flags flags) :
	value(GetValueFromFlags(flags)),
	status(GetStatusFromFlags(flags))
{}

BinaryCommandEvent::BinaryCommandEvent(Flags flags, DNPTime time) :
	value(GetValueFromFlags(flags)),
	status(GetStatusFromFlags(flags)),
	time(time)
{}

BinaryCommandEvent::BinaryCommandEvent(bool value, CommandStatus status) :
	value(value),
	status(status)
{}

BinaryCommandEvent::BinaryCommandEvent(bool value, CommandStatus status, DNPTime time) :
	value(value),
	status(status),
	time(time)
{}

Flags BinaryCommandEvent::GetFlags() const
{
	return (static_cast<uint8_t>(value) << 7) | (CommandStatusToType(status));
}

bool BinaryCommandEvent::operator==(const BinaryCommandEvent& rhs) const
{
	return value == rhs.value && status == rhs.status && time == rhs.time;
}

bool BinaryCommandEvent::GetValueFromFlags(Flags flags)
{
	return (flags.value & ValueMask) == ValueMask;
}

CommandStatus BinaryCommandEvent::GetStatusFromFlags(Flags flags)
{
	return CommandStatusFromType(flags.value & StatusMask);
}

}
