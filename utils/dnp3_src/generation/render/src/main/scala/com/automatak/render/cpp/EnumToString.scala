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
package com.automatak.render.cpp

import com.automatak.render._

object EnumToString extends HeaderImplModelRender[EnumModel] {

  def impl: ModelRenderer[EnumModel]  = ImplRender
  def header: ModelRenderer[EnumModel]  = HeaderRender

  private def signature(name: String) : String = List(cString, List(name,"ToString(", name," arg)").mkString).mkString(" ")

  private object HeaderRender extends ModelRenderer[EnumModel] {
    def render(em: EnumModel)(implicit indent: Indentation) : Iterator[String] = Iterator(signature(em.name)+";")
  }

  private object ImplRender extends ModelRenderer[EnumModel] {

    def render(em: EnumModel)(implicit indent: Indentation) : Iterator[String] = {

      def header = Iterator(signature(em.name))
      def smr = new SwitchModelRenderer[EnumValue](ev => em.qualified(ev))(ev => quoted(ev.displayName))
      def getDefault : EnumValue = em.defaultValue.getOrElse(EnumValue("UNDEFINED",0))
      def switch = smr.render(em.nonDefaultValues, getDefault)

      header ++ bracket {
        switch
      }
    }
  }
}


