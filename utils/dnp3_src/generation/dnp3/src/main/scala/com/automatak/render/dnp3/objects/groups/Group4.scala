package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects.generators.ConversionToDoubleBitBinary
import com.automatak.render.dnp3.objects.{AnyVariation, FixedSize, ObjectGroup}

import com.automatak.render.dnp3.objects.FixedSizeField._
import com.automatak.render.dnp3.objects.VariationNames._

object Group4 extends ObjectGroup {
  def objects = List(Group4Var0, Group4Var1, Group4Var2, Group4Var3)
  def group: Byte = 4
  def desc: String = "Double-bit Binary Input Event"
  def isEventGroup: Boolean = true
}

object Group4Var0 extends AnyVariation(Group4, 0)
object Group4Var1 extends FixedSize(Group4, 1, withoutTime)(flags) with ConversionToDoubleBitBinary
object Group4Var2 extends FixedSize(Group4, 2, withAbsoluteTime)(flags, time48) with ConversionToDoubleBitBinary
object Group4Var3 extends FixedSize(Group4, 3, withRelativeTime)(flags, time16) with ConversionToDoubleBitBinary