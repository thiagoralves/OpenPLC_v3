package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects.{DefaultVariableSize, ObjectGroup}

object Group70 extends ObjectGroup {

  def objects = List(
    Group70Var1,
    Group70Var2,
    Group70Var3,
    Group70Var4,
    Group70Var5,
    Group70Var6,
    Group70Var7,
    Group70Var8
  )

  def group: Byte = 70
  def desc: String = "File-control"
  def isEventGroup: Boolean = false

}

object Group70Var1 extends DefaultVariableSize(Group70, 1, "File identifier")
object Group70Var2 extends DefaultVariableSize(Group70, 2, "Authentication")
object Group70Var3 extends DefaultVariableSize(Group70, 3, "File command")
object Group70Var4 extends DefaultVariableSize(Group70, 4, "File command status")
object Group70Var5 extends DefaultVariableSize(Group70, 5, "File transport")
object Group70Var6 extends DefaultVariableSize(Group70, 6, "File transport status")
object Group70Var7 extends DefaultVariableSize(Group70, 7, "File descriptor")
object Group70Var8 extends DefaultVariableSize(Group70, 8, "File specification string")

