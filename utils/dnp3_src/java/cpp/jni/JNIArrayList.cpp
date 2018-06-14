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

#include "JNIArrayList.h"

namespace jni
{
    namespace cache
    {
        bool ArrayList::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Ljava/util/ArrayList;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->init1Constructor = env->GetMethodID(this->clazz, "<init>", "(I)V");
            if(!this->init1Constructor) return false;

            this->addMethod = env->GetMethodID(this->clazz, "add", "(Ljava/lang/Object;)Z");
            if(!this->addMethod) return false;

            return true;
        }

        void ArrayList::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        jboolean ArrayList::add(JNIEnv* env, jobject instance, jobject arg0)
        {
            return env->CallBooleanMethod(instance, this->addMethod, arg0);
        }

        LocalRef<jobject> ArrayList::init1(JNIEnv* env, jint arg0)
        {
            return LocalRef<jobject>(env, env->NewObject(this->clazz, this->init1Constructor, arg0));
        }
    }
}
