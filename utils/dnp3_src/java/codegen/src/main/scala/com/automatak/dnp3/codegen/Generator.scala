package com.automatak.dnp3.codegen

import java.nio.file.FileSystems

object Generator {

  val javaJNIPath = FileSystems.getDefault.getPath("./cpp/jni/");

  def main(args: Array[String]): Unit = {

    implicit val indent = CppIndentation

    Classes.all.foreach { c =>

      val gen = JNIClassGenerator(c)

      writeTo(javaJNIPath.resolve(gen.headerFileName))(gen.header)
      writeTo(javaJNIPath.resolve(gen.implFileName))(gen.impl)
    }

    val staticClass = JCacheGenerator(Classes.all)

    writeTo(javaJNIPath.resolve(staticClass.headerFileName))(staticClass.header)
    writeTo(javaJNIPath.resolve(staticClass.implFileName))(staticClass.impl)

  }



}
