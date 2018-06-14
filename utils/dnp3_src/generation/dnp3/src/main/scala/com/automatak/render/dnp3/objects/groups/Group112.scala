package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects.{SizedByVariation, ObjectGroup}

object Group112 extends ObjectGroup {
  def objects = List(Group112AnyVar)
  def group: Byte = 112
  def desc: String = "Virtual Terminal Output Block"
  def isEventGroup: Boolean = false
}

object Group112AnyVar extends SizedByVariation(Group112, 0)