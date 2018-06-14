package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects.{SizedByVariation, ObjectGroup}

object Group111 extends ObjectGroup {
  def objects = List(Group111AnyVar)
  def group: Byte = 111
  def desc: String = "Octet String Event"
  def isEventGroup: Boolean = false
}

object Group111AnyVar extends SizedByVariation(Group111, 0)