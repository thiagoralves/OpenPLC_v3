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
#include "com_automatak_dnp3_impl_ManagerImpl.h"

#include <asiodnp3/DNP3Manager.h>

#include "adapters/CString.h"
#include "adapters/LogHandlerAdapter.h"
#include "adapters/ChannelListenerAdapter.h"

#include "jni/JCache.h"

using namespace openpal;
using namespace asiopal;
using namespace asiodnp3;
using namespace opendnp3;

asiopal::TLSConfig ConvertTLSConfig(JNIEnv* env, jobject jconfig)
{
	auto& ref = jni::JCache::TLSConfig;

	CString peerCertFilePath(env, ref.getpeerCertFilePath(env, jconfig));
	CString localCertFilePath(env, ref.getlocalCertFilePath(env, jconfig));
	CString privateKeyFilePath(env, ref.getprivateKeyFilePath(env, jconfig));
	CString cipherList(env, ref.getcipherList(env, jconfig));

	return asiopal::TLSConfig(
		peerCertFilePath.str(),
		localCertFilePath.str(),
		privateKeyFilePath.str(),
		ref.getmaxVerifyDepth(env, jconfig),
		!!ref.getallowTLSv10(env, jconfig),
		!!ref.getallowTLSv11(env, jconfig),
		!!ref.getallowTLSv12(env, jconfig),
		cipherList.str()
	);
}

JNIEXPORT jlong JNICALL Java_com_automatak_dnp3_impl_ManagerImpl_create_1native_1manager
(JNIEnv* env, jobject, jint concurreny, jobject loghandler)
{			
	auto attach = []() { JNI::AttachCurrentThread(); };
	auto detach = []() { JNI::DetachCurrentThread(); };

	auto adapter = std::make_shared<LogHandlerAdapter>(loghandler);	

	return (jlong) new DNP3Manager(concurreny, adapter, attach, detach);	
}


JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_ManagerImpl_shutdown_1native_1manager
(JNIEnv*, jobject, jlong pointer)
{
	delete (DNP3Manager*) pointer;
}

JNIEXPORT jlong JNICALL Java_com_automatak_dnp3_impl_ManagerImpl_get_1native_1channel_1tcp_1client
(JNIEnv* env, jobject, jlong native, jstring jid, jint jlevels, jlong jminRetry, jlong jmaxRetry, jstring jhost, jstring jadapter, jint jport, jobject jlistener)
{
	const auto manager = (DNP3Manager*) native;

	CString id(env, jid);
	CString host(env, jhost);
	CString adapter(env, jadapter);
	ChannelRetry retry(TimeDuration::Milliseconds(jminRetry), TimeDuration::Milliseconds(jmaxRetry));

	auto listener = jlistener ? std::make_shared<ChannelListenerAdapter>(jlistener) : nullptr;

	auto channel = manager->AddTCPClient(id.str(), jlevels, retry, host.str(), adapter.str(), static_cast<uint16_t>(jport), listener);

	return (jlong) new std::shared_ptr<IChannel>(channel);
}

JNIEXPORT jlong JNICALL Java_com_automatak_dnp3_impl_ManagerImpl_get_1native_1channel_1tcp_1server
(JNIEnv* env, jobject, jlong native, jstring jid, jint jlevels, jlong jminRetry, jlong jmaxRetry, jstring jadapter, jint jport, jobject jlistener)
{
	const auto manager = (DNP3Manager*) native;

	CString id(env, jid);	
	CString adapter(env, jadapter);
	ChannelRetry retry(TimeDuration::Milliseconds(jminRetry), TimeDuration::Milliseconds(jmaxRetry));

	auto listener = jlistener ? std::make_shared<ChannelListenerAdapter>(jlistener) : nullptr;

	auto channel = manager->AddTCPServer(id.str(), jlevels, retry, adapter.str(), static_cast<uint16_t>(jport), listener);

	return (jlong) new std::shared_ptr<IChannel>(channel);
}

JNIEXPORT jlong JNICALL Java_com_automatak_dnp3_impl_ManagerImpl_get_1native_1channel_1tls_1client
(JNIEnv* env, jobject, jlong native, jstring jid, jint jlevels, jlong jminRetry, jlong jmaxRetry, jstring jhost, jstring jadapter, jint jport, jobject jtlsconfig, jobject jlistener)
{
	const auto manager = (DNP3Manager*)native;

	CString id(env, jid);
	CString host(env, jhost);
	CString adapter(env, jadapter);
	ChannelRetry retry(TimeDuration::Milliseconds(jminRetry), TimeDuration::Milliseconds(jmaxRetry));

	auto tlsconf = ConvertTLSConfig(env, jtlsconfig);

	auto listener = jlistener ? std::make_shared<ChannelListenerAdapter>(jlistener) : nullptr;

	std::error_code ec;

	auto channel = manager->AddTLSClient(id.str(), jlevels, retry, host.str(), adapter.str(), static_cast<uint16_t>(jport), tlsconf, listener, ec);

	return (jlong) new std::shared_ptr<IChannel>(channel);
}

JNIEXPORT jlong JNICALL Java_com_automatak_dnp3_impl_ManagerImpl_get_1native_1channel_1tls_1server
(JNIEnv* env, jobject, jlong native, jstring jid, jint jlevels, jlong jminRetry, jlong jmaxRetry, jstring jadapter, jint jport, jobject jtlsconfig, jobject jlistener)
{
	const auto manager = (DNP3Manager*)native;

	CString id(env, jid);
	CString adapter(env, jadapter);
	ChannelRetry retry(TimeDuration::Milliseconds(jminRetry), TimeDuration::Milliseconds(jmaxRetry));

	auto tlsconf = ConvertTLSConfig(env, jtlsconfig);

	auto listener = jlistener ? std::make_shared<ChannelListenerAdapter>(jlistener) : nullptr;

	std::error_code ec;

	auto channel = manager->AddTLSServer(id.str(), jlevels, retry, adapter.str(), static_cast<uint16_t>(jport), tlsconf, listener, ec);

	return ec ? 0 : (jlong) new std::shared_ptr<IChannel>(channel);
}

JNIEXPORT jlong JNICALL Java_com_automatak_dnp3_impl_ManagerImpl_get_1native_1channel_1serial
(JNIEnv* env, jobject, jlong native, jstring jid, jint jlevels, jlong jminRetry, jlong jmaxRetry, jstring jsdevice, jint jbaudRate, jint jdatabits, jint jparity, jint jstopbits, jint jflowcontrol, jobject jlistener)
{
	const auto manager = (DNP3Manager*)native;

	CString id(env, jid);
	ChannelRetry retry(TimeDuration::Milliseconds(jminRetry), TimeDuration::Milliseconds(jmaxRetry));
	CString sdevice(env, jsdevice);

	asiopal::SerialSettings settings;
	settings.asyncOpenDelay = openpal::TimeDuration::Milliseconds(100);
	settings.baud = jbaudRate;
	settings.dataBits = jdatabits;
	settings.deviceName = sdevice;
	settings.flowType = opendnp3::FlowControlFromType(static_cast<uint8_t>(jflowcontrol));
	settings.parity = opendnp3::ParityFromType(static_cast<uint8_t>(jparity));
	settings.stopBits = opendnp3::StopBitsFromType(static_cast<uint8_t>(jstopbits));

	auto listener = jlistener ? std::make_shared<ChannelListenerAdapter>(jlistener) : nullptr;

	auto channel = manager->AddSerial(id.str(), jlevels, retry, settings, listener);

	return (jlong) new std::shared_ptr<IChannel>(channel);
}




