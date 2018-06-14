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

#include "JNIIndexMode.h"

namespace jni
{
    namespace cache
    {
        bool IndexMode::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/enums/IndexMode;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->valuesMethod = env->GetStaticMethodID(this->clazz, "values", "()[Lcom/automatak/dnp3/enums/IndexMode;");
            if(!this->valuesMethod) return false;

            this->valueOfMethod = env->GetStaticMethodID(this->clazz, "valueOf", "(Ljava/lang/String;)Lcom/automatak/dnp3/enums/IndexMode;");
            if(!this->valueOfMethod) return false;

            this->fromTypeMethod = env->GetStaticMethodID(this->clazz, "fromType", "(I)Lcom/automatak/dnp3/enums/IndexMode;");
            if(!this->fromTypeMethod) return false;

            this->toTypeMethod = env->GetMethodID(this->clazz, "toType", "()I");
            if(!this->toTypeMethod) return false;

            return true;
        }

        void IndexMode::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        LocalRef<jobject> IndexMode::values(JNIEnv* env)
        {
            return LocalRef<jobject>(env, env->CallStaticObjectMethod(this->clazz, this->valuesMethod));
        }

        LocalRef<jobject> IndexMode::valueOf(JNIEnv* env, jstring arg0)
        {
            return LocalRef<jobject>(env, env->CallStaticObjectMethod(this->clazz, this->valueOfMethod, arg0));
        }

        LocalRef<jobject> IndexMode::fromType(JNIEnv* env, jint arg0)
        {
            return LocalRef<jobject>(env, env->CallStaticObjectMethod(this->clazz, this->fromTypeMethod, arg0));
        }

        jint IndexMode::toType(JNIEnv* env, jobject instance)
        {
            return env->CallIntMethod(instance, this->toTypeMethod);
        }
    }
}
