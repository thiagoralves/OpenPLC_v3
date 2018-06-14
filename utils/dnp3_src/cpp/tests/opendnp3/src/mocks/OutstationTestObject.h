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
#ifndef __OUTSTATION_TEST_OBJECT_H_
#define __OUTSTATION_TEST_OBJECT_H_

#include <opendnp3/LogLevels.h>
#include <opendnp3/app/ITransactable.h>
#include <opendnp3/outstation/Database.h>
#include <opendnp3/outstation/OutstationContext.h>

#include <functional>

#include <testlib/MockExecutor.h>
#include <testlib/MockLogHandler.h>

#include <dnp3mocks/MockCommandHandler.h>
#include <dnp3mocks/MockLowerLayer.h>
#include <dnp3mocks/MockOutstationApplication.h>

#include <functional>

namespace opendnp3
{

class OutstationTestObject
{

public:
	OutstationTestObject(const OutstationConfig& config, const DatabaseSizes& dbSizes = DatabaseSizes::Empty());


	size_t SendToOutstation(const std::string& hex);

	size_t LowerLayerUp();

	size_t LowerLayerDown();

	size_t OnSendResult(bool isSuccess);

	size_t NumPendingTimers() const;

	bool AdvanceToNextTimer();

	size_t AdvanceTime(const openpal::TimeDuration& td);

	testlib::MockLogHandler log;

	void Transaction(const std::function<void (IUpdateHandler&)>& apply)
	{
		//auto& handler = context.GetUpdateHandler();
		apply(context.GetUpdateHandler());
		context.CheckForTaskStart();
	}

private:

	const std::shared_ptr<testlib::MockExecutor> exe;

public:

	const std::shared_ptr<MockLowerLayer> lower;
	const std::shared_ptr<MockCommandHandler> cmdHandler;
	const std::shared_ptr<MockOutstationApplication> application;
	OContext context;
};


}

#endif
