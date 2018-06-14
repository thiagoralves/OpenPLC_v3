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

#include "JNIClassAssignment.h"

namespace jni
{
    namespace cache
    {
        bool ClassAssignment::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/ClassAssignment;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->groupField = env->GetFieldID(this->clazz, "group", "B");
            if(!this->groupField) return false;

            this->variationField = env->GetFieldID(this->clazz, "variation", "B");
            if(!this->variationField) return false;

            this->clazzField = env->GetFieldID(this->clazz, "clazz", "Lcom/automatak/dnp3/enums/PointClass;");
            if(!this->clazzField) return false;

            this->rangeField = env->GetFieldID(this->clazz, "range", "Lcom/automatak/dnp3/Range;");
            if(!this->rangeField) return false;

            return true;
        }

        void ClassAssignment::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        jbyte ClassAssignment::getgroup(JNIEnv* env, jobject instance)
        {
            return env->GetByteField(instance, this->groupField);
        }

        jbyte ClassAssignment::getvariation(JNIEnv* env, jobject instance)
        {
            return env->GetByteField(instance, this->variationField);
        }

        LocalRef<jobject> ClassAssignment::getclazz(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->clazzField));
        }

        LocalRef<jobject> ClassAssignment::getrange(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->GetObjectField(instance, this->rangeField));
        }
    }
}
