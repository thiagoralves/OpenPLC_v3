package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects.{SizedByVariation, ObjectGroup}

object Group110 extends ObjectGroup {
  def objects = List(Group110AnyVar)
  def group: Byte = 110
  def desc: String = "Octet String"
  def isEventGroup: Boolean = false
}

object Group110AnyVar extends SizedByVariation(Group110, 0)
