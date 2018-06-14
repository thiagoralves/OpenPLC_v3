package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._

import com.automatak.render.dnp3.objects.FixedSizeField._
import com.automatak.render.dnp3.objects.generators.ConversionToSecurityStat

object Group122 extends ObjectGroup {
  def objects = List(Group122Var0, Group122Var1, Group122Var2)
  def group: Byte = 122
  def desc: String = "Security Statistic event"
  def isEventGroup: Boolean = true
}

object Group122Var0 extends AnyVariation(Group122, 0)
object Group122Var1 extends FixedSize(Group122, 1, VariationNames.bit32WithFlag)(flags, assocId, count32) with ConversionToSecurityStat
object Group122Var2 extends FixedSize(Group122, 2, VariationNames.bit32WithFlagTime)(flags, assocId, count32, time48) with ConversionToSecurityStat
