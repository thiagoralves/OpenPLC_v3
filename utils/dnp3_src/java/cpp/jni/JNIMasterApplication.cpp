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

#include "JNIMasterApplication.h"

namespace jni
{
    namespace cache
    {
        bool MasterApplication::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/MasterApplication;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->getMillisecondsSinceEpochMethod = env->GetMethodID(this->clazz, "getMillisecondsSinceEpoch", "()J");
            if(!this->getMillisecondsSinceEpochMethod) return false;

            this->onOpenMethod = env->GetMethodID(this->clazz, "onOpen", "()V");
            if(!this->onOpenMethod) return false;

            this->onCloseMethod = env->GetMethodID(this->clazz, "onClose", "()V");
            if(!this->onCloseMethod) return false;

            this->onTaskStartMethod = env->GetMethodID(this->clazz, "onTaskStart", "(Lcom/automatak/dnp3/enums/MasterTaskType;Lcom/automatak/dnp3/TaskId;)V");
            if(!this->onTaskStartMethod) return false;

            this->onTaskCompleteMethod = env->GetMethodID(this->clazz, "onTaskComplete", "(Lcom/automatak/dnp3/TaskInfo;)V");
            if(!this->onTaskCompleteMethod) return false;

            this->onReceiveIINMethod = env->GetMethodID(this->clazz, "onReceiveIIN", "(Lcom/automatak/dnp3/IINField;)V");
            if(!this->onReceiveIINMethod) return false;

            this->getClassAssignmentsMethod = env->GetMethodID(this->clazz, "getClassAssignments", "()Ljava/lang/Iterable;");
            if(!this->getClassAssignmentsMethod) return false;

            this->assignClassDuringStartupMethod = env->GetMethodID(this->clazz, "assignClassDuringStartup", "()Z");
            if(!this->assignClassDuringStartupMethod) return false;

            return true;
        }

        void MasterApplication::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        jlong MasterApplication::getMillisecondsSinceEpoch(JNIEnv* env, jobject instance)
        {
            return env->CallLongMethod(instance, this->getMillisecondsSinceEpochMethod);
        }

        void MasterApplication::onOpen(JNIEnv* env, jobject instance)
        {
            env->CallVoidMethod(instance, this->onOpenMethod);
        }

        void MasterApplication::onClose(JNIEnv* env, jobject instance)
        {
            env->CallVoidMethod(instance, this->onCloseMethod);
        }

        void MasterApplication::onTaskStart(JNIEnv* env, jobject instance, jobject arg0, jobject arg1)
        {
            env->CallVoidMethod(instance, this->onTaskStartMethod, arg0, arg1);
        }

        void MasterApplication::onTaskComplete(JNIEnv* env, jobject instance, jobject arg0)
        {
            env->CallVoidMethod(instance, this->onTaskCompleteMethod, arg0);
        }

        void MasterApplication::onReceiveIIN(JNIEnv* env, jobject instance, jobject arg0)
        {
            env->CallVoidMethod(instance, this->onReceiveIINMethod, arg0);
        }

        LocalRef<jobject> MasterApplication::getClassAssignments(JNIEnv* env, jobject instance)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->getClassAssignmentsMethod));
        }

        jboolean MasterApplication::assignClassDuringStartup(JNIEnv* env, jobject instance)
        {
            return env->CallBooleanMethod(instance, this->assignClassDuringStartupMethod);
        }
    }
}
