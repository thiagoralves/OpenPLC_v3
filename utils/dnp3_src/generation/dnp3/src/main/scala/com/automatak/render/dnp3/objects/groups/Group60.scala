package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects.{ClassData, AnyVariation, ObjectGroup}

object Group60 extends ObjectGroup {
  def objects = List(Group60Var1, Group60Var2, Group60Var3, Group60Var4)
  def group: Byte = 60
  def desc: String = "Class Data"
  def isEventGroup: Boolean = false
}

object Group60Var1 extends ClassData(Group60, 1, "Class 0")
object Group60Var2 extends ClassData(Group60, 2, "Class 1")
object Group60Var3 extends ClassData(Group60, 3, "Class 2")
object Group60Var4 extends ClassData(Group60, 4, "Class 3")
