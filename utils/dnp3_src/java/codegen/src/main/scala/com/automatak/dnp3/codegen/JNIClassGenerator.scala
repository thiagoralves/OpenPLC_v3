package com.automatak.dnp3.codegen

import java.lang.reflect.{Constructor, Field, Method}


case class JNIClassGenerator(cfg: ClassConfig) {

  def headerFileName : String = "JNI%s.h".format(cfg.clazz.getSimpleName)

  def implFileName : String = "JNI%s.cpp".format(cfg.clazz.getSimpleName)

  def header(implicit indent: Indentation): Iterator[String] = {

    def classMember: Iterator[String] = "jclass clazz = nullptr;".iter

    def methodSignatures: Iterator[String] = cfg.ifEnabled(Features.Methods) {
      space ++ "// methods".iter ++ cfg.methods.map(m => "%s;".format(JNIMethod.getSignature(m))).toIterator
    }

    def methodsMembers: Iterator[String] = cfg.ifEnabled(Features.Methods) {
      space ++ "// method ids".iter ++ cfg.methods.map(m => "jmethodID %sMethod = nullptr;".format(m.getName)).toIterator
    }

    def constructorSignatures: Iterator[String] = cfg.ifEnabled(Features.Constructors) {
      space ++ "// constructor methods".iter ++ cfg.constructors.map(c => JNIMethod.getConstructorSignature(c)+";").toIterator
    }

    def constructorMembers: Iterator[String] = cfg.ifEnabled(Features.Constructors) {
      space ++ "// constructor method ids".iter ++ cfg.constructors.map(c => "jmethodID init%dConstructor = nullptr;".format(c.getParameterCount)).toIterator
    }

    def fieldMembers: Iterator[String] = cfg.ifEnabled(Features.Fields) {
      space ++ "// field ids".iter ++ cfg.clazz.getFields.map(f => "jfieldID %sField = nullptr;".format(f.getName)).toIterator
    }

    def fieldGetters: Iterator[String] = cfg.ifEnabled(Features.Fields) {

      def returnType(f: Field) : String = {
        if(f.getType.isPrimitive) JNIMethod.getType(f.getType) else "LocalRef<%s>".format(JNIMethod.getType(f.getType))
      }
      space ++ "// field getter methods".iter ++ cfg.fields.map(f => "%s get%s(JNIEnv* env, jobject instance);".format(returnType(f), f.getName)).toIterator
    }

    def initSignature: Iterator[String] = "bool init(JNIEnv* env);".iter

    def cleanupSignature: Iterator[String] = "void cleanup(JNIEnv* env);".iter

    commented(LicenseHeader()) ++ space ++
      includeGuards("JNI%s".format(cfg.clazz.getSimpleName)) {
        "#include <jni.h>".iter ++ space ++
        """#include "../adapters/LocalRef.h"""".iter ++ space ++
        namespace("jni") {
          "struct JCache;".iter ++ space ++
          namespace("cache") {
            classDef(cfg.clazz.getSimpleName) {
              "friend struct jni::JCache;".iter ++ space ++
                initSignature ++ cleanupSignature ++ space ++
                "public:".iter ++
                constructorSignatures ++ methodSignatures ++ fieldGetters ++
                space ++ "private:".iter ++ space ++
                classMember ++ constructorMembers ++ methodsMembers ++ fieldMembers
            }
          }
        }
      }
  }

  def impl(implicit indent: Indentation): Iterator[String] = {

    def initImpl: Iterator[String] = {

      def setAndCheckReturn(name: String)(value: String) : Iterator[String] = {
        space ++
        "%s = %s;".format(name, value).iter ++
        "if(!%s) return false;".format(name).iter
      }

      def constructorInit : Iterator[String] = cfg.ifEnabled(Features.Constructors) {

        def lines(c : Constructor[_]) : Iterator[String] = {
          setAndCheckReturn("this->init%dConstructor".format(c.getParameterCount)) {
            "env->GetMethodID(this->clazz, \"<init>\", \"%s\")".format(c.jniSignature)
          }
        }

        cfg.constructors.toIterator.map(lines).flatten
      }

      def methodInit : Iterator[String] = cfg.ifEnabled(Features.Methods) {

        def lines(m : Method) : Iterator[String] = {

          def typ = if(m.isStatic) "Static" else ""

          setAndCheckReturn("this->%sMethod".format(m.getName)) {
              "env->Get%sMethodID(this->clazz, \"%s\", \"%s\")".format(typ, m.getName, m.jniSignature)
          }
        }

        cfg.methods.toIterator.map(lines).flatten
      }

      def fieldInit : Iterator[String] = cfg.ifEnabled(Features.Fields) {

        def lines(f : Field) : Iterator[String] = {
          setAndCheckReturn("this->%sField".format(f.getName)) {
            "env->GetFieldID(this->clazz, \"%s\", \"%s\")".format(f.getName, JNIMethod.getFieldType(f.getType))
          }
        }

        cfg.clazz.getFields.toIterator.map(lines).flatten
      }

      "bool %s::init(JNIEnv* env)".format(cfg.clazz.getSimpleName).iter ++ bracket {

        "auto clazzTemp = env->FindClass(\"%s\");".format(cfg.clazz.fqcn).iter ++
        "this->clazz = (jclass) env->NewGlobalRef(clazzTemp);".iter ++
        "env->DeleteLocalRef(clazzTemp);".iter ++
        constructorInit ++ methodInit ++ fieldInit ++
        space ++ "return true;".iter
      }
    }

    def cleanupImpl : Iterator[String] = {
      "void %s::cleanup(JNIEnv* env)".format(cfg.clazz.getSimpleName).iter ++ bracket {
        "env->DeleteGlobalRef(this->clazz);".iter
      }
    }

    def methodsImpls : Iterator[String] = cfg.ifEnabled(Features.Methods) {
      cfg.methods.toIterator.flatMap { m =>
        space ++ JNIMethod.getImpl(m)
      }
    }

    def constructorImpls : Iterator[String] = cfg.ifEnabled(Features.Constructors) {
      cfg.constructors.toIterator.flatMap { c =>
        space ++ JNIMethod.getConstructorImpl(c)
      }
    }

    def fieldGetterImpls : Iterator[String] = cfg.ifEnabled(Features.Fields) {
      cfg.fields.toIterator.flatMap { f =>
        space ++ JNIMethod.getFieldGetterImpl(f)
      }
    }

    commented(LicenseHeader()) ++ space ++
    "#include \"%s\"".format(headerFileName).iter ++ space ++
    namespace("jni") {
      namespace("cache") {
        initImpl ++ space ++ cleanupImpl ++ methodsImpls ++ constructorImpls ++ fieldGetterImpls
      }
    }
  }

}
