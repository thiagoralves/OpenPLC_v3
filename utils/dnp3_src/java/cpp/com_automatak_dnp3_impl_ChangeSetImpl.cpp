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
#include "com_automatak_dnp3_impl_ChangeSetImpl.h"

#include "asiodnp3/UpdateBuilder.h"

using namespace opendnp3;
using namespace asiodnp3;

JNIEXPORT jlong JNICALL Java_com_automatak_dnp3_impl_ChangeSetImpl_create_1instance_1native
(JNIEnv* env, jobject)
{
	return (jlong) new UpdateBuilder();
}


JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_ChangeSetImpl_destroy_1instance_1native
(JNIEnv* env, jobject, jlong native)
{
	delete (UpdateBuilder*) native;
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_ChangeSetImpl_update_1binary_1native
(JNIEnv* env, jobject, jlong native, jboolean value, jbyte flags, jlong time, jint index, jint mode)
{
	const auto changes = (UpdateBuilder*) native;
	changes->Update(Binary(!!value, flags, DNPTime(time)), static_cast<uint16_t>(index), static_cast<EventMode>(mode));
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_ChangeSetImpl_update_1double_1binary_1native
(JNIEnv* env, jobject, jlong native, jint value, jbyte flags, jlong time, jint index, jint mode)
{
	const auto changes = (UpdateBuilder*) native;
	changes->Update(DoubleBitBinary(static_cast<DoubleBit>(value), flags, DNPTime(time)), static_cast<uint16_t>(index), static_cast<EventMode>(mode));
}


JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_ChangeSetImpl_update_1analog_1native
(JNIEnv* env, jobject, jlong native, jdouble value, jbyte flags, jlong time, jint index, jint mode)
{
	const auto changes = (UpdateBuilder*) native;
	changes->Update(Analog(value, flags, DNPTime(time)), static_cast<uint16_t>(index), static_cast<EventMode>(mode));
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_ChangeSetImpl_update_1counter_1native
(JNIEnv* env, jobject, jlong native, jlong value, jbyte flags, jlong time, jint index, jint mode)
{
	const auto changes = (UpdateBuilder*) native;
	changes->Update(Counter(static_cast<uint32_t>(value), flags, DNPTime(time)), static_cast<uint16_t>(index), static_cast<EventMode>(mode));
}

JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_ChangeSetImpl_update_1frozen_1counter_1native
(JNIEnv* env, jobject, jlong native, jlong value, jbyte flags, jlong time, jint index, jint mode)
{
	const auto changes = (UpdateBuilder*) native;
	changes->Update(FrozenCounter(static_cast<uint32_t>(value), flags, DNPTime(time)), static_cast<uint16_t>(index), static_cast<EventMode>(mode));
}


JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_ChangeSetImpl_update_1bo_1status_1native
(JNIEnv* env, jobject, jlong native, jboolean value, jbyte flags, jlong time, jint index, jint mode)
{
	const auto changes = (UpdateBuilder*) native;
	changes->Update(BinaryOutputStatus(!!value, flags, DNPTime(time)), static_cast<uint16_t>(index), static_cast<EventMode>(mode));
}


JNIEXPORT void JNICALL Java_com_automatak_dnp3_impl_ChangeSetImpl_update_1ao_1status_1native
 (JNIEnv* env, jobject, jlong native, jdouble value, jbyte flags, jlong time, jint index, jint mode)
{
	const auto changes = (UpdateBuilder*) native;
	changes->Update(AnalogOutputStatus(value, flags, DNPTime(time)), static_cast<uint16_t>(index), static_cast<EventMode>(mode));
}

