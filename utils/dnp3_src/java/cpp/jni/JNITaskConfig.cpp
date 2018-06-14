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

#include "JNITaskConfig.h"

namespace jni
{
    bool TaskConfig::init(JNIEnv* env)
    {
        auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/TaskConfig;");
        this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
        env->DeleteLocalRef(clazzTemp);

        this->init2Constructor = env->GetMethodID(this->clazz, "<init>", "(Lcom/automatak/dnp3/TaskId;Lcom/automatak/dnp3/TaskCallback;)V");
        if(!this->init2Constructor) return false;

        return true;
    }

    void TaskConfig::cleanup(JNIEnv* env)
    {
        env->DeleteGlobalRef(this->clazz);
    }

    jobject TaskConfig::init2(JNIEnv* env, jobject arg0, jobject arg1)
    {
        return env->NewObject(this->clazz, this->init2Constructor, arg0, arg1);
    }
}
