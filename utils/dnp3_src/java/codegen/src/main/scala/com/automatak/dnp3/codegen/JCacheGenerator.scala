package com.automatak.dnp3.codegen

case class JCacheGenerator(classes: List[ClassConfig]) {

  def headerFileName : String = "JCache.h"

  def implFileName : String = "JCache.cpp"

  def header(implicit indent: Indentation): Iterator[String] = {

    def classIncludes : Iterator[String] = classes.map(c => "#include \"JNI%s.h\"".format(c.clazz.getSimpleName)).toIterator

    def instances : Iterator[String] = classes.map(c => "static cache::%s %s;".format(c.clazz.getSimpleName, c.clazz.getSimpleName)).toIterator

    commented(LicenseHeader()) ++ space ++
      includeGuards("OPENDNP3_JNITYPES") {
        "#include \"openpal/util/Uncopyable.h\"".iter ++ space ++
        classIncludes ++ space ++
          namespace("jni") {
            structDef("JCache: private openpal::StaticOnly") {
              "static bool init(JNIEnv* env);".iter ++
                "static void cleanup(JNIEnv* env);".iter ++
                space ++
                instances
            }
          }
      }
  }

  def impl(implicit indent: Indentation): Iterator[String] = {

    def initAll : Iterator[String] = {
      val entries = classes.map {
        c => "%s.init(env)".format(c.clazz.getSimpleName)
      }
      
      ("return " + entries.head).iter ++ entries.tail.map(s => "&& " + s).iterator ++ ";".iter
    }

    def jcacheInit = {
      "bool JCache::init(JNIEnv* env)".iter ++ bracket {
        initAll
      }
    }

    def jcacheCleanup = {
      "void JCache::cleanup(JNIEnv* env)".iter ++ bracket {
        classes.iterator.flatMap { c =>
          "%s.cleanup(env);".format(c.clazz.getSimpleName).iter
        }
      }
    }

    def staticInitializers : Iterator[String] = {
      classes.iterator.flatMap { c =>
        "cache::%s JCache::%s;".format(c.clazz.getSimpleName, c.clazz.getSimpleName).iter
      }
    }

    commented(LicenseHeader()) ++ space ++
      "#include \"JCache.h\"".iter ++ space ++
      namespace("jni") {
        staticInitializers ++ space ++ jcacheInit ++ space ++ jcacheCleanup
      }
  }

}
