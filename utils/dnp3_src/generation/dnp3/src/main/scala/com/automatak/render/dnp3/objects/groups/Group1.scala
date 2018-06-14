package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._
import com.automatak.render.dnp3.objects.VariationNames._
import com.automatak.render.dnp3.objects.generators.ConversionToBinary

object Group1 extends ObjectGroup {
  def objects = List(Group1Var0, Group1Var1, Group1Var2)
  def desc: String = "Binary Input"
  def group: Byte = 1
  def isEventGroup: Boolean = false
}

object Group1Var0 extends AnyVariation(Group1, 0)

object Group1Var1 extends SingleBitfield(Group1, 1, packedFormat)

object Group1Var2 extends FixedSize(Group1, 2, withFlags)(FixedSizeField.flags) with ConversionToBinary

