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
#ifndef OPENDNP3_MASTERTASKS_H
#define OPENDNP3_MASTERTASKS_H

#include "opendnp3/master/ClearRestartTask.h"
#include "opendnp3/master/AssignClassTask.h"
#include "opendnp3/master/EnableUnsolicitedTask.h"
#include "opendnp3/master/StartupIntegrityPoll.h"
#include "opendnp3/master/DisableUnsolicitedTask.h"
#include "opendnp3/master/SerialTimeSyncTask.h"
#include "opendnp3/master/CommandTask.h"
#include "opendnp3/master/EventScanTask.h"

#include "opendnp3/master/MasterParams.h"
#include "opendnp3/master/MasterScheduler.h"

#include <vector>

namespace opendnp3
{

class IMasterApplication;

class MasterTasks
{

public:

	MasterTasks(const MasterParams& params, const openpal::Logger& logger, IMasterApplication& application, ISOEHandler& SOEHandler);

	void Initialize(MasterScheduler& scheduler);

	// master tasks that can be "failed" (startup and in response to IIN bits)
	const std::shared_ptr<EnableUnsolicitedTask> enableUnsol;
	const std::shared_ptr<ClearRestartTask> clearRestart;
	const std::shared_ptr<AssignClassTask> assignClass;
	const std::shared_ptr<StartupIntegrityPoll> startupIntegrity;
	const std::shared_ptr<DisableUnsolicitedTask> disableUnsol;
	const std::shared_ptr<SerialTimeSyncTask >timeSync;
	const std::shared_ptr<EventScanTask> eventScan;


	void BindTask(const std::shared_ptr<IMasterTask>& task);

private:

	std::vector<std::shared_ptr<IMasterTask>> boundTasks;

};

}



#endif
