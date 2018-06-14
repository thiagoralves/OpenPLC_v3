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

#include "opendnp3/master/SimpleRequestTaskBase.h"

using namespace openpal;

namespace opendnp3
{

SimpleRequestTaskBase::SimpleRequestTaskBase(IMasterApplication& app, FunctionCode func, int taskPriority, const HeaderBuilderT& format, openpal::Logger logger, const TaskConfig& config) :
	IMasterTask(app, MonotonicTimestamp::Min(), logger, config),
	m_func(func),
	m_priority(taskPriority),
	m_format(format)
{

}


bool SimpleRequestTaskBase::BuildRequest(APDURequest& request, uint8_t seq)
{
	request.SetFunction(m_func);
	request.SetControl(AppControlField::Request(seq));
	auto writer = request.GetWriter();
	return m_format(writer);
}

} //end ns



