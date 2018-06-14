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
#include "CommandActionAdapter.h"

#include "opendnp3/app/ITransactable.h"

namespace opendnp3
{

CommandActionAdapter::CommandActionAdapter(ICommandHandler* handler, bool isSelect, OperateType opType) :
	m_handler(handler),
	m_isSelect(isSelect),
	m_opType(opType),
	m_isStarted(false)
{}

CommandActionAdapter::~CommandActionAdapter()
{
	if (m_isStarted)
	{
		Transaction::End(m_handler);
	}
}

void CommandActionAdapter::CheckStart()
{
	if (!m_isStarted)
	{
		m_isStarted = true;
		Transaction::Start(m_handler);
	}
}

CommandStatus CommandActionAdapter::Action(const ControlRelayOutputBlock& command, uint16_t index)
{
	return this->ActionT(command, index);
}

CommandStatus CommandActionAdapter::Action(const AnalogOutputInt16& command, uint16_t index)
{
	return this->ActionT(command, index);
}

CommandStatus CommandActionAdapter::Action(const AnalogOutputInt32& command, uint16_t index)
{
	return this->ActionT(command, index);
}

CommandStatus CommandActionAdapter::Action(const AnalogOutputFloat32& command, uint16_t index)
{
	return this->ActionT(command, index);
}

CommandStatus CommandActionAdapter::Action(const AnalogOutputDouble64& command, uint16_t index)
{
	return this->ActionT(command, index);
}

}
