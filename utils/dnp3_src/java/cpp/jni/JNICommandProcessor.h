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

#ifndef OPENDNP3JAVA_JNICOMMANDPROCESSOR_H
#define OPENDNP3JAVA_JNICOMMANDPROCESSOR_H

#include <jni.h>

#include "../adapters/LocalRef.h"

namespace jni
{
    struct JCache;

    namespace cache
    {
        class CommandProcessor
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // methods
            LocalRef<jobject> directOperate(JNIEnv* env, jobject instance, jobject arg0);
            LocalRef<jobject> selectAndOperate(JNIEnv* env, jobject instance, jobject arg0);
            LocalRef<jobject> selectAndOperateCROB(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> directOperateAOInt32(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> selectAndOperateAOInt32(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> directOperateAOInt16(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> selectAndOperateAOInt16(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> selectAndOperateAOFloat32(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> directOperateCROB(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> directOperateAOFloat32(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> directOperateAODouble64(JNIEnv* env, jobject instance, jobject arg0, jint arg1);
            LocalRef<jobject> selectAndOperateAODouble64(JNIEnv* env, jobject instance, jobject arg0, jint arg1);

            private:

            jclass clazz = nullptr;

            // method ids
            jmethodID directOperateMethod = nullptr;
            jmethodID selectAndOperateMethod = nullptr;
            jmethodID selectAndOperateCROBMethod = nullptr;
            jmethodID directOperateAOInt32Method = nullptr;
            jmethodID selectAndOperateAOInt32Method = nullptr;
            jmethodID directOperateAOInt16Method = nullptr;
            jmethodID selectAndOperateAOInt16Method = nullptr;
            jmethodID selectAndOperateAOFloat32Method = nullptr;
            jmethodID directOperateCROBMethod = nullptr;
            jmethodID directOperateAOFloat32Method = nullptr;
            jmethodID directOperateAODouble64Method = nullptr;
            jmethodID selectAndOperateAODouble64Method = nullptr;
        };
    }
}

#endif
