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

#include "JNIAnalogOutputInt32.h"

namespace jni
{
    namespace cache
    {
        bool AnalogOutputInt32::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/AnalogOutputInt32;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->init2Constructor = env->GetMethodID(this->clazz, "<init>", "(ILcom/automatak/dnp3/enums/CommandStatus;)V");
            if(!this->init2Constructor) return false;

            this->valueField = env->GetFieldID(this->clazz, "value", "I");
            if(!this->valueField) return false;

            this->statusField = env->GetFieldID(this->clazz, "status", "Lcom/automatak/dnp3/enums/CommandStatus;");
            if(!this->statusField) return false;

            return true;
        }

        void AnalogOutputInt32::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        LocalRef<jobject> AnalogOutputInt32::init2(JNIEnv* env, jint arg0, jobject arg1)
        {
            return LocalRef<jobject>(env, env->NewObject(this->clazz, this->init2Constructor, arg0, arg1));
        }

        jint AnalogOutputInt32::getvalue(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->valueField);
        }

        LocalRef<jobject> AnalogOutputInt32::getstatus(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->statusField));
        }
    }
}
