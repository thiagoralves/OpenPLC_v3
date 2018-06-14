/**
 * Copyright 2013 Automatak, LLC
 *
 * Licensed to Automatak, LLC (www.automatak.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership. Automatak, LLC
 * licenses this file to you under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
package com.automatak.render

package object cpp {

  private val quote : String = "\""

  def getEnumType(typ: EnumModel.Type): String = typ match {
    case EnumModel.UInt8 => "uint8_t"
    case EnumModel.UInt16 => "uint16_t"
    case EnumModel.UInt32 => "uint32_t"
  }

  def quoted(s: String): String = List(quote, s, quote).mkString

  def bracketed(s: String): String = List("<", s, ">").mkString

  def concat(a: Iterator[String], b: Iterator[Option[String]]): Iterator[String] = {

    new Iterator[String] {
      def hasNext = a.hasNext && b.hasNext

      def next(): String = {
        val d = a.next()
        b.next() match {
          case Some(c) => d + c
          case None => d
        }
      }
    }
  }

  def stdString: String = "std::string"
  def cString: String = "char const*"

  def namespace(ns: String)(internals: Iterator[String]): Iterator[String] = {

    Iterator.apply(List("namespace",ns,"{").spaced) ++
    space ++
    internals ++
    space ++
    Iterator.apply("}")

  }

  def comment(comment: String)(implicit i: Indentation): Iterator[String] = Iterator("// %s".format(comment))

  def privateSection(inner: => Iterator[String]): Iterator[String] = Iterator("private:") ++ space ++ inner

  def classPublic(inner: => Iterator[String])(implicit i: Indentation): Iterator[String] = Iterator("public:","") ++ inner

  def classPrivate(inner: => Iterator[String])(implicit i: Indentation): Iterator[String] = Iterator("private:","") ++ inner

  def struct(name: String, base: Option[String] = None)(inner: => Iterator[String])(implicit i: Indentation): Iterator[String] = {
    def header = base match {
      case Some(b) => Iterator("struct " + name + " : public " + b)
      case None => Iterator("struct " + name)
    }
    header ++
      bracketSemiColon {
        inner
      }
  }

  def clazz(name: String, bases: List[String] = Nil)(inner: => Iterator[String])(implicit i: Indentation): Iterator[String] = {
    def header = "class " + name
    def headerWithBases = Iterator(if(bases.isEmpty) header else header + ": " + bases.map(b => "public " + b).mkString(","))
    headerWithBases ++
    bracketSemiColon {
      inner
    }
  }

  private def opendnp3Pattern(name: String) = "OPENDNP3_"+ name.toUpperCase + "_H"

  def includeGuards(name: String)(internals: => Iterator[String]): Iterator[String] = {

    Iterator("#ifndef %s".format(opendnp3Pattern(name))) ++
    Iterator("#define %s".format(opendnp3Pattern(name))) ++
    space ++
    internals ++
    space ++
    Iterator("#endif")

  }

  def includeGuardsRaw(name: String)(internals: => Iterator[String]): Iterator[String] = {

    Iterator("#ifndef %s".format(name)) ++
      Iterator("#define %s".format(name)) ++
      space ++
      internals ++
      space ++
      Iterator("#endif")

  }

  def staticCast(typ: EnumModel.Type)(arg: String): String = staticCast(getEnumType(typ))(arg)

  def staticCast(typ: String)(arg: String): String = List("static_cast<",typ,">(",arg,")").mkString

  def include(s: String): String = "#include " + s

  def cstdint : Iterator[String] = Iterator.apply(include("<cstdint>"))

  def string : Iterator[String] = Iterator.apply(include("<string>"))

}
