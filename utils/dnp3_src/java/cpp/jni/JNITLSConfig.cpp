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

#include "JNITLSConfig.h"

namespace jni
{
    namespace cache
    {
        bool TLSConfig::init(JNIEnv* env)
        {
            auto clazzTemp = env->FindClass("Lcom/automatak/dnp3/TLSConfig;");
            this->clazz = (jclass) env->NewGlobalRef(clazzTemp);
            env->DeleteLocalRef(clazzTemp);

            this->peerCertFilePathField = env->GetFieldID(this->clazz, "peerCertFilePath", "Ljava/lang/String;");
            if(!this->peerCertFilePathField) return false;

            this->localCertFilePathField = env->GetFieldID(this->clazz, "localCertFilePath", "Ljava/lang/String;");
            if(!this->localCertFilePathField) return false;

            this->privateKeyFilePathField = env->GetFieldID(this->clazz, "privateKeyFilePath", "Ljava/lang/String;");
            if(!this->privateKeyFilePathField) return false;

            this->maxVerifyDepthField = env->GetFieldID(this->clazz, "maxVerifyDepth", "I");
            if(!this->maxVerifyDepthField) return false;

            this->allowTLSv10Field = env->GetFieldID(this->clazz, "allowTLSv10", "Z");
            if(!this->allowTLSv10Field) return false;

            this->allowTLSv11Field = env->GetFieldID(this->clazz, "allowTLSv11", "Z");
            if(!this->allowTLSv11Field) return false;

            this->allowTLSv12Field = env->GetFieldID(this->clazz, "allowTLSv12", "Z");
            if(!this->allowTLSv12Field) return false;

            this->cipherListField = env->GetFieldID(this->clazz, "cipherList", "Ljava/lang/String;");
            if(!this->cipherListField) return false;

            return true;
        }

        void TLSConfig::cleanup(JNIEnv* env)
        {
            env->DeleteGlobalRef(this->clazz);
        }

        LocalRef<jstring> TLSConfig::getpeerCertFilePath(JNIEnv* env, jobject instance)
        {
            return LocalRef<jstring>(env, (jstring) env->GetObjectField(instance, this->peerCertFilePathField));
        }

        LocalRef<jstring> TLSConfig::getlocalCertFilePath(JNIEnv* env, jobject instance)
        {
            return LocalRef<jstring>(env, (jstring) env->GetObjectField(instance, this->localCertFilePathField));
        }

        LocalRef<jstring> TLSConfig::getprivateKeyFilePath(JNIEnv* env, jobject instance)
        {
            return LocalRef<jstring>(env, (jstring) env->GetObjectField(instance, this->privateKeyFilePathField));
        }

        jint TLSConfig::getmaxVerifyDepth(JNIEnv* env, jobject instance)
        {
            return env->GetIntField(instance, this->maxVerifyDepthField);
        }

        jboolean TLSConfig::getallowTLSv10(JNIEnv* env, jobject instance)
        {
            return env->GetBooleanField(instance, this->allowTLSv10Field);
        }

        jboolean TLSConfig::getallowTLSv11(JNIEnv* env, jobject instance)
        {
            return env->GetBooleanField(instance, this->allowTLSv11Field);
        }

        jboolean TLSConfig::getallowTLSv12(JNIEnv* env, jobject instance)
        {
            return env->GetBooleanField(instance, this->allowTLSv12Field);
        }

        LocalRef<jstring> TLSConfig::getcipherList(JNIEnv* env, jobject instance)
        {
            return LocalRef<jstring>(env, (jstring) env->GetObjectField(instance, this->cipherListField));
        }
    }
}
