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

#include "JNIIndexedValue.h"

namespace jni
{
    namespace cache
    {
        bool IndexedValue::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/IndexedValue;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->init2Constructor = env->GetMethodID(this->clazz, "<init>", "(Ljava/lang/Object;I)V");
            if(!this->init2Constructor) return false;

            this->valueField = env->GetFieldID(this->clazz, "value", "Ljava/lang/Object;");
            if(!this->valueField) return false;

            this->indexField = env->GetFieldID(this->clazz, "index", "I");
            if(!this->indexField) return false;

            return true;
        }

        void IndexedValue::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        LocalRef<jobject> IndexedValue::init2(JNIEnv* env, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->NewObject(this->clazz, this->init2Constructor, arg0, arg1));
        }

        LocalRef<jobject> IndexedValue::getvalue(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->valueField));
        }

        jint IndexedValue::getindex(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->indexField);
        }
    }
}
