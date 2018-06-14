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

#include "CommandHandlerAdapter.h"


#include "../jni/JCache.h"

using namespace opendnp3;
using namespace jni;

void CommandHandlerAdapter::Start()
{
	const auto env = JNI::GetEnv();
	JCache::CommandHandler.start(env, proxy);
}

void CommandHandlerAdapter::End()
{
	const auto env = JNI::GetEnv();
	JCache::CommandHandler.end(env, proxy);
}

CommandStatus CommandHandlerAdapter::Select(const ControlRelayOutputBlock& command, uint16_t index)
{
	const auto env = JNI::GetEnv();
	auto jcommand = Convert(env, command);
	auto jstatus = JCache::CommandHandler.selectCROB(env, proxy, jcommand, index);
	return CommandStatusFromType(static_cast<uint8_t>(JCache::CommandStatus.toType(env, jstatus)));
}

CommandStatus CommandHandlerAdapter::Operate(const ControlRelayOutputBlock& command, uint16_t index, OperateType opType)
{
	const auto env = JNI::GetEnv();
	auto jcommand = Convert(env, command);
	auto jopType = JCache::OperateType.fromType(env, static_cast<jint>(opType));
	auto jstatus = JCache::CommandHandler.operateCROB(env, proxy, jcommand, index, jopType);
	return CommandStatusFromType(static_cast<uint8_t>(JCache::CommandStatus.toType(env, jstatus)));
}

CommandStatus CommandHandlerAdapter::Select(const AnalogOutputInt16& command, uint16_t index)
{
	const auto env = JNI::GetEnv();
	auto jcommand = Convert(env, command);
	auto jstatus = JCache::CommandHandler.selectAOI16(env, proxy, jcommand, index);
	return CommandStatusFromType(static_cast<uint8_t>(JCache::CommandStatus.toType(env, jstatus)));
}

CommandStatus CommandHandlerAdapter::Operate(const AnalogOutputInt16& command, uint16_t index, OperateType opType)
{
	const auto env = JNI::GetEnv();
	auto jcommand = Convert(env, command);
	auto jopType = JCache::OperateType.fromType(env, static_cast<jint>(opType));
	auto jstatus = JCache::CommandHandler.operateAOI16(env, proxy, jcommand, index, jopType);
	return CommandStatusFromType(static_cast<uint8_t>(JCache::CommandStatus.toType(env, jstatus)));
}

CommandStatus CommandHandlerAdapter::Select(const AnalogOutputInt32& command, uint16_t index)
{
	const auto env = JNI::GetEnv();
	auto jcommand = Convert(env, command);
	auto jstatus = JCache::CommandHandler.selectAOI32(env, proxy, jcommand, index);
	return CommandStatusFromType(static_cast<uint8_t>(JCache::CommandStatus.toType(env, jstatus)));
}

CommandStatus CommandHandlerAdapter::Operate(const AnalogOutputInt32& command, uint16_t index, OperateType opType)
{
	const auto env = JNI::GetEnv();
	auto jcommand = Convert(env, command);
	auto jopType = JCache::OperateType.fromType(env, static_cast<jint>(opType));
	auto jstatus = JCache::CommandHandler.operateAOI32(env, proxy, jcommand, index, jopType);
	return CommandStatusFromType(static_cast<uint8_t>(JCache::CommandStatus.toType(env, jstatus)));
}

CommandStatus CommandHandlerAdapter::Select(const AnalogOutputFloat32& command, uint16_t index)
{
	const auto env = JNI::GetEnv();
	auto jcommand = Convert(env, command);
	auto jstatus = JCache::CommandHandler.selectAOF32(env, proxy, jcommand, index);
	return CommandStatusFromType(static_cast<uint8_t>(JCache::CommandStatus.toType(env, jstatus)));
}

CommandStatus CommandHandlerAdapter::Operate(const AnalogOutputFloat32& command, uint16_t index, OperateType opType)
{
	const auto env = JNI::GetEnv();
	auto jcommand = Convert(env, command);
	auto jopType = JCache::OperateType.fromType(env, static_cast<jint>(opType));
	auto jstatus = JCache::CommandHandler.operateAOF32(env, proxy, jcommand, index, jopType);
	return CommandStatusFromType(static_cast<uint8_t>(JCache::CommandStatus.toType(env, jstatus)));
}

CommandStatus CommandHandlerAdapter::Select(const AnalogOutputDouble64& command, uint16_t index)
{
	const auto env = JNI::GetEnv();
	auto jcommand = Convert(env, command);
	auto jstatus = JCache::CommandHandler.selectAOD64(env, proxy, jcommand, index);
	return CommandStatusFromType(static_cast<uint8_t>(JCache::CommandStatus.toType(env, jstatus)));
}

CommandStatus CommandHandlerAdapter::Operate(const AnalogOutputDouble64& command, uint16_t index, OperateType opType)
{
	const auto env = JNI::GetEnv();
	auto jcommand = Convert(env, command);
	auto jopType = JCache::OperateType.fromType(env, static_cast<jint>(opType));
	auto jstatus = JCache::CommandHandler.operateAOD64(env, proxy, jcommand, index, jopType);
	return CommandStatusFromType(static_cast<uint8_t>(JCache::CommandStatus.toType(env, jstatus)));
}

LocalRef<jobject> CommandHandlerAdapter::Convert(JNIEnv* env, const ControlRelayOutputBlock& command)
{
	auto jcontrolcode = JCache::ControlCode.fromType(env, command.rawCode);
	auto jcommandstatus = JCache::CommandStatus.fromType(env, CommandStatusToType(command.status));

	return JCache::ControlRelayOutputBlock.init5(env, jcontrolcode, command.count, command.onTimeMS, command.offTimeMS, jcommandstatus);
}

LocalRef<jobject> CommandHandlerAdapter::Convert(JNIEnv* env, const AnalogOutputInt16& command)
{
	auto jcommandstatus = JCache::CommandStatus.fromType(env, CommandStatusToType(command.status));
	return JCache::AnalogOutputInt16.init2(env, command.value, jcommandstatus);
}

LocalRef<jobject> CommandHandlerAdapter::Convert(JNIEnv* env, const AnalogOutputInt32& command)
{
	auto jcommandstatus = JCache::CommandStatus.fromType(env, CommandStatusToType(command.status));
	return JCache::AnalogOutputInt32.init2(env, command.value, jcommandstatus);
}

LocalRef<jobject> CommandHandlerAdapter::Convert(JNIEnv* env, const AnalogOutputFloat32& command)
{
	auto jcommandstatus = JCache::CommandStatus.fromType(env, CommandStatusToType(command.status));
	return JCache::AnalogOutputFloat32.init2(env, command.value, jcommandstatus);
}

LocalRef<jobject> CommandHandlerAdapter::Convert(JNIEnv* env, const AnalogOutputDouble64& command)
{
	auto jcommandstatus = JCache::CommandStatus.fromType(env, CommandStatusToType(command.status));
	return JCache::AnalogOutputDouble64.init2(env, command.value, jcommandstatus);
}

