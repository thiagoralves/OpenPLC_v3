package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._

import FixedSizeField._
import com.automatak.render.dnp3.objects.VariationNames._
import com.automatak.render.dnp3.objects.generators.ConversionToBinary

object Group2 extends ObjectGroup {
  val objects = List(Group2Var0, Group2Var1, Group2Var2, Group2Var3)
  def group: Byte = 2
  def desc: String = "Binary Input Event"
  def isEventGroup: Boolean = true
}

object Group2Var0 extends AnyVariation(Group2, 0)
object Group2Var1 extends FixedSize(Group2, 1, withoutTime)(flags) with ConversionToBinary
object Group2Var2 extends FixedSize(Group2, 2, withAbsoluteTime)(flags, time48) with ConversionToBinary
object Group2Var3 extends FixedSize(Group2, 3, withRelativeTime)(flags, time16) with ConversionToBinary