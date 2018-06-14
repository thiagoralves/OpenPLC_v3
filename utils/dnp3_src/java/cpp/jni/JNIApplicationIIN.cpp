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

#include "JNIApplicationIIN.h"

namespace jni
{
    namespace cache
    {
        bool ApplicationIIN::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/ApplicationIIN;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->needTimeField = env->GetFieldID(this->clazz, "needTime", "Z");
            if(!this->needTimeField) return false;

            this->localControlField = env->GetFieldID(this->clazz, "localControl", "Z");
            if(!this->localControlField) return false;

            this->deviceTroubleField = env->GetFieldID(this->clazz, "deviceTrouble", "Z");
            if(!this->deviceTroubleField) return false;

            this->configCorruptField = env->GetFieldID(this->clazz, "configCorrupt", "Z");
            if(!this->configCorruptField) return false;

            return true;
        }

        void ApplicationIIN::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        jboolean ApplicationIIN::getneedTime(JNIEnv* env, jobject instance)
        {
            return env->GetBooleanField(instance, this->needTimeField);
        }

        jboolean ApplicationIIN::getlocalControl(JNIEnv* env, jobject instance)
        {
            return env->GetBooleanField(instance, this->localControlField);
        }

        jboolean ApplicationIIN::getdeviceTrouble(JNIEnv* env, jobject instance)
        {
            return env->GetBooleanField(instance, this->deviceTroubleField);
        }

        jboolean ApplicationIIN::getconfigCorrupt(JNIEnv* env, jobject instance)
        {
            return env->GetBooleanField(instance, this->configCorruptField);
        }
    }
}
