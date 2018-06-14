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

#ifndef OPENDNP3_COMMAND_POINT_RESULT_H
#define OPENDNP3_COMMAND_POINT_RESULT_H

#include "opendnp3/gen/CommandStatus.h"
#include "opendnp3/gen/CommandPointState.h"

namespace opendnp3
{

/// Represents the result of a command operation on a particular point
class CommandPointResult
{

public:

	/// Fully construct based on all members
	CommandPointResult(uint32_t headerIndex_, uint16_t index_, CommandPointState state_, CommandStatus status_) :
		headerIndex(headerIndex_),
		index(index_),
		state(state_),
		status(status_)
	{}

	/// Check the result for equality against another value
	bool Equals(const CommandPointResult& other) const
	{
		return	(headerIndex == other.headerIndex) &&
		        (index == other.index) &&
		        (state == other.state) &&
		        (status == other.status);
	}

	/// The index of the header when request was made (0-based)
	uint32_t headerIndex;

	/// The index of the command that was requested
	uint16_t index;

	/// The final state of the command operation on this point
	CommandPointState state;

	/// The response value. This is only valid if state == SUCCESS or state == SELECT_FAIL
	CommandStatus status;
};

}

#endif
