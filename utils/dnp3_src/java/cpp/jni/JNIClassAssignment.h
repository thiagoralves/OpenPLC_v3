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

#ifndef OPENDNP3JAVA_JNICLASSASSIGNMENT_H
#define OPENDNP3JAVA_JNICLASSASSIGNMENT_H

#include <jni.h>

#include "../adapters/LocalRef.h"

namespace jni
{
    struct JCache;

    namespace cache
    {
        class ClassAssignment
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // field getter methods
            jbyte getgroup(JNIEnv* env, jobject instance);
            jbyte getvariation(JNIEnv* env, jobject instance);
            LocalRef<jobject> getclazz(JNIEnv* env, jobject instance);
            LocalRef<jobject> getrange(JNIEnv* env, jobject instance);

            private:

            jclass clazz = nullptr;

            // field ids
            jfieldID groupField = nullptr;
            jfieldID variationField = nullptr;
            jfieldID clazzField = nullptr;
            jfieldID rangeField = nullptr;
        };
    }
}

#endif
