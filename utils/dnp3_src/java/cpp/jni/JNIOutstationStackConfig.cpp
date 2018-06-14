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

#include "JNIOutstationStackConfig.h"

namespace jni
{
    namespace cache
    {
        bool OutstationStackConfig::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/OutstationStackConfig;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->linkConfigField = env->GetFieldID(this->clazz, "linkConfig", "Lcom/automatak/dnp3/LinkLayerConfig;");
            if(!this->linkConfigField) return false;

            this->outstationConfigField = env->GetFieldID(this->clazz, "outstationConfig", "Lcom/automatak/dnp3/OutstationConfig;");
            if(!this->outstationConfigField) return false;

            this->databaseConfigField = env->GetFieldID(this->clazz, "databaseConfig", "Lcom/automatak/dnp3/DatabaseConfig;");
            if(!this->databaseConfigField) return false;

            this->eventBufferConfigField = env->GetFieldID(this->clazz, "eventBufferConfig", "Lcom/automatak/dnp3/EventBufferConfig;");
            if(!this->eventBufferConfigField) return false;

            return true;
        }

        void OutstationStackConfig::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        LocalRef<jobject> OutstationStackConfig::getlinkConfig(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->linkConfigField));
        }

        LocalRef<jobject> OutstationStackConfig::getoutstationConfig(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->outstationConfigField));
        }

        LocalRef<jobject> OutstationStackConfig::getdatabaseConfig(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->databaseConfigField));
        }

        LocalRef<jobject> OutstationStackConfig::geteventBufferConfig(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->eventBufferConfigField));
        }
    }
}
