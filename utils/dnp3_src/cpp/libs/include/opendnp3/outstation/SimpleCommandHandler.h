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
#ifndef OPENDNP3_SIMPLECOMMANDHANDLER_H
#define OPENDNP3_SIMPLECOMMANDHANDLER_H

#include "ICommandHandler.h"

#include <memory>

namespace opendnp3
{

/**
* Mock ICommandHandler used for examples and demos
*/
class SimpleCommandHandler : public ICommandHandler
{
public:

	/**
	* @param status The status value to return in response to all commands
	*/
	SimpleCommandHandler(CommandStatus status);

	CommandStatus Select(const ControlRelayOutputBlock& command, uint16_t index) override final;
	CommandStatus Operate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType) override final;


	CommandStatus Select(const AnalogOutputInt16& command, uint16_t index) override final;
	CommandStatus Operate(const AnalogOutputInt16& command, uint16_t index, OperateType opType) override final;


	CommandStatus Select(const AnalogOutputInt32& command, uint16_t index) override final;
	CommandStatus Operate(const AnalogOutputInt32& command, uint16_t index, OperateType opType) override final;


	CommandStatus Select(const AnalogOutputFloat32& command, uint16_t index) override final;
	CommandStatus Operate(const AnalogOutputFloat32& command, uint16_t index, OperateType opType) override final;


	CommandStatus Select(const AnalogOutputDouble64& command, uint16_t index) override final;
	CommandStatus Operate(const AnalogOutputDouble64& command, uint16_t index, OperateType opType) override final;


protected:

	virtual void DoSelect(const ControlRelayOutputBlock& command, uint16_t index) {}
	virtual void DoOperate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType) {}

	virtual void DoSelect(const AnalogOutputInt16& command, uint16_t index) {}
	virtual void DoOperate(const AnalogOutputInt16& command, uint16_t index, OperateType opType) {}

	virtual void DoSelect(const AnalogOutputInt32& command, uint16_t index) {}
	virtual void DoOperate(const AnalogOutputInt32& command, uint16_t index, OperateType opType) {}

	virtual void DoSelect(const AnalogOutputFloat32& command, uint16_t index) {}
	virtual void DoOperate(const AnalogOutputFloat32& command, uint16_t index, OperateType opType) {}

	virtual void DoSelect(const AnalogOutputDouble64& command, uint16_t index) {}
	virtual void DoOperate(const AnalogOutputDouble64& command, uint16_t index, OperateType opType) {}

	virtual void Start() override;
	virtual void End() override;

	CommandStatus status;

public:

	uint32_t numOperate;
	uint32_t numSelect;
	uint32_t numStart;
	uint32_t numEnd;

};

/**
* A singleton command handler that always returns success
*/
class SuccessCommandHandler : public SimpleCommandHandler
{
public:

	static std::shared_ptr<ICommandHandler> Create()
	{
		return std::make_shared<SuccessCommandHandler>();
	}

	SuccessCommandHandler() : SimpleCommandHandler(CommandStatus::SUCCESS) {}
};

}

#endif

