/**
 * Copyright 2013 Automatak, LLC
 *
 * Licensed to Automatak, LLC (www.automatak.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership. Automatak, LLC
 * licenses this file to you under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#ifndef OPENDNP3_COMMANDHANDLERADAPTER_H
#define OPENDNP3_COMMANDHANDLERADAPTER_H

#include <opendnp3/outstation/ICommandHandler.h>

#include "GlobalRef.h"
#include "LocalRef.h"

class CommandHandlerAdapter : public opendnp3::ICommandHandler
{
public:

	CommandHandlerAdapter(jobject proxy) : proxy(proxy) {}

	virtual void Start() override;
	virtual void End() override;

	virtual opendnp3::CommandStatus Select(const opendnp3::ControlRelayOutputBlock& command, uint16_t index) override;

	virtual opendnp3::CommandStatus Operate(const opendnp3::ControlRelayOutputBlock& command, uint16_t index, opendnp3::OperateType opType) override;

	virtual opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt16& command, uint16_t index) override;

	virtual opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt16& command, uint16_t index, opendnp3::OperateType opType) override;

	virtual opendnp3::CommandStatus Select(const opendnp3::AnalogOutputInt32& command, uint16_t index) override;

	virtual opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputInt32& command, uint16_t index, opendnp3::OperateType opType) override;

	virtual opendnp3::CommandStatus Select(const opendnp3::AnalogOutputFloat32& command, uint16_t index) override;

	virtual opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputFloat32& command, uint16_t index, opendnp3::OperateType opType) override;

	virtual opendnp3::CommandStatus Select(const opendnp3::AnalogOutputDouble64& command, uint16_t index) override;

	virtual opendnp3::CommandStatus Operate(const opendnp3::AnalogOutputDouble64& command, uint16_t index, opendnp3::OperateType opType) override;


private:

	static LocalRef<jobject> Convert(JNIEnv* env, const opendnp3::ControlRelayOutputBlock& command);
	static LocalRef<jobject> Convert(JNIEnv* env, const opendnp3::AnalogOutputInt16& command);
	static LocalRef<jobject> Convert(JNIEnv* env, const opendnp3::AnalogOutputInt32& command);
	static LocalRef<jobject> Convert(JNIEnv* env, const opendnp3::AnalogOutputFloat32& command);
	static LocalRef<jobject> Convert(JNIEnv* env, const opendnp3::AnalogOutputDouble64& command);

	GlobalRef proxy;

};

#endif
