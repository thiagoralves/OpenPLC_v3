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

#ifndef OPENDNP3JAVA_JNIOUTSTATIONCONFIG_H
#define OPENDNP3JAVA_JNIOUTSTATIONCONFIG_H

#include <jni.h>

#include "../adapters/LocalRef.h"

namespace jni
{
    struct JCache;

    namespace cache
    {
        class OutstationConfig
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // field getter methods
            LocalRef<jobject> getindexMode(JNIEnv* env, jobject instance);
            jshort getmaxControlsPerRequest(JNIEnv* env, jobject instance);
            LocalRef<jobject> getselectTimeout(JNIEnv* env, jobject instance);
            LocalRef<jobject> getsolConfirmTimeout(JNIEnv* env, jobject instance);
            LocalRef<jobject> getunsolRetryTimeout(JNIEnv* env, jobject instance);
            jint getmaxTxFragSize(JNIEnv* env, jobject instance);
            jint getmaxRxFragSize(JNIEnv* env, jobject instance);
            jboolean getallowUnsolicited(JNIEnv* env, jobject instance);

            private:

            jclass clazz = nullptr;

            // field ids
            jfieldID indexModeField = nullptr;
            jfieldID maxControlsPerRequestField = nullptr;
            jfieldID selectTimeoutField = nullptr;
            jfieldID solConfirmTimeoutField = nullptr;
            jfieldID unsolRetryTimeoutField = nullptr;
            jfieldID maxTxFragSizeField = nullptr;
            jfieldID maxRxFragSizeField = nullptr;
            jfieldID allowUnsolicitedField = nullptr;
        };
    }
}

#endif
