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
package com.automatak.render.cc

import com.automatak.render._
import com.automatak.render.cpp._

object EnumModelRenderer  {

  def render(enum: EnumModel, rename: Option[String], prefix: String)(implicit indent: Indentation) : Iterator[String] = {

    def outputName = rename.getOrElse(enum.name)

    def pair(ir: IntRender)(ev: EnumValue): String = List(prefix+ev.name, "=", ir(ev.value)).spaced

    def header: Iterator[String] = Iterator("typedef enum %s".format(outputName))

    def comments: Iterator[Option[Iterator[String]]] = enum.allValues.map(ev => ev.comment.map(c => Iterator("/// " + c))).iterator

    def definitions : Iterator[String] = commaDelimited(enum.allValues.map(pair(enum.render)).iterator)

    def summary = if(enum.comments.isEmpty) Iterator.empty else {
      Iterator("/**") ++ indent(enum.comments.toIterator) ++ Iterator("*/")
    }

    summary ++
    header ++ bracketSemiColon(outputName) {
      merge(comments, definitions)
    }

  }

}
