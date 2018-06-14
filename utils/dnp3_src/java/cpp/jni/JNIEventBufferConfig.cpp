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

#include "JNIEventBufferConfig.h"

namespace jni
{
    namespace cache
    {
        bool EventBufferConfig::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/EventBufferConfig;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->maxBinaryEventsField = env->GetFieldID(this->clazz, "maxBinaryEvents", "I");
            if(!this->maxBinaryEventsField) return false;

            this->maxDoubleBinaryEventsField = env->GetFieldID(this->clazz, "maxDoubleBinaryEvents", "I");
            if(!this->maxDoubleBinaryEventsField) return false;

            this->maxAnalogEventsField = env->GetFieldID(this->clazz, "maxAnalogEvents", "I");
            if(!this->maxAnalogEventsField) return false;

            this->maxCounterEventsField = env->GetFieldID(this->clazz, "maxCounterEvents", "I");
            if(!this->maxCounterEventsField) return false;

            this->maxFrozenCounterEventsField = env->GetFieldID(this->clazz, "maxFrozenCounterEvents", "I");
            if(!this->maxFrozenCounterEventsField) return false;

            this->maxBinaryOutputStatusEventsField = env->GetFieldID(this->clazz, "maxBinaryOutputStatusEvents", "I");
            if(!this->maxBinaryOutputStatusEventsField) return false;

            this->maxAnalogOutputStatusEventsField = env->GetFieldID(this->clazz, "maxAnalogOutputStatusEvents", "I");
            if(!this->maxAnalogOutputStatusEventsField) return false;

            return true;
        }

        void EventBufferConfig::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        jint EventBufferConfig::getmaxBinaryEvents(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->maxBinaryEventsField);
        }

        jint EventBufferConfig::getmaxDoubleBinaryEvents(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->maxDoubleBinaryEventsField);
        }

        jint EventBufferConfig::getmaxAnalogEvents(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->maxAnalogEventsField);
        }

        jint EventBufferConfig::getmaxCounterEvents(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->maxCounterEventsField);
        }

        jint EventBufferConfig::getmaxFrozenCounterEvents(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->maxFrozenCounterEventsField);
        }

        jint EventBufferConfig::getmaxBinaryOutputStatusEvents(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->maxBinaryOutputStatusEventsField);
        }

        jint EventBufferConfig::getmaxAnalogOutputStatusEvents(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->maxAnalogOutputStatusEventsField);
        }
    }
}
