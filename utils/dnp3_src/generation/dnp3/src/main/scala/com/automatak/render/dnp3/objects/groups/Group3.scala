package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._

import FixedSizeField._
import com.automatak.render.dnp3.objects.VariationNames._
import com.automatak.render.dnp3.objects.generators.ConversionToDoubleBitBinary

object Group3 extends ObjectGroup {
  def objects = List(Group3Var0, Group3Var1, Group3Var2)
  def group: Byte = 3
  def desc: String = "Double-bit Binary Input"
  def isEventGroup: Boolean = false
}

object Group3Var0 extends AnyVariation(Group3, 0)
object Group3Var1 extends DoubleBitfield(Group3, 1, packedFormat)
object Group3Var2 extends FixedSize(Group3, 2, withFlags)(flags) with ConversionToDoubleBitBinary
