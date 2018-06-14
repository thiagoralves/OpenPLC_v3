package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects.{FixedSize, FixedSizeField, ObjectGroup}

import FixedSizeField._

object Group52 extends ObjectGroup {
  def objects = List(Group52Var1, Group52Var2)
  def group: Byte = 52
  def desc: String = "Time Delay"
  def isEventGroup: Boolean = false
}

object Group52Var1 extends FixedSize(Group52, 1, "Coarse")(time16)
object Group52Var2 extends FixedSize(Group52, 2, "Fine")(time16)