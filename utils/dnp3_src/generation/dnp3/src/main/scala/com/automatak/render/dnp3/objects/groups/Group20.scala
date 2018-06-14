package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._
import com.automatak.render.dnp3.objects.VariationNames._
import FixedSizeField._
import com.automatak.render.dnp3.objects.generators.ConversionToCounter

// counters
object Group20 extends ObjectGroup {
  def objects = List(Group20Var0, Group20Var1, Group20Var2, Group20Var5, Group20Var6)
  def group: Byte = 20
  def desc: String = "Counter"
  def isEventGroup: Boolean = false
}

object Group20Var0 extends AnyVariation(Group20, 0)
object Group20Var1 extends FixedSize(Group20, 1, bit32WithFlag)(flags, count32) with ConversionToCounter
object Group20Var2 extends FixedSize(Group20, 2, bit16WithFlag)(flags, count16) with ConversionToCounter
object Group20Var5 extends FixedSize(Group20, 5, bit32WithoutFlag)(count32) with ConversionToCounter
object Group20Var6 extends FixedSize(Group20, 6, bit16WithoutFlag)(count16) with ConversionToCounter


