package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._

import com.automatak.render.dnp3.objects.FixedSizeField._
import com.automatak.render.dnp3.objects.generators.ConversionToSecurityStat

object Group121 extends ObjectGroup {
  def objects = List(Group121Var0, Group121Var1)
  def group: Byte = 121
  def desc: String = "Security statistic"
  def isEventGroup: Boolean = false
}

object Group121Var0 extends AnyVariation(Group121, 0)
object Group121Var1 extends FixedSize(Group121, 1, VariationNames.bit32WithFlag)(flags, assocId, count32) with ConversionToSecurityStat

