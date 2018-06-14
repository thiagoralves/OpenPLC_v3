package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects.{SingleBitfield, ObjectGroup}
import com.automatak.render.dnp3.objects.VariationNames._

object Group80 extends ObjectGroup {
  def objects = List(Group80Var1)
  def group: Byte = 80
  def desc: String = "Internal Indications"
  def isEventGroup: Boolean = false
}

object Group80Var1 extends SingleBitfield(Group80, 1, packedFormat)

