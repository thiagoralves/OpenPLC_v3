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

#ifndef OPENDNP3JAVA_JNICOMMANDHANDLER_H
#define OPENDNP3JAVA_JNICOMMANDHANDLER_H

#include <jni.h>

#include "../adapters/LocalRef.h"

namespace jni
{
    struct JCache;

    namespace cache
    {
        class CommandHandler
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // methods
            void start(JNIEnv* env, jobject instance);
            void end(JNIEnv* env, jobject instance);
            LocalRef<jobject> operateAOI16(JNIEnv* env, jobject instance, jobject arg0, jint arg1, jobject arg2);
            LocalRef<jobject> operateAOI32(JNIEnv* env, jobject instance, jobject arg0, jint arg1, jobject arg2);
            LocalRef<jobject> operateAOF32(JNIEnv* env, jobject instance, jobject arg0, jint arg1, jobject arg2);
            LocalRef<jobject> operateAOD64(JNIEnv* env, jobject instance, jobject arg0, jint arg1, jobject arg2);
            LocalRef<jobject> selectCROB(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> selectAOF32(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> selectAOD64(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> operateCROB(JNIEnv* env, jobject instance, jobject arg0, jint arg1, jobject arg2);
            LocalRef<jobject> selectAOI16(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> selectAOI32(JNIEnv* env, jobject instance, jobject arg0, jint arg1);

            private:

            jclass clazz = nullptr;

            // method ids
            jmethodID startMethod = nullptr;
            jmethodID endMethod = nullptr;
            jmethodID operateAOI16Method = nullptr;
            jmethodID operateAOI32Method = nullptr;
            jmethodID operateAOF32Method = nullptr;
            jmethodID operateAOD64Method = nullptr;
            jmethodID selectCROBMethod = nullptr;
            jmethodID selectAOF32Method = nullptr;
            jmethodID selectAOD64Method = nullptr;
            jmethodID operateCROBMethod = nullptr;
            jmethodID selectAOI16Method = nullptr;
            jmethodID selectAOI32Method = nullptr;
        };
    }
}

#endif
