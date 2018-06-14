//
//  _   _         ______    _ _ _   _             _ _ _
// | \ | |       |  ____|  | (_) | (_)           | | | |
// |  \| | ___   | |__   __| |_| |_ _ _ __   __ _| | | |
// | . ` |/ _ \  |  __| / _` | | __| | '_ \ / _` | | | |
// | |\  | (_) | | |___| (_| | | |_| | | | | (_| |_|_|_|
// |_| \_|\___/  |______\__,_|_|\__|_|_| |_|\__, (_|_|_)
//                                           __/ |
//                                          |___/
// 
// This file is auto-generated. Do not edit manually
// 
// Copyright 2016 Automatak LLC
// 
// Automatak LLC (www.automatak.com) licenses this file
// to you under the the Apache License Version 2.0 (the "License"):
// 
// http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "JNIOutstationConfig.h"

namespace jni
{
    namespace cache
    {
        bool OutstationConfig::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/OutstationConfig;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->indexModeField = env->GetFieldID(this->clazz, "indexMode", "Lcom/automatak/dnp3/enums/IndexMode;");
            if(!this->indexModeField) return false;

            this->maxControlsPerRequestField = env->GetFieldID(this->clazz, "maxControlsPerRequest", "S");
            if(!this->maxControlsPerRequestField) return false;

            this->selectTimeoutField = env->GetFieldID(this->clazz, "selectTimeout", "Ljava/time/Duration;");
            if(!this->selectTimeoutField) return false;

            this->solConfirmTimeoutField = env->GetFieldID(this->clazz, "solConfirmTimeout", "Ljava/time/Duration;");
            if(!this->solConfirmTimeoutField) return false;

            this->unsolRetryTimeoutField = env->GetFieldID(this->clazz, "unsolRetryTimeout", "Ljava/time/Duration;");
            if(!this->unsolRetryTimeoutField) return false;

            this->maxTxFragSizeField = env->GetFieldID(this->clazz, "maxTxFragSize", "I");
            if(!this->maxTxFragSizeField) return false;

            this->maxRxFragSizeField = env->GetFieldID(this->clazz, "maxRxFragSize", "I");
            if(!this->maxRxFragSizeField) return false;

            this->allowUnsolicitedField = env->GetFieldID(this->clazz, "allowUnsolicited", "Z");
            if(!this->allowUnsolicitedField) return false;

            return true;
        }

        void OutstationConfig::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        LocalRef<jobject> OutstationConfig::getindexMode(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->indexModeField));
        }

        jshort OutstationConfig::getmaxControlsPerRequest(JNIEnv* env, jobject instance)
        {
            return env->GetShortField(instance, this->maxControlsPerRequestField);
        }

        LocalRef<jobject> OutstationConfig::getselectTimeout(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->selectTimeoutField));
        }

        LocalRef<jobject> OutstationConfig::getsolConfirmTimeout(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->solConfirmTimeoutField));
        }

        LocalRef<jobject> OutstationConfig::getunsolRetryTimeout(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->unsolRetryTimeoutField));
        }

        jint OutstationConfig::getmaxTxFragSize(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->maxTxFragSizeField);
        }

        jint OutstationConfig::getmaxRxFragSize(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->maxRxFragSizeField);
        }

        jboolean OutstationConfig::getallowUnsolicited(JNIEnv* env, jobject instance)
        {
            return env->GetBooleanField(instance, this->allowUnsolicitedField);
        }
    }
}
