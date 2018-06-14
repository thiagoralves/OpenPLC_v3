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
#ifndef OPENDNP3_CONTROLSTATE_H
#define OPENDNP3_CONTROLSTATE_H


#include <openpal/executor/MonotonicTimestamp.h>

#include "opendnp3/gen/CommandStatus.h"
#include "opendnp3/link/CRC.h"
#include "opendnp3/app/AppSeqNum.h"


namespace opendnp3
{

///
/// Represent all of the mutable state for SBO controls
///
class ControlState
{

public:

	ControlState() : digest(0), length(0)
	{}

	CommandStatus ValidateSelection(const AppSeqNum& seq, const openpal::MonotonicTimestamp& now, const openpal::TimeDuration& timeout, const openpal::RSlice& objects) const
	{
		if (expectedSeq.Equals(seq))
		{
			if (selectTime.milliseconds <= now.milliseconds)
			{
				auto elapsed = now.milliseconds - selectTime.milliseconds;
				if (elapsed < timeout)
				{
					if (length == objects.Size() && digest == CRC::CalcCrc(objects))
					{
						return CommandStatus::SUCCESS;
					}
					else
					{
						return CommandStatus::NO_SELECT;
					}
				}
				else
				{
					return CommandStatus::TIMEOUT;
				}
			}
			else
			{
				return CommandStatus::TIMEOUT;
			}
		}
		else
		{
			return CommandStatus::NO_SELECT;
		}
	}

	void Select(const AppSeqNum& currentSeqN, const openpal::MonotonicTimestamp& now, const openpal::RSlice& objects)
	{
		selectTime = now;
		expectedSeq = currentSeqN.Next();
		digest = CRC::CalcCrc(objects);
		length = objects.Size();
	}

private:

	AppSeqNum expectedSeq;
	openpal::MonotonicTimestamp selectTime;
	uint16_t digest;
	uint32_t length;

};


}

#endif
