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
#ifndef OPENDNP3_SIMPLE_REQUEST_TASK_BASE_H
#define OPENDNP3_SIMPLE_REQUEST_TASK_BASE_H

#include "opendnp3/master/IMasterTask.h"
#include "opendnp3/master/HeaderBuilder.h"

#include <string>

namespace opendnp3
{

class IMasterApplication;

class SimpleRequestTaskBase : public IMasterTask
{

public:

	SimpleRequestTaskBase(IMasterApplication& app, FunctionCode func, int taskPriority, const HeaderBuilderT& format, openpal::Logger logger, const TaskConfig& config);

	virtual bool IsRecurring() const override final
	{
		return false;
	}

	virtual bool BuildRequest(APDURequest& request, uint8_t seq) override final;

	virtual int Priority(void) const override final
	{
		return m_priority;
	}

	virtual bool BlocksLowerPriority() const override final
	{
		return false;
	}

protected:

	const FunctionCode m_func;

private:

	const int m_priority;
	const HeaderBuilderT m_format;


	virtual bool IsEnabled() const override final
	{
		return true;
	}

	virtual MasterTaskType GetTaskType() const override final
	{
		return MasterTaskType::USER_TASK;
	}

};

} //end ns


#endif
