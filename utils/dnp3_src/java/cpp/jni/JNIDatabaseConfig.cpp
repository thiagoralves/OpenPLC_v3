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

#include "JNIDatabaseConfig.h"

namespace jni
{
    namespace cache
    {
        bool DatabaseConfig::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/DatabaseConfig;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->binaryField = env->GetFieldID(this->clazz, "binary", "Ljava/util/List;");
            if(!this->binaryField) return false;

            this->doubleBinaryField = env->GetFieldID(this->clazz, "doubleBinary", "Ljava/util/List;");
            if(!this->doubleBinaryField) return false;

            this->analogField = env->GetFieldID(this->clazz, "analog", "Ljava/util/List;");
            if(!this->analogField) return false;

            this->counterField = env->GetFieldID(this->clazz, "counter", "Ljava/util/List;");
            if(!this->counterField) return false;

            this->frozenCounterField = env->GetFieldID(this->clazz, "frozenCounter", "Ljava/util/List;");
            if(!this->frozenCounterField) return false;

            this->boStatusField = env->GetFieldID(this->clazz, "boStatus", "Ljava/util/List;");
            if(!this->boStatusField) return false;

            this->aoStatusField = env->GetFieldID(this->clazz, "aoStatus", "Ljava/util/List;");
            if(!this->aoStatusField) return false;

            return true;
        }

        void DatabaseConfig::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        LocalRef<jobject> DatabaseConfig::getbinary(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->binaryField));
        }

        LocalRef<jobject> DatabaseConfig::getdoubleBinary(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->doubleBinaryField));
        }

        LocalRef<jobject> DatabaseConfig::getanalog(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->analogField));
        }

        LocalRef<jobject> DatabaseConfig::getcounter(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->counterField));
        }

        LocalRef<jobject> DatabaseConfig::getfrozenCounter(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->frozenCounterField));
        }

        LocalRef<jobject> DatabaseConfig::getboStatus(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->boStatusField));
        }

        LocalRef<jobject> DatabaseConfig::getaoStatus(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->aoStatusField));
        }
    }
}
