package com.automatak.dnp3.codegen

import java.lang.reflect.{Constructor, Method, Field}

object JNIMethod {

  val classOfBool = classOf[java.lang.Boolean]
  val classOfShort = classOf[java.lang.Short]
  val classOfInt = classOf[java.lang.Integer]
  val classOfLong = classOf[java.lang.Long]
  val classOfFloat = classOf[java.lang.Float]
  val classOfDouble = classOf[java.lang.Double]
  val classOfString = classOf[java.lang.String]

  def getType(clazz: Class[_]): String = clazz match {
    case `classOfBool` => "jboolean"
    case `classOfShort` => "jshort"
    case `classOfInt` => "jint"
    case `classOfLong` => "jlong"
    case `classOfFloat` => "jfloat"
    case `classOfDouble` => "jdouble"
    case `classOfString` => "jstring"
    case _ => {
      if(clazz.isPrimitive) {
        clazz.getTypeName match {
          case "void" => "void"
          case "boolean" => "jboolean"
          case "int" => "jint"
          case "byte" => "jbyte"
          case "long" => "jlong"
          case "float" => "jfloat"
          case "short" => "jshort"
          case "double" => "jdouble"
          case _ => throw new Exception("undefined primitive type: %s".format(clazz.getTypeName))
        }
      } else {
        "jobject"
      }

    }
  }

  def getFieldType(clazz: Class[_]): String = clazz match {

    case `classOfInt` => "I"
    case `classOfLong` => "J"
    case `classOfBool` => "Z"
    case `classOfDouble` => "D"
    case _ => {
      if(clazz.isPrimitive) {
        clazz.getTypeName match {
          case "boolean" => "Z"
          case "int" => "I"
          case "void" => "void"
          case "byte" => "B"
          case "long" => "J"
          case "short" => "S"
          case "float" => "F"
          case "double" => "D"
          case _ => throw new Exception("undefined primitive type: %s".format(clazz.getTypeName))
        }
      } else {
        clazz.fqcn
      }

    }
  }

  def getReturnType(clazz: Class[_]): String = clazz match {

    case `classOfInt` => "Int"
    case `classOfLong` => "Long"
    case `classOfBool` => "Boolean"
    case `classOfDouble` => "Double"
    case _ => {
      if(clazz.isPrimitive) {
        clazz.getTypeName match {
          case "boolean" => "Boolean"
          case "int" => "Int"
          case "long" => "Long"
          case "void" => "Void"
          case "byte" => "Byte"
          case "short" => "Short"
          case "float" => "Float"
          case "double" => "Double"
          case _ => throw new Exception("undefined primitive type: %s".format(clazz.getTypeName))
        }
      } else {
        "Object"
      }
    }
  }

  def getSignature(method: Method, className: Option[String] = None) : String = {

    def returnType = {
      val value = getType(method.getReturnType)
      if(value == "jobject") "LocalRef<jobject>" else value
    }

    def arguments = {
      if(method.getParameterCount == 0) "" else {
        ", " + method.getParameters.map(p => "%s %s".format(getType(p.getType), p.getName)).mkString(", ")
      }
    }

    def prefix = className.map(n => "%s::".format(n)).getOrElse("")

    def additionalArgs = if(method.isStatic) "" else ", jobject instance"

    "%s %s%s(JNIEnv* env%s%s)".format(returnType, prefix, method.getName, additionalArgs, arguments)
  }

  def getImpl(method: Method)(implicit i: Indentation) : Iterator[String] = {

    def returnPrefix : String = {

      if(!method.getReturnType.isPrimitive())
      {
        "return LocalRef<%s>(env, ".format(getType(method.getReturnType))
      }
      else
      {
        if(method.isVoid) "" else "return "
      }

    }

    def returnSuffix : String = {

      if(!method.getReturnType.isPrimitive())
      {
        ");"
      }
      else
      {
        ";"
      }

    }

    def args : String = if(method.getParameterCount == 0) "" else {
      ", " + method.getParameters.map(p => p.getName).mkString(", ")
    }

    def callMethod : String = {
      if(method.isStatic) {
        "%senv->CallStatic%sMethod(this->clazz, this->%sMethod%s)%s".format(
          returnPrefix,
          getReturnType(method.getReturnType),
          method.getName,
          args,
          returnSuffix
        )
      }
      else {
        "%senv->Call%sMethod(instance, this->%sMethod%s)%s".format(
          returnPrefix,
          getReturnType(method.getReturnType),
          method.getName,
          args,
          returnSuffix
        )
      }
    }

    JNIMethod.getSignature(method, Some(method.getDeclaringClass.getSimpleName)).iter ++ bracket {
      callMethod.iter
    }

  }

  def getConstructorSignature(constructor: Constructor[_], className: Option[String] = None) : String = {


    def arguments = constructor.getParameters.map(p => "%s %s".format(getType(p.getType), p.getName)).mkString(", ")

    if(arguments.isEmpty) {
      "LocalRef<jobject> %sinit%d(JNIEnv* env)".format(className.map(n => "%s::".format(n)).getOrElse(""), constructor.getParameterCount)
    }
    else {
      "LocalRef<jobject> %sinit%d(JNIEnv* env, %s)".format(className.map(n => "%s::".format(n)).getOrElse(""), constructor.getParameterCount, arguments)
    }

  }

  def getConstructorImpl(constructor: Constructor[_])(implicit i: Indentation) : Iterator[String] = {

    def args : String = if(constructor.getParameterCount == 0) "" else {
      ", " + constructor.getParameters.map(p => p.getName).mkString(", ")
    }

    JNIMethod.getConstructorSignature(constructor, Some(constructor.getDeclaringClass.getSimpleName)).iter ++ bracket {
      "return LocalRef<jobject>(env, env->NewObject(this->clazz, this->init%dConstructor%s));".format(
        constructor.getParameterCount,
        args
      ).iter
    }

  }

  def getFieldGetterImpl(f : Field)(implicit i: Indentation) : Iterator[String] = {

    def fieldType : String = getReturnType(f.getType)

    def cast : String = if(f.getType == classOf[String]) "(jstring) " else ""

    if(f.getType.isPrimitive) {
      "%s %s::get%s(JNIEnv* env, jobject instance)".format(getType(f.getType), f.getDeclaringClass.getSimpleName, f.getName).iter  ++ bracket {
        "return %senv->Get%sField(instance, this->%sField);".format(cast, fieldType, f.getName).iter
      }
    }
    else {
      "LocalRef<%s> %s::get%s(JNIEnv* env, jobject instance)".format(getType(f.getType), f.getDeclaringClass.getSimpleName, f.getName).iter  ++ bracket {
        "return LocalRef<%s>(env, %senv->Get%sField(instance, this->%sField));".format(getType(f.getType), cast, fieldType, f.getName).iter
      }
    }


  }
}
