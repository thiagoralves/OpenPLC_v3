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

#include "JNIHeader.h"

namespace jni
{
    namespace cache
    {
        bool Header::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/Header;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->groupField = env->GetFieldID(this->clazz, "group", "B");
            if(!this->groupField) return false;

            this->variationField = env->GetFieldID(this->clazz, "variation", "B");
            if(!this->variationField) return false;

            this->qualifierField = env->GetFieldID(this->clazz, "qualifier", "Lcom/automatak/dnp3/enums/QualifierCode;");
            if(!this->qualifierField) return false;

            this->countField = env->GetFieldID(this->clazz, "count", "I");
            if(!this->countField) return false;

            this->startField = env->GetFieldID(this->clazz, "start", "I");
            if(!this->startField) return false;

            this->stopField = env->GetFieldID(this->clazz, "stop", "I");
            if(!this->stopField) return false;

            return true;
        }

        void Header::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        jbyte Header::getgroup(JNIEnv* env, jobject instance)
        {
            return env->GetByteField(instance, this->groupField);
        }

        jbyte Header::getvariation(JNIEnv* env, jobject instance)
        {
            return env->GetByteField(instance, this->variationField);
        }

        LocalRef<jobject> Header::getqualifier(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->qualifierField));
        }

        jint Header::getcount(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->countField);
        }

        jint Header::getstart(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->startField);
        }

        jint Header::getstop(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->stopField);
        }
    }
}
