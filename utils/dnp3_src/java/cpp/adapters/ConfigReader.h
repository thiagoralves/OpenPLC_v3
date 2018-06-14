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
#ifndef OPENDNP3_CONFIG_READER_H
#define OPENDNP3_CONFIG_READER_H

#include <jni.h>
#include <string>

#include "asiodnp3/MasterStackConfig.h"
#include "asiodnp3/OutstationStackConfig.h"

class ConfigReader
{
public:

	static asiodnp3::MasterStackConfig ConvertMasterStackConfig(JNIEnv* env, jobject jconfig);
	static asiodnp3::OutstationStackConfig ConvertOutstationStackConfig(JNIEnv* env, jobject jconfig);

private:

	static opendnp3::LinkConfig ConvertLinkConfig(JNIEnv* env, jobject jconfig);
	static opendnp3::MasterParams ConvertMasterConfig(JNIEnv* apEnv, jobject jcfg);

	static opendnp3::ClassField ConvertClassField(JNIEnv* env, jobject jclassmask);
	static opendnp3::EventBufferConfig ConvertEventBufferConfig(JNIEnv* env, jobject jeventconfig);
	static opendnp3::OutstationParams ConvertOutstationConfig(JNIEnv* env, jobject jconfig);
	static openpal::TimeDuration ConvertDuration(JNIEnv* env, jobject jduration);
	static void ConvertDatabase(JNIEnv* env, jobject jdb, asiodnp3::DatabaseConfig& cfg);

	static opendnp3::BinaryConfig ConvertBinaryConfig(JNIEnv* env, jobject jconfig);
	static opendnp3::DoubleBitBinaryConfig ConvertDoubleBinaryConfig(JNIEnv* env, jobject jconfig);
	static opendnp3::AnalogConfig ConvertAnalogConfig(JNIEnv* env, jobject jconfig);
	static opendnp3::CounterConfig ConvertCounterConfig(JNIEnv* env, jobject jconfig);
	static opendnp3::FrozenCounterConfig ConvertFrozenCounterConfig(JNIEnv* env, jobject jconfig);
	static opendnp3::BOStatusConfig ConvertBOStatusConfig(JNIEnv* env, jobject jconfig);
	static opendnp3::AOStatusConfig ConvertAOStatusConfig(JNIEnv* env, jobject jconfig);
};

#endif


