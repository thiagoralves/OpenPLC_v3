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

#include "JNIIterator.h"

namespace jni
{
    namespace cache
    {
        bool Iterator::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Ljava/util/Iterator;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->hasNextMethod = env->GetMethodID(this->clazz, "hasNext", "()Z");
            if(!this->hasNextMethod) return false;

            this->nextMethod = env->GetMethodID(this->clazz, "next", "()Ljava/lang/Object;");
            if(!this->nextMethod) return false;

            return true;
        }

        void Iterator::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        jboolean Iterator::hasNext(JNIEnv* env, jobject instance)
        {
            return env->CallBooleanMethod(instance, this->hasNextMethod);
        }

        LocalRef<jobject> Iterator::next(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->nextMethod));
        }
    }
}
