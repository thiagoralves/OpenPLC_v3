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

#ifndef OPENDNP3JAVA_LOCALREF_H
#define OPENDNP3JAVA_LOCALREF_H

#include <jni.h>
#include <assert.h>

// RAII class for automatically disposing of local refs
template <class T>
class LocalRef
{
	JNIEnv* const env;
	T ref;

	LocalRef(LocalRef&) = delete;

	LocalRef& operator=(LocalRef&) = delete;

public:

	LocalRef(JNIEnv* env, T ref) : env(env), ref(ref)
	{

	}

	~LocalRef()
	{
		if (ref != nullptr)
		{
			env->DeleteLocalRef(ref);
		}
	}

	LocalRef(LocalRef&& other) : env(other.env), ref(other.ref)
	{
		other.ref = nullptr;
	}

	inline operator const T& () const
	{
		return ref;
	}

};

class LocalJString : public LocalRef<jstring>
{

public:

	LocalJString(JNIEnv* env, const char* cstring) : LocalRef<jstring>(env, env->NewStringUTF(cstring))
	{

	}

};

#endif

