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

#include "JNICommandHandler.h"

namespace jni
{
    namespace cache
    {
        bool CommandHandler::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/CommandHandler;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->startMethod = env->GetMethodID(this->clazz, "start", "()V");
            if(!this->startMethod) return false;

            this->endMethod = env->GetMethodID(this->clazz, "end", "()V");
            if(!this->endMethod) return false;

            this->operateAOI16Method = env->GetMethodID(this->clazz, "operateAOI16", "(Lcom/automatak/dnp3/AnalogOutputInt16;ILcom/automatak/dnp3/enums/OperateType;)Lcom/automatak/dnp3/enums/CommandStatus;");
            if(!this->operateAOI16Method) return false;

            this->operateAOI32Method = env->GetMethodID(this->clazz, "operateAOI32", "(Lcom/automatak/dnp3/AnalogOutputInt32;ILcom/automatak/dnp3/enums/OperateType;)Lcom/automatak/dnp3/enums/CommandStatus;");
            if(!this->operateAOI32Method) return false;

            this->operateAOF32Method = env->GetMethodID(this->clazz, "operateAOF32", "(Lcom/automatak/dnp3/AnalogOutputFloat32;ILcom/automatak/dnp3/enums/OperateType;)Lcom/automatak/dnp3/enums/CommandStatus;");
            if(!this->operateAOF32Method) return false;

            this->operateAOD64Method = env->GetMethodID(this->clazz, "operateAOD64", "(Lcom/automatak/dnp3/AnalogOutputDouble64;ILcom/automatak/dnp3/enums/OperateType;)Lcom/automatak/dnp3/enums/CommandStatus;");
            if(!this->operateAOD64Method) return false;

            this->selectCROBMethod = env->GetMethodID(this->clazz, "selectCROB", "(Lcom/automatak/dnp3/ControlRelayOutputBlock;I)Lcom/automatak/dnp3/enums/CommandStatus;");
            if(!this->selectCROBMethod) return false;

            this->selectAOF32Method = env->GetMethodID(this->clazz, "selectAOF32", "(Lcom/automatak/dnp3/AnalogOutputFloat32;I)Lcom/automatak/dnp3/enums/CommandStatus;");
            if(!this->selectAOF32Method) return false;

            this->selectAOD64Method = env->GetMethodID(this->clazz, "selectAOD64", "(Lcom/automatak/dnp3/AnalogOutputDouble64;I)Lcom/automatak/dnp3/enums/CommandStatus;");
            if(!this->selectAOD64Method) return false;

            this->operateCROBMethod = env->GetMethodID(this->clazz, "operateCROB", "(Lcom/automatak/dnp3/ControlRelayOutputBlock;ILcom/automatak/dnp3/enums/OperateType;)Lcom/automatak/dnp3/enums/CommandStatus;");
            if(!this->operateCROBMethod) return false;

            this->selectAOI16Method = env->GetMethodID(this->clazz, "selectAOI16", "(Lcom/automatak/dnp3/AnalogOutputInt16;I)Lcom/automatak/dnp3/enums/CommandStatus;");
            if(!this->selectAOI16Method) return false;

            this->selectAOI32Method = env->GetMethodID(this->clazz, "selectAOI32", "(Lcom/automatak/dnp3/AnalogOutputInt32;I)Lcom/automatak/dnp3/enums/CommandStatus;");
            if(!this->selectAOI32Method) return false;

            return true;
        }

        void CommandHandler::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        void CommandHandler::start(JNIEnv* env, jobject instance)
        {
            env->CallVoidMethod(instance, this->startMethod);
        }

        void CommandHandler::end(JNIEnv* env, jobject instance)
        {
            env->CallVoidMethod(instance, this->endMethod);
        }

        LocalRef<jobject> CommandHandler::operateAOI16(JNIEnv* env, jobject instance, jobject arg0, jint arg1, jobject arg2)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->operateAOI16Method, arg0, arg1, arg2));
        }

        LocalRef<jobject> CommandHandler::operateAOI32(JNIEnv* env, jobject instance, jobject arg0, jint arg1, jobject arg2)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->operateAOI32Method, arg0, arg1, arg2));
        }

        LocalRef<jobject> CommandHandler::operateAOF32(JNIEnv* env, jobject instance, jobject arg0, jint arg1, jobject arg2)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->operateAOF32Method, arg0, arg1, arg2));
        }

        LocalRef<jobject> CommandHandler::operateAOD64(JNIEnv* env, jobject instance, jobject arg0, jint arg1, jobject arg2)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->operateAOD64Method, arg0, arg1, arg2));
        }

        LocalRef<jobject> CommandHandler::selectCROB(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->selectCROBMethod, arg0, arg1));
        }

        LocalRef<jobject> CommandHandler::selectAOF32(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->selectAOF32Method, arg0, arg1));
        }

        LocalRef<jobject> CommandHandler::selectAOD64(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->selectAOD64Method, arg0, arg1));
        }

        LocalRef<jobject> CommandHandler::operateCROB(JNIEnv* env, jobject instance, jobject arg0, jint arg1, jobject arg2)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->operateCROBMethod, arg0, arg1, arg2));
        }

        LocalRef<jobject> CommandHandler::selectAOI16(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->selectAOI16Method, arg0, arg1));
        }

        LocalRef<jobject> CommandHandler::selectAOI32(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->selectAOI32Method, arg0, arg1));
        }
    }
}
