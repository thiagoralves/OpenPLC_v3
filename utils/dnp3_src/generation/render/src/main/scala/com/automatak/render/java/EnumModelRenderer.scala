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
package com.automatak.render.java

import com.automatak.render._
import com.automatak.render.cpp.SwitchModelRenderer

object EnumModelRenderer extends ModelRenderer[EnumModel] {

  def render(enum: EnumModel)(implicit indent: Indentation) : Iterator[String] = {

    def noCast(ir: IntRender)(ev: EnumValue): String = ev.name + "(" + ir(ev.value) + ")"

    def getEnumType(typ: EnumModel.Type): String = typ match {
      case EnumModel.UInt8 => "int"
      case EnumModel.UInt16 => "int"
      case EnumModel.UInt32 => "int"
    }

    def header: Iterator[String] = Iterator(List("public", "enum", enum.name).spaced)

    def summary(comments: Iterator[String]): Iterator[String] = {
      Iterator("/**") ++ comments.map("* " + _) ++ Iterator("*/")
    }

    def comments: Iterator[Option[Iterator[String]]] = enum.allValues.map(ev => ev.comment.map(c => summary(Iterator(c)))).iterator

    def definitions : Iterator[String] = commaDelimitedWithSemicolon(enum.allValues.map(noCast(enum.render)).iterator)

    def id = Iterator(List("private final", getEnumType(enum.enumType),"id;").spaced)

    def constructor = Iterator(List(enum.name, "(", getEnumType(enum.enumType), " id)").mkString) ++ bracket {
        Iterator("this.id = id;")
    }

    def create = Iterator("public static %s create(int value)".format(enum.name)) ++ bracket {
      Iterator("return new %s(value);".format(enum.name))
    }

    def toType = Iterator(List("public ", getEnumType(enum.enumType), " toType()").mkString) ++ bracket {
      Iterator("return id;")
    }

    def fromType(inner: => Iterator[String]) = Iterator(List("public static " +  enum.name + " fromType(", getEnumType(enum.enumType), " arg)").mkString) ++ bracket {
      inner
    }

    def switch = new SwitchModelRenderer[EnumValue](v => enum.render(v.value))(v => v.name).render(enum.nonDefaultValues, enum.defaultOrHead)

    def fromTypeLines : Iterator[String] = {
      space ++ fromType {
          switch
      }
    }

    summary(enum.comments.toIterator) ++
    header ++ bracket {
      merge(comments, definitions) ++ space ++
      id ++ space ++  toType ++
        space ++ constructor ++
        fromTypeLines
    }

  }

}
