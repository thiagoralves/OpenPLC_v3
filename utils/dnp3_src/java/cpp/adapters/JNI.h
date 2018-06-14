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

#ifndef OPENDNP3_JNI_H
#define OPENDNP3_JNI_H

#include <jni.h>
#include <string>
#include <functional>
#include <openpal/util/Uncopyable.h>

#include "LocalRef.h"

#define OPENDNP3_JNI_VERSION JNI_VERSION_1_8

class JNI : private openpal::StaticOnly
{

public:

	// called once during JNI_OnLoad
	static void Initialize(JavaVM* vm);

	static JNIEnv* GetEnv();

	static bool AttachCurrentThread();
	static bool DetachCurrentThread();

	static jobject CreateGlobalRef(jobject ref);
	static void DeleteGlobalRef(jobject ref);

	static void Iterate(JNIEnv* env, jobject iterable, const std::function<void(LocalRef<jobject>)>& callback);

	static void IterateWithIndex(JNIEnv* env, jobject iterable, const std::function<void(LocalRef<jobject>, int)>& callback);


private:

	static JavaVM* vm;
};

#endif

