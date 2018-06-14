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
#ifndef OPENDNP3_MASTERSCHEDULER_H
#define OPENDNP3_MASTERSCHEDULER_H

#include <openpal/executor/TimerRef.h>
#include <openpal/executor/IExecutor.h>
#include <openpal/container/Settable.h>

#include "opendnp3/master/UserPollTask.h"
#include "opendnp3/master/IMasterTask.h"
#include "opendnp3/master/IScheduleCallback.h"
#include "opendnp3/master/ITaskFilter.h"

#include <vector>
#include <functional>
#include <memory>

namespace opendnp3
{

class MasterScheduler
{

public:

	explicit MasterScheduler(ITaskFilter& filter);

	/*
	* Add a task to the scheduler
	*/
	void Schedule(const std::shared_ptr<IMasterTask>& task);

	/**
	* @return Task to start or undefined pointer if no task to start
	* If there is no task to start, 'next' is set to the timestamp when the scheduler should be re-evaluated
	*/
	std::shared_ptr<IMasterTask> GetNext(const openpal::MonotonicTimestamp& now, openpal::MonotonicTimestamp& next);

	/**
	* Cleanup all existing tasks & cancel any timers
	*/
	void Shutdown(const openpal::MonotonicTimestamp& now);

	/**
	* Check if any tasks have exceeded their start timeout
	*/
	void CheckTaskStartTimeout(const openpal::MonotonicTimestamp& now);

private:

	bool IsTimedOut(const openpal::MonotonicTimestamp& now, const std::shared_ptr<IMasterTask>& task);

	void RecalculateTaskStartTimeout();


	std::vector<std::shared_ptr<IMasterTask>>::iterator GetNextTask(const openpal::MonotonicTimestamp& now);

	ITaskFilter* m_filter;
	std::vector<std::shared_ptr<IMasterTask>> m_tasks;
};

}



#endif
