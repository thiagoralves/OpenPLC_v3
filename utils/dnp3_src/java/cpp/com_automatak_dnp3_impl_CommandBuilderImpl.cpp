/*
 * Copyright 2013-2016 Automatak, LLC
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
#include "com_automatak_dnp3_impl_CommandBuilderImpl.h"

#include "opendnp3/master/CommandSet.h"

#include "jni/JCache.h"
#include "adapters/JNI.h"

using namespace opendnp3;

JNIEXPORT jlong JNICALL Java_com_automatak_dnp3_impl_CommandBuilderImpl_create_1native
(JNIEnv* env, jobject)
{
	return (jlong) new CommandSet();
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_CommandBuilderImpl_destroy_1native
  (JNIEnv* env, jobject, jlong native)
{
	delete (CommandSet*)native;
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_CommandBuilderImpl_add_1crob_1native
(JNIEnv* env, jobject, jlong native, jobject jcommands)
{
	const auto set = (CommandSet*) native;
	
	std::vector<Indexed<ControlRelayOutputBlock>> commands;
	auto process = [&](LocalRef<jobject> indexed) 
	{		
		const auto jindex = jni::JCache::IndexedValue.getindex(env, indexed);
		const auto jcommand = jni::JCache::IndexedValue.getvalue(env, indexed);

		auto& ref = jni::JCache::ControlRelayOutputBlock;
		const auto code = jni::JCache::ControlCode.toType(env, ref.getfunction(env, jcommand));
		const auto count = ref.getcount(env, jcommand);
		const auto onTime = ref.getonTimeMs(env, jcommand);
		const auto offTime = ref.getoffTimeMs(env, jcommand);
		const auto status = jni::JCache::CommandStatus.toType(env, ref.getstatus(env, jcommand));

		Indexed<ControlRelayOutputBlock> value(
			ControlRelayOutputBlock(
				static_cast<uint8_t>(code),
				static_cast<uint8_t>(count),
				static_cast<uint32_t>(onTime),
				static_cast<uint32_t>(offTime),
				CommandStatusFromType(static_cast<uint8_t>(status))
			),
			static_cast<uint16_t>(jindex)
		);

		commands.push_back(value);		
	};

	JNI::Iterate(env, jcommands, process);	
	set->Add(commands);
}

template <class T, class Cache>
void process_analogs(JNIEnv* env, jlong native, jobject jcommands, Cache& cache)
{
	const auto set = (CommandSet*)native;
	
	std::vector<Indexed<T>> commands;
	auto process = [&](jobject indexed)
	{
		const auto jindex = jni::JCache::IndexedValue.getindex(env, indexed);
		const auto jcommand = jni::JCache::IndexedValue.getvalue(env, indexed);
		
		const auto avalue = cache.getvalue(env, jcommand);
		const auto status = jni::JCache::CommandStatus.toType(env, cache.getstatus(env, jcommand));

		Indexed<T> value(
			T(avalue, CommandStatusFromType(static_cast<uint8_t>(status))),
			static_cast<uint16_t>(jindex)
		);

		commands.push_back(value);
	};

	JNI::Iterate(env, jcommands, process);
	set->Add(commands);
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_CommandBuilderImpl_add_1aoI16_1native
(JNIEnv* env, jobject, jlong native, jobject jcommands)
{
	process_analogs<AnalogOutputInt16>(env, native, jcommands, jni::JCache::AnalogOutputInt16);
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_CommandBuilderImpl_add_1aoI32_1native
(JNIEnv* env, jobject, jlong native, jobject jcommands)
{
	process_analogs<AnalogOutputInt32>(env, native, jcommands, jni::JCache::AnalogOutputInt32);
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_CommandBuilderImpl_add_1aoF32_1native
(JNIEnv* env, jobject, jlong native, jobject jcommands)
{
	process_analogs<AnalogOutputFloat32>(env, native, jcommands, jni::JCache::AnalogOutputFloat32);
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_CommandBuilderImpl_add_1aoD64_1native
(JNIEnv* env, jobject, jlong native, jobject jcommands)
{
	process_analogs<AnalogOutputDouble64>(env, native, jcommands, jni::JCache::AnalogOutputDouble64);
}

