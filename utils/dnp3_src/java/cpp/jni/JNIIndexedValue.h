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

#ifndef OPENDNP3JAVA_JNIINDEXEDVALUE_H
#define OPENDNP3JAVA_JNIINDEXEDVALUE_H

#include <jni.h>

#include "../adapters/LocalRef.h"

namespace jni
{
    struct JCache;

    namespace cache
    {
        class IndexedValue
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // constructor methods
            LocalRef<jobject> init2(JNIEnv* env, jobject arg0, jint arg1);

            // field getter methods
            LocalRef<jobject> getvalue(JNIEnv* env, jobject instance);
            jint getindex(JNIEnv* env, jobject instance);

            private:

            jclass clazz = nullptr;

            // constructor method ids
            jmethodID init2Constructor = nullptr;

            // field ids
            jfieldID valueField = nullptr;
            jfieldID indexField = nullptr;
        };
    }
}

#endif
