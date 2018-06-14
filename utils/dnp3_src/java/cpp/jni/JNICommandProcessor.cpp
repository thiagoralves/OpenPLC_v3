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

#include "JNICommandProcessor.h"

namespace jni
{
    namespace cache
    {
        bool CommandProcessor::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/CommandProcessor;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->directOperateMethod = env->GetMethodID(this->clazz, "directOperate", "(Lcom/automatak/dnp3/CommandHeaders;)Ljava/util/concurrent/CompletableFuture;");
            if(!this->directOperateMethod) return false;

            this->selectAndOperateMethod = env->GetMethodID(this->clazz, "selectAndOperate", "(Lcom/automatak/dnp3/CommandHeaders;)Ljava/util/concurrent/CompletableFuture;");
            if(!this->selectAndOperateMethod) return false;

            this->selectAndOperateCROBMethod = env->GetMethodID(this->clazz, "selectAndOperateCROB", "(Lcom/automatak/dnp3/ControlRelayOutputBlock;I)Ljava/util/concurrent/CompletableFuture;");
            if(!this->selectAndOperateCROBMethod) return false;

            this->directOperateAOInt32Method = env->GetMethodID(this->clazz, "directOperateAOInt32", "(Lcom/automatak/dnp3/AnalogOutputInt32;I)Ljava/util/concurrent/CompletableFuture;");
            if(!this->directOperateAOInt32Method) return false;

            this->selectAndOperateAOInt32Method = env->GetMethodID(this->clazz, "selectAndOperateAOInt32", "(Lcom/automatak/dnp3/AnalogOutputInt32;I)Ljava/util/concurrent/CompletableFuture;");
            if(!this->selectAndOperateAOInt32Method) return false;

            this->directOperateAOInt16Method = env->GetMethodID(this->clazz, "directOperateAOInt16", "(Lcom/automatak/dnp3/AnalogOutputInt16;I)Ljava/util/concurrent/CompletableFuture;");
            if(!this->directOperateAOInt16Method) return false;

            this->selectAndOperateAOInt16Method = env->GetMethodID(this->clazz, "selectAndOperateAOInt16", "(Lcom/automatak/dnp3/AnalogOutputInt16;I)Ljava/util/concurrent/CompletableFuture;");
            if(!this->selectAndOperateAOInt16Method) return false;

            this->selectAndOperateAOFloat32Method = env->GetMethodID(this->clazz, "selectAndOperateAOFloat32", "(Lcom/automatak/dnp3/AnalogOutputFloat32;I)Ljava/util/concurrent/CompletableFuture;");
            if(!this->selectAndOperateAOFloat32Method) return false;

            this->directOperateCROBMethod = env->GetMethodID(this->clazz, "directOperateCROB", "(Lcom/automatak/dnp3/ControlRelayOutputBlock;I)Ljava/util/concurrent/CompletableFuture;");
            if(!this->directOperateCROBMethod) return false;

            this->directOperateAOFloat32Method = env->GetMethodID(this->clazz, "directOperateAOFloat32", "(Lcom/automatak/dnp3/AnalogOutputFloat32;I)Ljava/util/concurrent/CompletableFuture;");
            if(!this->directOperateAOFloat32Method) return false;

            this->directOperateAODouble64Method = env->GetMethodID(this->clazz, "directOperateAODouble64", "(Lcom/automatak/dnp3/AnalogOutputDouble64;I)Ljava/util/concurrent/CompletableFuture;");
            if(!this->directOperateAODouble64Method) return false;

            this->selectAndOperateAODouble64Method = env->GetMethodID(this->clazz, "selectAndOperateAODouble64", "(Lcom/automatak/dnp3/AnalogOutputDouble64;I)Ljava/util/concurrent/CompletableFuture;");
            if(!this->selectAndOperateAODouble64Method) return false;

            return true;
        }

        void CommandProcessor::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        LocalRef<jobject> CommandProcessor::directOperate(JNIEnv* env, jobject instance, jobject arg0)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->directOperateMethod, arg0));
        }

        LocalRef<jobject> CommandProcessor::selectAndOperate(JNIEnv* env, jobject instance, jobject arg0)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->selectAndOperateMethod, arg0));
        }

        LocalRef<jobject> CommandProcessor::selectAndOperateCROB(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->selectAndOperateCROBMethod, arg0, arg1));
        }

        LocalRef<jobject> CommandProcessor::directOperateAOInt32(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->directOperateAOInt32Method, arg0, arg1));
        }

        LocalRef<jobject> CommandProcessor::selectAndOperateAOInt32(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->selectAndOperateAOInt32Method, arg0, arg1));
        }

        LocalRef<jobject> CommandProcessor::directOperateAOInt16(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->directOperateAOInt16Method, arg0, arg1));
        }

        LocalRef<jobject> CommandProcessor::selectAndOperateAOInt16(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->selectAndOperateAOInt16Method, arg0, arg1));
        }

        LocalRef<jobject> CommandProcessor::selectAndOperateAOFloat32(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->selectAndOperateAOFloat32Method, arg0, arg1));
        }

        LocalRef<jobject> CommandProcessor::directOperateCROB(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->directOperateCROBMethod, arg0, arg1));
        }

        LocalRef<jobject> CommandProcessor::directOperateAOFloat32(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->directOperateAOFloat32Method, arg0, arg1));
        }

        LocalRef<jobject> CommandProcessor::directOperateAODouble64(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->directOperateAODouble64Method, arg0, arg1));
        }

        LocalRef<jobject> CommandProcessor::selectAndOperateAODouble64(JNIEnv* env, jobject instance, jobject arg0, jint arg1)
        {
            return LocalRef<jobject>(env, env->CallObjectMethod(instance, this->selectAndOperateAODouble64Method, arg0, arg1));
        }
    }
}
