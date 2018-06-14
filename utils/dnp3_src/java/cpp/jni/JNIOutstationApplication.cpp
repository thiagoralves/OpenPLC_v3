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

#include "JNIOutstationApplication.h"

namespace jni
{
    namespace cache
    {
        bool OutstationApplication::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/OutstationApplication;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->recordClassAssignmentMethod = env->GetMethodID(this->clazz, "recordClassAssignment", "(Lcom/automatak/dnp3/enums/AssignClassType;Lcom/automatak/dnp3/enums/PointClass;II)V");
            if(!this->recordClassAssignmentMethod) return false;

            this->getApplicationIINMethod = env->GetMethodID(this->clazz, "getApplicationIIN", "()Lcom/automatak/dnp3/ApplicationIIN;");
            if(!this->getApplicationIINMethod) return false;

            this->supportsWriteAbsoluteTimeMethod = env->GetMethodID(this->clazz, "supportsWriteAbsoluteTime", "()Z");
            if(!this->supportsWriteAbsoluteTimeMethod) return false;

            this->writeAbsoluteTimeMethod = env->GetMethodID(this->clazz, "writeAbsoluteTime", "(J)Z");
            if(!this->writeAbsoluteTimeMethod) return false;

            this->supportsAssignClassMethod = env->GetMethodID(this->clazz, "supportsAssignClass", "()Z");
            if(!this->supportsAssignClassMethod) return false;

            return true;
        }

        void OutstationApplication::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        void OutstationApplication::recordClassAssignment(JNIEnv* env, jobject instance, jobject arg0, jobject arg1, jint arg2, jint arg3)
        {
            env->CallVoidMethod(instance, this->recordClassAssignmentMethod, arg0, arg1, arg2, arg3);
        }

        LocalRef<jobject> OutstationApplication::getApplicationIIN(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->getApplicationIINMethod));
        }

        jboolean OutstationApplication::supportsWriteAbsoluteTime(JNIEnv* env, jobject instance)
        {
            return env->CallBooleanMethod(instance, this->supportsWriteAbsoluteTimeMethod);
        }

        jboolean OutstationApplication::writeAbsoluteTime(JNIEnv* env, jobject instance, jlong arg0)
        {
            return env->CallBooleanMethod(instance, this->writeAbsoluteTimeMethod, arg0);
        }

        jboolean OutstationApplication::supportsAssignClass(JNIEnv* env, jobject instance)
        {
            return env->CallBooleanMethod(instance, this->supportsAssignClassMethod);
        }
    }
}
