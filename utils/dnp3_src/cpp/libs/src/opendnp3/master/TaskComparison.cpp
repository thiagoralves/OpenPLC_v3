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
#include "TaskComparison.h"

using namespace openpal;

namespace opendnp3
{

TaskComparison::Result TaskComparison::SelectHigherPriority(const openpal::MonotonicTimestamp& now, const IMasterTask& lhs, const IMasterTask& rhs, ITaskFilter& filter)
{
	// if one of the tasks is disabled, pick the enabled one

	bool leftEnabled = TaskComparison::Enabled(lhs, filter);
	bool rightEnabled = TaskComparison::Enabled(rhs, filter);

	if (!leftEnabled || !rightEnabled)
	{
		// always prefer the enabled task over the one that is disabled
		return rightEnabled ? Result::Right : Result::Left;
	}

	if ((lhs.Priority() > rhs.Priority()) && rhs.BlocksLowerPriority())
	{
		return Result::Right;
	}
	else if (rhs.Priority() > lhs.Priority() && lhs.BlocksLowerPriority())
	{
		return Result::Left;
	}
	else // equal priority or neither task blocks lower priority tasks, compare based on time
	{
		auto tlhs = lhs.ExpirationTime();
		auto trhs = rhs.ExpirationTime();
		auto lhsExpired = tlhs.milliseconds <= now.milliseconds;
		auto rhsExpired = trhs.milliseconds <= now.milliseconds;

		if (lhsExpired && rhsExpired)
		{
			// both expired, compare based on priority
			return HigherPriority(lhs, rhs);
		}
		else
		{
			if (tlhs.milliseconds < trhs.milliseconds)
			{
				return Result::Left;
			}
			else if (tlhs.milliseconds > trhs.milliseconds)
			{
				return Result::Right;
			}
			else
			{
				// if equal times, compare based on priority
				return HigherPriority(lhs, rhs);
			}
		}
	}
}

bool TaskComparison::Enabled(const IMasterTask& task, ITaskFilter& filter)
{
	return !task.ExpirationTime().IsMax() && filter.CanRun(task);
}

TaskComparison::Result TaskComparison::HigherPriority(const IMasterTask& lhs, const IMasterTask& rhs)
{
	if (lhs.Priority() < rhs.Priority())
	{
		return Result::Left;
	}
	else if (rhs.Priority() < lhs.Priority())
	{
		return Result::Right;
	}
	else
	{
		return Result::Same;
	}
}

}


