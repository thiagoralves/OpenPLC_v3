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

#include "JNICounterConfig.h"

namespace jni
{
    namespace cache
    {
        bool CounterConfig::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/CounterConfig;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->deadbandField = env->GetFieldID(this->clazz, "deadband", "I");
            if(!this->deadbandField) return false;

            this->eventVariationField = env->GetFieldID(this->clazz, "eventVariation", "Lcom/automatak/dnp3/enums/EventCounterVariation;");
            if(!this->eventVariationField) return false;

            this->staticVariationField = env->GetFieldID(this->clazz, "staticVariation", "Lcom/automatak/dnp3/enums/StaticCounterVariation;");
            if(!this->staticVariationField) return false;

            this->vIndexField = env->GetFieldID(this->clazz, "vIndex", "I");
            if(!this->vIndexField) return false;

            this->clazzField = env->GetFieldID(this->clazz, "clazz", "Lcom/automatak/dnp3/enums/PointClass;");
            if(!this->clazzField) return false;

            return true;
        }

        void CounterConfig::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        jint CounterConfig::getdeadband(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->deadbandField);
        }

        LocalRef<jobject> CounterConfig::geteventVariation(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->eventVariationField));
        }

        LocalRef<jobject> CounterConfig::getstaticVariation(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->staticVariationField));
        }
    }
}
