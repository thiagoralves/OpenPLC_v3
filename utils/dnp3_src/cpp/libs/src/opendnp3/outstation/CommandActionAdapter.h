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
#ifndef OPENDNP3_COMMANDACTIONADAPTER_H
#define OPENDNP3_COMMANDACTIONADAPTER_H

#include "ICommandAction.h"

#include "opendnp3/outstation/ICommandHandler.h"

namespace opendnp3
{

/**
* Interface used to dispatch an abstract action using a command
*/
class CommandActionAdapter : public ICommandAction
{

public:

	CommandActionAdapter(ICommandHandler* handler, bool isSelect, OperateType opType);

	~CommandActionAdapter();

	virtual CommandStatus Action(const ControlRelayOutputBlock& command, uint16_t index) final;

	virtual CommandStatus Action(const AnalogOutputInt16& command, uint16_t index) final;

	virtual CommandStatus Action(const AnalogOutputInt32& command, uint16_t index) final;

	virtual CommandStatus Action(const AnalogOutputFloat32& command, uint16_t index) final;

	virtual CommandStatus Action(const AnalogOutputDouble64& command, uint16_t index) final;

private:

	template <class T>
	CommandStatus ActionT(const T& command, uint16_t index)
	{
		this->CheckStart();
		return m_isSelect ? m_handler->Select(command, index) : m_handler->Operate(command, index, m_opType);
	}

	void CheckStart();

	ICommandHandler* m_handler;
	bool m_isSelect;
	OperateType m_opType;
	bool m_isStarted;

};

}

#endif

