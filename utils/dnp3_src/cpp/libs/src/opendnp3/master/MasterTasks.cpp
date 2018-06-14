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
#include "MasterTasks.h"

using namespace openpal;

namespace opendnp3
{

MasterTasks::MasterTasks(const MasterParams& params, const openpal::Logger& logger, IMasterApplication& app, ISOEHandler& SOEHandler) :
	enableUnsol(std::make_shared<EnableUnsolicitedTask>(app, params.unsolClassMask, params.taskRetryPeriod, logger)),
	clearRestart(std::make_shared<ClearRestartTask>(app, params.taskRetryPeriod, logger)),
	assignClass(std::make_shared<AssignClassTask>(app, params.taskRetryPeriod, logger)),
	startupIntegrity(std::make_shared<StartupIntegrityPoll>(app, SOEHandler, params.startupIntegrityClassMask, params.taskRetryPeriod, logger)),
	disableUnsol(std::make_shared<DisableUnsolicitedTask>(app, params.disableUnsolOnStartup, params.taskRetryPeriod, logger)),
	timeSync(std::make_shared<SerialTimeSyncTask>(app, logger)),
	eventScan(std::make_shared<EventScanTask>(app, SOEHandler, params.eventScanOnEventsAvailableClassMask, params.taskRetryPeriod, logger))
{

}

void MasterTasks::Initialize(MasterScheduler& scheduler)
{
	scheduler.Schedule(enableUnsol);
	scheduler.Schedule(clearRestart);
	scheduler.Schedule(assignClass);
	scheduler.Schedule(startupIntegrity);
	scheduler.Schedule(disableUnsol);
	scheduler.Schedule(timeSync);
	scheduler.Schedule(eventScan);

	for (auto& task : boundTasks)
	{
		scheduler.Schedule(task);
	}
}

void MasterTasks::BindTask(const std::shared_ptr<IMasterTask>& task)
{
	boundTasks.push_back(task);
}

}
