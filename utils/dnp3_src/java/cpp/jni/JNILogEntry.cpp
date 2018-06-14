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

#include "JNILogEntry.h"

namespace jni
{
    namespace cache
    {
        bool LogEntry::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/LogEntry;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->init4Constructor = env->GetMethodID(this->clazz, "<init>", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
            if(!this->init4Constructor) return false;

            return true;
        }

        void LogEntry::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        LocalRef<jobject> LogEntry::init4(JNIEnv* env, jint arg0, jstring arg1, jstring arg2, jstring arg3)
        {
            return LocalRef<jobject>(env, env->NewObject(this->clazz, this->init4Constructor, arg0, arg1, arg2, arg3));
        }
    }
}
