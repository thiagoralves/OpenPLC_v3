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

#include "JNICompletableFuture.h"

namespace jni
{
    namespace cache
    {
        bool CompletableFuture::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Ljava/util/concurrent/CompletableFuture;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->completeMethod = env->GetMethodID(this->clazz, "complete", "(Ljava/lang/Object;)Z");
            if(!this->completeMethod) return false;

            return true;
        }

        void CompletableFuture::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        jboolean CompletableFuture::complete(JNIEnv* env, jobject instance, jobject arg0)
        {
            return env->CallBooleanMethod(instance, this->completeMethod, arg0);
        }
    }
}
