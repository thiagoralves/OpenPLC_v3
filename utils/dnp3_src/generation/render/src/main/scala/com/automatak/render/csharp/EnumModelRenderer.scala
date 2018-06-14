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
package com.automatak.render.csharp

import com.automatak.render._

object EnumModelRenderer extends ModelRenderer[EnumModel] {

  def render(enum: EnumModel)(implicit indent: Indentation) : Iterator[String] = {

    def pair(ir: IntRender)(ev: EnumValue): String = List(ev.name, "=", ir(ev.value)).spaced

    def getEnumType(typ: EnumModel.Type): String = typ match {
      case EnumModel.UInt8 => "byte"
      case EnumModel.UInt16 => "ushort"
      case EnumModel.UInt32 => "uint"
    }

    def header: Iterator[String] = Iterator(List("public", "enum", enum.name, ":", getEnumType(enum.enumType)).spaced)

    def summary(comments: Iterator[String]): Iterator[String] = {
      Iterator("/// <summary>") ++ comments.map("/// " + _) ++ Iterator("/// </summary>")
    }

    def comments: Iterator[Option[Iterator[String]]] = enum.allValues.map(ev => ev.comment.map(c => summary(Iterator(c)))).iterator

    def definitions : Iterator[String] = commaDelimited(enum.allValues.map(pair(enum.render)).iterator)

    summary(enum.comments.toIterator) ++
    header ++ bracket {
      merge(comments, definitions)
    }

  }

}
