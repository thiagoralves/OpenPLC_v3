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

#ifndef OPENDNP3JAVA_JNITLSCONFIG_H
#define OPENDNP3JAVA_JNITLSCONFIG_H

#include <jni.h>

#include "../adapters/LocalRef.h"

namespace jni
{
    struct JCache;

    namespace cache
    {
        class TLSConfig
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // field getter methods
            LocalRef<jstring> getpeerCertFilePath(JNIEnv* env, jobject instance);
            LocalRef<jstring> getlocalCertFilePath(JNIEnv* env, jobject instance);
            LocalRef<jstring> getprivateKeyFilePath(JNIEnv* env, jobject instance);
            jint getmaxVerifyDepth(JNIEnv* env, jobject instance);
            jboolean getallowTLSv10(JNIEnv* env, jobject instance);
            jboolean getallowTLSv11(JNIEnv* env, jobject instance);
            jboolean getallowTLSv12(JNIEnv* env, jobject instance);
            LocalRef<jstring> getcipherList(JNIEnv* env, jobject instance);

            private:

            jclass clazz = nullptr;

            // field ids
            jfieldID peerCertFilePathField = nullptr;
            jfieldID localCertFilePathField = nullptr;
            jfieldID privateKeyFilePathField = nullptr;
            jfieldID maxVerifyDepthField = nullptr;
            jfieldID allowTLSv10Field = nullptr;
            jfieldID allowTLSv11Field = nullptr;
            jfieldID allowTLSv12Field = nullptr;
            jfieldID cipherListField = nullptr;
        };
    }
}

#endif
