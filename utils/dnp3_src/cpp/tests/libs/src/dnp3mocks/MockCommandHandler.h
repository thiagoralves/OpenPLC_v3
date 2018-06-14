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
#ifndef OPENDNP3_MOCK_COMMAND_HANDLER_H
#define OPENDNP3_MOCK_COMMAND_HANDLER_H

#include <opendnp3/outstation/SimpleCommandHandler.h>

#include <vector>

namespace opendnp3
{

template <class T>
class Operation : public Indexed<T>
{
public:

	Operation(T value, uint16_t index, OperateType opType_) :
		Indexed<T>(value, index),
		opType(opType_)
	{

	}

	OperateType opType;
};


class MockCommandHandler final : public SimpleCommandHandler
{
public:

	MockCommandHandler(CommandStatus status = CommandStatus::SUCCESS) : SimpleCommandHandler(status)
	{}

	void SetResponse(CommandStatus status_)
	{
		status = status_;
	}

	uint32_t NumInvocations() const
	{
		return numSelect + numOperate;
	}

	uint32_t NumSelect() const
	{
		return numSelect;
	}

	uint32_t NumOperate() const
	{
		return numOperate;
	}

protected:

	virtual void DoOperate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType) override
	{
		this->crobOps.push_back(Operation<ControlRelayOutputBlock>(command, index, opType));
	}

	virtual void DoOperate(const AnalogOutputInt16& command, uint16_t index, OperateType opType) override
	{
		this->aoInt16Ops.push_back(Operation<AnalogOutputInt16>(command, index, opType));
	}

	virtual void DoOperate(const AnalogOutputInt32& command, uint16_t index, OperateType opType) override
	{
		this->aoInt32Ops.push_back(Operation<AnalogOutputInt32>(command, index, opType));
	}

	virtual void DoOperate(const AnalogOutputFloat32& command, uint16_t index, OperateType opType) override
	{
		this->aoFloat32Ops.push_back(Operation<AnalogOutputFloat32>(command, index, opType));
	}

	virtual void DoOperate(const AnalogOutputDouble64& command, uint16_t index, OperateType opType) override
	{
		this->aoDouble64Ops.push_back(Operation<AnalogOutputDouble64>(command, index, opType));
	}

public:

	std::vector<Operation<ControlRelayOutputBlock>> crobOps;
	std::vector<Operation<AnalogOutputInt16>> aoInt16Ops;
	std::vector<Operation<AnalogOutputInt32>> aoInt32Ops;
	std::vector<Operation<AnalogOutputFloat32>> aoFloat32Ops;
	std::vector<Operation<AnalogOutputDouble64>> aoDouble64Ops;

};

}

#endif

