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

#include "JNI.h"

#include "../jni/JCache.h"

#include <assert.h>

using namespace std;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNI::Initialize(vm);
	return jni::JCache::init(JNI::GetEnv()) ? OPENDNP3_JNI_VERSION : JNI_ERR;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
	jni::JCache::cleanup(JNI::GetEnv());
}

JNIEnv* JNI::GetEnv()
{
	JNIEnv* env = nullptr;
	vm->GetEnv((void**)&env, OPENDNP3_JNI_VERSION);
	assert(env);
	return env;
}

void JNI::Initialize(JavaVM* vmin)
{
	assert(vmin);
	JNI::vm = vmin;
}

bool JNI::AttachCurrentThread()
{
	JNIEnv* env;
	return vm->AttachCurrentThread((void**)&env, nullptr) == 0;
}

bool JNI::DetachCurrentThread()
{
	return vm->DetachCurrentThread() == 0;
}

// initialize static objects
JavaVM* JNI::vm(nullptr);

jobject JNI::CreateGlobalRef(jobject ref)
{
	return GetEnv()->NewGlobalRef(ref);
}

void JNI::DeleteGlobalRef(jobject ref)
{
	GetEnv()->DeleteGlobalRef(ref);
}

void JNI::Iterate(JNIEnv* env, jobject iterable, const std::function<void(LocalRef<jobject>)>& callback)
{
	auto iterator = jni::JCache::Iterable.iterator(env, iterable);

	while (jni::JCache::Iterator.hasNext(env, iterator))
	{
		callback(jni::JCache::Iterator.next(env, iterator));
	}
}

void JNI::IterateWithIndex(JNIEnv* env, jobject iterable, const std::function<void(LocalRef<jobject>, int)>& callback)
{
	auto iterator = jni::JCache::Iterable.iterator(env, iterable);

	int i = 0;
	while (jni::JCache::Iterator.hasNext(env, iterator))
	{
		callback(jni::JCache::Iterator.next(env, iterator), i);
		++i;
	}
}
