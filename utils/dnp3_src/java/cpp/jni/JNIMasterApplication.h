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

#ifndef OPENDNP3JAVA_JNIMASTERAPPLICATION_H
#define OPENDNP3JAVA_JNIMASTERAPPLICATION_H

#include <jni.h>

#include "../adapters/LocalRef.h"

namespace jni
{
    struct JCache;

    namespace cache
    {
        class MasterApplication
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // methods
            jlong getMillisecondsSinceEpoch(JNIEnv* env, jobject instance);
            void onOpen(JNIEnv* env, jobject instance);
            void onClose(JNIEnv* env, jobject instance);
            void onTaskStart(JNIEnv* env, jobject instance, jobject arg0, jobject arg1);
            void onTaskComplete(JNIEnv* env, jobject instance, jobject arg0);
            void onReceiveIIN(JNIEnv* env, jobject instance, jobject arg0);
            LocalRef<jobject> getClassAssignments(JNIEnv* env, jobject instance);
            jboolean assignClassDuringStartup(JNIEnv* env, jobject instance);

            private:

            jclass clazz = nullptr;

            // method ids
            jmethodID getMillisecondsSinceEpochMethod = nullptr;
            jmethodID onOpenMethod = nullptr;
            jmethodID onCloseMethod = nullptr;
            jmethodID onTaskStartMethod = nullptr;
            jmethodID onTaskCompleteMethod = nullptr;
            jmethodID onReceiveIINMethod = nullptr;
            jmethodID getClassAssignmentsMethod = nullptr;
            jmethodID assignClassDuringStartupMethod = nullptr;
        };
    }
}

#endif
