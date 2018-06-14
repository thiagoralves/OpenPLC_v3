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

#ifndef OPENDNP3JAVA_JNISOEHANDLER_H
#define OPENDNP3JAVA_JNISOEHANDLER_H

#include <jni.h>

#include "../adapters/LocalRef.h"

namespace jni
{
    struct JCache;

    namespace cache
    {
        class SOEHandler
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // methods
            void start(JNIEnv* env, jobject instance);
            void end(JNIEnv* env, jobject instance);
            void processDBI(JNIEnv* env, jobject instance, jobject arg0, jobject arg1);
            void processAI(JNIEnv* env, jobject instance, jobject arg0, jobject arg1);
            void processBOS(JNIEnv* env, jobject instance, jobject arg0, jobject arg1);
            void processAOS(JNIEnv* env, jobject instance, jobject arg0, jobject arg1);
            void processFC(JNIEnv* env, jobject instance, jobject arg0, jobject arg1);
            void processBI(JNIEnv* env, jobject instance, jobject arg0, jobject arg1);
            void processC(JNIEnv* env, jobject instance, jobject arg0, jobject arg1);
            void processDNPTime(JNIEnv* env, jobject instance, jobject arg0, jobject arg1);

            private:

            jclass clazz = nullptr;

            // method ids
            jmethodID startMethod = nullptr;
            jmethodID endMethod = nullptr;
            jmethodID processDBIMethod = nullptr;
            jmethodID processAIMethod = nullptr;
            jmethodID processBOSMethod = nullptr;
            jmethodID processAOSMethod = nullptr;
            jmethodID processFCMethod = nullptr;
            jmethodID processBIMethod = nullptr;
            jmethodID processCMethod = nullptr;
            jmethodID processDNPTimeMethod = nullptr;
        };
    }
}

#endif
