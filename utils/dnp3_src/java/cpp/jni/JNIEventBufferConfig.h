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

#ifndef OPENDNP3JAVA_JNIEVENTBUFFERCONFIG_H
#define OPENDNP3JAVA_JNIEVENTBUFFERCONFIG_H

#include <jni.h>

#include "../adapters/LocalRef.h"

namespace jni
{
    struct JCache;

    namespace cache
    {
        class EventBufferConfig
        {
            friend struct jni::JCache;

            bool init(JNIEnv* env);
            void cleanup(JNIEnv* env);

            public:

            // field getter methods
            jint getmaxBinaryEvents(JNIEnv* env, jobject instance);
            jint getmaxDoubleBinaryEvents(JNIEnv* env, jobject instance);
            jint getmaxAnalogEvents(JNIEnv* env, jobject instance);
            jint getmaxCounterEvents(JNIEnv* env, jobject instance);
            jint getmaxFrozenCounterEvents(JNIEnv* env, jobject instance);
            jint getmaxBinaryOutputStatusEvents(JNIEnv* env, jobject instance);
            jint getmaxAnalogOutputStatusEvents(JNIEnv* env, jobject instance);

            private:

            jclass clazz = nullptr;

            // field ids
            jfieldID maxBinaryEventsField = nullptr;
            jfieldID maxDoubleBinaryEventsField = nullptr;
            jfieldID maxAnalogEventsField = nullptr;
            jfieldID maxCounterEventsField = nullptr;
            jfieldID maxFrozenCounterEventsField = nullptr;
            jfieldID maxBinaryOutputStatusEventsField = nullptr;
            jfieldID maxAnalogOutputStatusEventsField = nullptr;
        };
    }
}

#endif
