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
#include "com_automatak_dnp3_impl_MasterImpl.h"

#include "jni/JCache.h"
#include "adapters/GlobalRef.h"
#include "adapters/Conversions.h"


#include "asiodnp3/IMaster.h"
#include "opendnp3/master/CommandSet.h"

#include <memory>

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_MasterImpl_set_1log_1level_1native
(JNIEnv* env, jobject, jlong native, jint levels)
{
	const auto master = (std::shared_ptr<asiodnp3::IMaster>*) native;
	(*master)->SetLogFilters(levels);
}

JNIEXPORT jobject JNICALL Java_com_automatak_dnp3_impl_MasterImpl_get_1statistics_1native
(JNIEnv* env, jobject, jlong native)
{
	const auto master = (std::shared_ptr<asiodnp3::IMaster>*) native;
	auto stats = (*master)->GetStackStatistics();
	return Conversions::ConvertStackStatistics(env, stats);
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_MasterImpl_enable_1native
(JNIEnv* env, jobject, jlong native)
{
	const auto master = (std::shared_ptr<asiodnp3::IMaster>*) native;
	(*master)->Enable();
}


JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_MasterImpl_disable_1native
(JNIEnv* env, jobject, jlong native)
{
	const auto master = (std::shared_ptr<asiodnp3::IMaster>*) native;
	(*master)->Disable();
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_MasterImpl_shutdown_1native
(JNIEnv* env, jobject, jlong native)
{
	const auto master = (std::shared_ptr<asiodnp3::IMaster>*) native;
	(*master)->Shutdown();
}


JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_MasterImpl_destroy_1native
(JNIEnv *, jobject, jlong native)
{
	const auto master = (std::shared_ptr<asiodnp3::IMaster>*) native;	
	delete master;
}

template <class Fun>
void operate(JNIEnv* env, jlong native, jlong nativeCommandSet, jobject future, const Fun& operate)
{
	auto& set = *(opendnp3::CommandSet*) nativeCommandSet;

	auto sharedf = std::make_shared<GlobalRef>(future);
	auto callback = [sharedf](const opendnp3::ICommandTaskResult& result)
	{
		const auto env = JNI::GetEnv();
		const auto jsummary = jni::JCache::TaskCompletion.fromType(env, static_cast<jint>(result.summary));
		const auto jlist = jni::JCache::ArrayList.init1(env, static_cast<jint>(result.Count()));

		auto addToJList = [&](const opendnp3::CommandPointResult& cpr) {
			const auto jstate = jni::JCache::CommandPointState.fromType(env, static_cast<jint>(cpr.state));
			const auto jstatus = jni::JCache::CommandStatus.fromType(env, static_cast<jint>(cpr.status));
			const auto jres = jni::JCache::CommandPointResult.init4(env, cpr.headerIndex, cpr.index, jstate, jstatus);
			jni::JCache::ArrayList.add(env, jlist, jres);
		};

		result.ForeachItem(addToJList);

		const auto jtaskresult = jni::JCache::CommandTaskResult.init2(env, jsummary, jlist);

		jni::JCache::CompletableFuture.complete(env, *sharedf, jtaskresult); // invoke the future		
	};

	const auto master = (std::shared_ptr<asiodnp3::IMaster>*) native;
	operate(**master, set, callback);	
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_MasterImpl_select_1and_1operate_1native
(JNIEnv* env, jobject, jlong native, jlong nativeCommandSet, jobject future)
{
	auto sbo = [](asiodnp3::IMaster& master, opendnp3::CommandSet& commandset, const opendnp3::CommandCallbackT& callback) -> void 
	{
		master.SelectAndOperate(std::move(commandset), callback);
	};

	operate(env, native, nativeCommandSet, future, sbo);
}


JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_MasterImpl_direct_1operate_1native
(JNIEnv*  env, jobject, jlong native, jlong nativeCommandSet, jobject future)
{
	auto directOp = [](asiodnp3::IMaster& master, opendnp3::CommandSet& commandset, const opendnp3::CommandCallbackT& callback) -> void
	{
		master.DirectOperate(std::move(commandset), callback);
	};

	operate(env, native, nativeCommandSet, future, directOp);
}

bool ConvertJHeader(JNIEnv* env, jobject jheader, opendnp3::Header& header)
{
	const auto group = jni::JCache::Header.getgroup(env, jheader);
	const auto var = jni::JCache::Header.getvariation(env, jheader);
	const auto qualifier = opendnp3::QualifierCodeFromType(static_cast<uint8_t>(jni::JCache::QualifierCode.toType(env, jni::JCache::Header.getqualifier(env, jheader))));
	const auto count = jni::JCache::Header.getcount(env, jheader);
	const auto start = jni::JCache::Header.getstart(env, jheader);
	const auto stop = jni::JCache::Header.getstop(env, jheader);

	switch (qualifier)
	{
	case(opendnp3::QualifierCode::ALL_OBJECTS):
		header = opendnp3::Header::AllObjects(group, var);		
		return true;
	case(opendnp3::QualifierCode::UINT8_CNT):
		header = opendnp3::Header::Count8(group, var, static_cast<uint8_t>(count));
		return true;
	case(opendnp3::QualifierCode::UINT16_CNT):
		header = opendnp3::Header::Count16(group, var, static_cast<uint16_t>(count));
		return true;
	case(opendnp3::QualifierCode::UINT8_START_STOP):
		header = opendnp3::Header::Range8(group, var, static_cast<uint8_t>(start), static_cast<uint8_t>(stop));
		return true;
	case(opendnp3::QualifierCode::UINT16_START_STOP):
		header = opendnp3::Header::Range16(group, var, static_cast<uint16_t>(start), static_cast<uint16_t>(stop));
		return true;
	default:
		return false;
	}
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_MasterImpl_scan_1native
(JNIEnv* env, jobject, jlong native, jobject jheaders)
{
	const auto master = (std::shared_ptr<asiodnp3::IMaster>*) native;

	std::vector<opendnp3::Header> headers;

	auto process = [&](LocalRef<jobject> jheader) {
		opendnp3::Header header;
		if (ConvertJHeader(env, jheader, header))
		{
			headers.push_back(header);
		}
	};

	JNI::Iterate(env, jheaders, process);

	(*master)->Scan(headers);
}


JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_MasterImpl_add_1periodic_1scan_1native
(JNIEnv* env, jobject, jlong native, jobject jduration, jobject jheaders)
{
	const auto master = (std::shared_ptr<asiodnp3::IMaster>*) native;

	std::vector<opendnp3::Header> headers;

	auto process = [&](LocalRef<jobject> jheader) {
		opendnp3::Header header;
		if (ConvertJHeader(env, jheader, header))
		{
			headers.push_back(header);
		}
	};

	JNI::Iterate(env, jheaders, process);

	auto period = openpal::TimeDuration::Milliseconds(jni::JCache::Duration.toMillis(env, jduration));

	(*master)->AddScan(period, headers);
}