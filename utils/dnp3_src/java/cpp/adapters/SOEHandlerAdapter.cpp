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

#include "SOEHandlerAdapter.h"

#include "../jni/JCache.h"

using namespace opendnp3;

void SOEHandlerAdapter::Start()
{

	jni::JCache::SOEHandler.start(JNI::GetEnv(), proxy);
}

void SOEHandlerAdapter::End()
{
	jni::JCache::SOEHandler.end(JNI::GetEnv(), proxy);
}

template <class T, class CreateMeas, class CallProxy>
void SOEHandlerAdapter::Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::Indexed<T>>& values, const CreateMeas& createMeas, const CallProxy& callProxy)
{
	const auto env = JNI::GetEnv();

	auto jinfo = Convert(env, info);

	auto jlist = jni::JCache::ArrayList.init1(env, static_cast<jint>(values.Count()));

	auto add = [&](const Indexed<T>& meas)
	{
		auto jvalue = createMeas(env, meas.value);
		auto jindexed = jni::JCache::IndexedValue.init2(env, jvalue, meas.index);
		jni::JCache::ArrayList.add(env, jlist, jindexed);
	};

	values.ForeachItem(add);

	callProxy(env, proxy, jinfo, jlist);
}

void SOEHandlerAdapter::Process(const HeaderInfo& info, const ICollection<Indexed<Binary>>& values)
{
	auto create = [](JNIEnv * env, const Binary & value) -> LocalRef<jobject> { return jni::JCache::BinaryInput.init3(env, value.value, value.flags.value, value.time); };

	auto call = [](JNIEnv * env, jobject proxy, jobject hinfo, jobject list)
	{
		jni::JCache::SOEHandler.processBI(env, proxy, hinfo, list);
	};

	this->Process(info, values, create, call);
}

void SOEHandlerAdapter::Process(const HeaderInfo& info, const ICollection<Indexed<DoubleBitBinary>>& values)
{
	auto create = [](JNIEnv * env, const DoubleBitBinary & value) -> LocalRef<jobject>
	{
		auto evalue = jni::JCache::DoubleBit.fromType(env, DoubleBitToType(value.value));
		return jni::JCache::DoubleBitBinaryInput.init3(env, evalue, value.flags.value, value.time);
	};
	auto call = [](JNIEnv * env, jobject proxy, jobject hinfo, jobject list)
	{
		jni::JCache::SOEHandler.processDBI(env, proxy, hinfo, list);
	};

	this->Process(info, values, create, call);
}

void SOEHandlerAdapter::Process(const HeaderInfo& info, const ICollection<Indexed<Analog>>& values)
{
	auto create = [](JNIEnv * env, const Analog & value) -> LocalRef<jobject> { return jni::JCache::AnalogInput.init3(env, value.value, value.flags.value, value.time); };
	auto call = [](JNIEnv * env, jobject proxy, jobject hinfo, jobject list)
	{
		jni::JCache::SOEHandler.processAI(env, proxy, hinfo, list);
	};

	this->Process(info, values, create, call);
}

void SOEHandlerAdapter::Process(const HeaderInfo& info, const ICollection<Indexed<Counter>>& values)
{
	auto create = [](JNIEnv * env, const Counter & value) -> LocalRef<jobject> { return jni::JCache::Counter.init3(env, value.value, value.flags.value, value.time); };
	auto call = [](JNIEnv * env, jobject proxy, jobject hinfo, jobject list)
	{
		jni::JCache::SOEHandler.processC(env, proxy, hinfo, list);
	};

	this->Process(info, values, create, call);
}

void SOEHandlerAdapter::Process(const HeaderInfo& info, const ICollection<Indexed<FrozenCounter>>& values)
{
	auto create = [](JNIEnv * env, const FrozenCounter & value) -> LocalRef<jobject> { return jni::JCache::FrozenCounter.init3(env, value.value, value.flags.value, value.time); };
	auto call = [](JNIEnv * env, jobject proxy, jobject hinfo, jobject list)
	{
		jni::JCache::SOEHandler.processFC(env, proxy, hinfo, list);
	};

	this->Process(info, values, create, call);
}

void SOEHandlerAdapter::Process(const HeaderInfo& info, const ICollection<Indexed<BinaryOutputStatus>>& values)
{
	auto create = [](JNIEnv * env, const BinaryOutputStatus & value) -> LocalRef<jobject> { return jni::JCache::BinaryOutputStatus.init3(env, value.value, value.flags.value, value.time); };
	auto call = [](JNIEnv * env, jobject proxy, jobject hinfo, jobject list)
	{
		jni::JCache::SOEHandler.processBOS(env, proxy, hinfo, list);
	};

	this->Process(info, values, create, call);
}

void SOEHandlerAdapter::Process(const HeaderInfo& info, const ICollection<Indexed<AnalogOutputStatus>>& values)
{
	auto create = [](JNIEnv * env, const AnalogOutputStatus & value) -> LocalRef<jobject> { return jni::JCache::AnalogOutputStatus.init3(env, value.value, value.flags.value, value.time); };
	auto call = [](JNIEnv * env, jobject proxy, jobject hinfo, jobject list)
	{
		jni::JCache::SOEHandler.processAOS(env, proxy, hinfo, list);
	};

	this->Process(info, values, create, call);
}

void SOEHandlerAdapter::Process(const opendnp3::HeaderInfo& info, const opendnp3::ICollection<opendnp3::DNPTime>& values)
{
	const auto env = JNI::GetEnv();

	auto jinfo = Convert(env, info);

	auto jlist = jni::JCache::ArrayList.init1(env, static_cast<jint>(values.Count()));

	auto add = [&](const DNPTime & value)
	{
		auto jvalue = jni::JCache::DNPTime.init1(env, value.value);
		jni::JCache::ArrayList.add(env, jlist, jvalue);
	};

	values.ForeachItem(add);

	jni::JCache::SOEHandler.processDNPTime(env, proxy, jinfo, jlist);
}

LocalRef<jobject> SOEHandlerAdapter::Convert(JNIEnv* env, const opendnp3::HeaderInfo& info)
{
	auto gv = jni::JCache::GroupVariation.fromType(env, GroupVariationToType(info.gv));
	auto qc = jni::JCache::QualifierCode.fromType(env, QualifierCodeToType(info.qualifier));
	auto tsmode = jni::JCache::TimestampMode.fromType(env, static_cast<jint>(info.tsmode));
	jboolean isEvent = info.isEventVariation;
	jboolean flagsValid = info.flagsValid;
	jint headerIndex = info.headerIndex;

	return jni::JCache::HeaderInfo.init6(env, gv, qc, tsmode, isEvent, flagsValid, headerIndex);
}

