package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._
import FixedSizeField._
import com.automatak.render.dnp3.objects.VariationNames._
import com.automatak.render.dnp3.objects.generators.ConversionToFrozenCounter

// frozen counter events
object Group23 extends ObjectGroup {
  def objects = List(Group23Var0, Group23Var1, Group23Var2, Group23Var5, Group23Var6)
  def group: Byte = 23
  def desc: String = "Frozen Counter Event"
  def isEventGroup: Boolean = true
}

object Group23Var0 extends AnyVariation(Group23, 0)
object Group23Var1 extends FixedSize(Group23, 1, bit32WithFlag)(flags, count32) with ConversionToFrozenCounter
object Group23Var2 extends FixedSize(Group23, 2, bit16WithFlag)(flags, count16) with ConversionToFrozenCounter
object Group23Var5 extends FixedSize(Group23, 5, bit32WithFlagTime)(flags, count32, time48) with ConversionToFrozenCounter
object Group23Var6 extends FixedSize(Group23, 6, bit16WithFlagTime)(flags, count16, time48) with ConversionToFrozenCounter
