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
#include "opendnp3/outstation/SimpleCommandHandler.h"

namespace opendnp3
{

SimpleCommandHandler::SimpleCommandHandler(CommandStatus status_) : status(status_), numOperate(0), numSelect(0), numStart(0), numEnd(0)
{

}

CommandStatus SimpleCommandHandler::Select(const ControlRelayOutputBlock& command, uint16_t index)
{
	this->DoSelect(command, index);
	++numSelect;
	return status;
}
CommandStatus SimpleCommandHandler::Operate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType)
{
	this->DoOperate(command, index, opType);
	++numOperate;
	return status;
}

CommandStatus SimpleCommandHandler::Select(const AnalogOutputInt16& command, uint16_t index)
{
	this->DoSelect(command, index);
	++numSelect;
	return status;
}
CommandStatus SimpleCommandHandler::Operate(const AnalogOutputInt16& command, uint16_t index, OperateType opType)
{
	this->DoOperate(command, index, opType);
	++numOperate;
	return status;
}

CommandStatus SimpleCommandHandler::Select(const AnalogOutputInt32& command, uint16_t index)
{
	this->DoSelect(command, index);
	++numSelect;
	return status;
}
CommandStatus SimpleCommandHandler::Operate(const AnalogOutputInt32& command, uint16_t index, OperateType opType)
{
	this->DoOperate(command, index, opType);
	++numOperate;
	return status;
}

CommandStatus SimpleCommandHandler::Select(const AnalogOutputFloat32& command, uint16_t index)
{
	this->DoSelect(command, index);
	++numSelect;
	return status;
}
CommandStatus SimpleCommandHandler::Operate(const AnalogOutputFloat32& command, uint16_t index, OperateType opType)
{
	this->DoOperate(command, index, opType);
	++numOperate;
	return status;
}

CommandStatus SimpleCommandHandler::Select(const AnalogOutputDouble64& command, uint16_t index)
{
	this->DoSelect(command, index);
	++numSelect;
	return status;
}
CommandStatus SimpleCommandHandler::Operate(const AnalogOutputDouble64& command, uint16_t index, OperateType opType)
{
	this->DoOperate(command, index, opType);
	++numOperate;
	return status;
}

void SimpleCommandHandler::Start()
{
	++numStart;
}

void SimpleCommandHandler::End()
{
	++numEnd;
}

}


