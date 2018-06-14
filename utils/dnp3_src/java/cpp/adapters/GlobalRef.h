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

#ifndef OPENDNP3_GLOBALREF_H
#define OPENDNP3_GLOBALREF_H

#include <jni.h>

#include <openpal/util/Uncopyable.h>

#include "JNI.h"

// RAII class for JNI global refs
class GlobalRef : private openpal::Uncopyable
{
	jobject reference;

public:

	GlobalRef(jobject reference) : reference(JNI::CreateGlobalRef(reference))
	{}

	~GlobalRef()
	{
		JNI::DeleteGlobalRef(reference);
	}

	operator const jobject& () const
	{
		return reference;
	}
	/*
		operator jobject& ()
		{
			return reference;
		}
	*/

};

#endif

