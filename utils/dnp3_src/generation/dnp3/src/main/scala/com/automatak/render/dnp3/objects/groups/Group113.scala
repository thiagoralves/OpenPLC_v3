package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects.{SizedByVariation, ObjectGroup}

object Group113 extends ObjectGroup {
  def objects = List(Group113AnyVar)
  def group: Byte = 113
  def desc: String = "Virtual Terminal Event Data"
  def isEventGroup: Boolean = true
}

object Group113AnyVar extends SizedByVariation(Group113, 0)