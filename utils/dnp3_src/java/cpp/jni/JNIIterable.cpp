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

#include "JNIIterable.h"

namespace jni
{
    namespace cache
    {
        bool Iterable::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Ljava/lang/Iterable;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->iteratorMethod = env->GetMethodID(this->clazz, "iterator", "()Ljava/util/Iterator;");
            if(!this->iteratorMethod) return false;

            return true;
        }

        void Iterable::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        LocalRef<jobject> Iterable::iterator(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->iteratorMethod));
        }
    }
}
