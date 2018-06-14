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
#ifndef OPENDNP3_CHANNELLISTENERADAPTER_H
#define OPENDNP3_CHANNELLISTENERADAPTER_H

#include "asiodnp3/IChannelListener.h"

#include "GlobalRef.h"

#include "../jni/JCache.h"

class ChannelListenerAdapter : public asiodnp3::IChannelListener
{
public:

	ChannelListenerAdapter(jobject proxy) : proxy(proxy) {}

	virtual void OnStateChange(opendnp3::ChannelState state) override
	{
		const auto env = JNI::GetEnv();
		auto jstate = jni::JCache::ChannelState.fromType(env, static_cast<jint>(state));
		jni::JCache::ChannelListener.onStateChange(env, proxy, jstate);
	}

private:

	GlobalRef proxy;

};

#endif
