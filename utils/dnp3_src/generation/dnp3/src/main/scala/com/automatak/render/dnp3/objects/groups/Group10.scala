package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._
import com.automatak.render.dnp3.objects.VariationNames._

import FixedSizeField._
import com.automatak.render.dnp3.objects.generators.ConversionToBinaryOutputStatus

object Group10 extends ObjectGroup {
  def objects = List(Group10Var0, Group10Var1, Group10Var2)
  def desc: String = "Binary Output"
  def group: Byte = 10
  def isEventGroup: Boolean = false
}

object Group10Var0 extends AnyVariation(Group10, 0)

object Group10Var1 extends SingleBitfield(Group10, 1, packedFormat)

object Group10Var2 extends FixedSize(Group10, 2, outputStatusWithFlags)(flags) with ConversionToBinaryOutputStatus

