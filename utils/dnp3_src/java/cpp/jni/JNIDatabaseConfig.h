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

#ifndef OPENDNP3JAVA_JNIDATABASECONFIG_H
#define OPENDNP3JAVA_JNIDATABASECONFIG_H

#include <jni.h>

#include "../adapters/LocalRef.h"

namespace jni
{
    struct JCache;

    namespace cache
    {
        class DatabaseConfig
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // field getter methods
            LocalRef<jobject> getbinary(JNIEnv* env, jobject instance);
            LocalRef<jobject> getdoubleBinary(JNIEnv* env, jobject instance);
            LocalRef<jobject> getanalog(JNIEnv* env, jobject instance);
            LocalRef<jobject> getcounter(JNIEnv* env, jobject instance);
            LocalRef<jobject> getfrozenCounter(JNIEnv* env, jobject instance);
            LocalRef<jobject> getboStatus(JNIEnv* env, jobject instance);
            LocalRef<jobject> getaoStatus(JNIEnv* env, jobject instance);

            private:

            jclass clazz = nullptr;

            // field ids
            jfieldID binaryField = nullptr;
            jfieldID doubleBinaryField = nullptr;
            jfieldID analogField = nullptr;
            jfieldID counterField = nullptr;
            jfieldID frozenCounterField = nullptr;
            jfieldID boStatusField = nullptr;
            jfieldID aoStatusField = nullptr;
        };
    }
}

#endif
