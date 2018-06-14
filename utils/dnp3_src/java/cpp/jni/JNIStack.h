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

#ifndef OPENDNP3JAVA_JNISTACK_H
#define OPENDNP3JAVA_JNISTACK_H

#include <jni.h>

#include "../adapters/LocalRef.h"

namespace jni
{
    struct JCache;

    namespace cache
    {
        class Stack
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // methods
            void shutdown(JNIEnv* env, jobject instance);
            void disable(JNIEnv* env, jobject instance);
            void enable(JNIEnv* env, jobject instance);
            void setLogLevel(JNIEnv* env, jobject instance, jint arg0);
            LocalRef<jobject> getStatistics(JNIEnv* env, jobject instance);

            private:

            jclass clazz = nullptr;

            // method ids
            jmethodID shutdownMethod = nullptr;
            jmethodID disableMethod = nullptr;
            jmethodID enableMethod = nullptr;
            jmethodID setLogLevelMethod = nullptr;
            jmethodID getStatisticsMethod = nullptr;
        };
    }
}

#endif
