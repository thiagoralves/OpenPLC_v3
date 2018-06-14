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

#include "JNICommandPointResult.h"

namespace jni
{
    namespace cache
    {
        bool CommandPointResult::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/CommandPointResult;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->init4Constructor = env->GetMethodID(this->clazz, "<init>", "(IILcom/automatak/dnp3/enums/CommandPointState;Lcom/automatak/dnp3/enums/CommandStatus;)V");
            if(!this->init4Constructor) return false;

            return true;
        }

        void CommandPointResult::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        LocalRef<jobject> CommandPointResult::init4(JNIEnv* env, jint arg0, jint arg1, jobject arg2, jobject arg3)
        {
            return LocalRef<jobject>(env, env->NewObject(this->clazz, this->init4Constructor, arg0, arg1, arg2, arg3));
        }
    }
}
