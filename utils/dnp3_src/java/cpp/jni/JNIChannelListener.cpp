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

#include "JNIChannelListener.h"

namespace jni
{
    namespace cache
    {
        bool ChannelListener::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/ChannelListener;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->onStateChangeMethod = env->GetMethodID(this->clazz, "onStateChange", "(Lcom/automatak/dnp3/enums/ChannelState;)V");
            if(!this->onStateChangeMethod) return false;

            return true;
        }

        void ChannelListener::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        void ChannelListener::onStateChange(JNIEnv* env, jobject instance, jobject arg0)
        {
            env->CallVoidMethod(instance, this->onStateChangeMethod, arg0);
        }
    }
}
