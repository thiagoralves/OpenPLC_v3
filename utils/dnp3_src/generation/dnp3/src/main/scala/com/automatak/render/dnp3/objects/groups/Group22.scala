package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._

import FixedSizeField._
import com.automatak.render.dnp3.objects.VariationNames._
import com.automatak.render.dnp3.objects.generators.ConversionToCounter

// counter event
object Group22 extends ObjectGroup {
  def objects = List(Group22Var0, Group22Var1, Group22Var2, Group22Var5, Group22Var6)
  def group: Byte = 22
  def desc: String = "Counter Event"
  def isEventGroup: Boolean = true
}

object Group22Var0 extends AnyVariation(Group22, 0)
object Group22Var1 extends FixedSize(Group22, 1, bit32WithFlag)(flags, count32) with ConversionToCounter
object Group22Var2 extends FixedSize(Group22, 2, bit16WithFlag)(flags, count16) with ConversionToCounter
object Group22Var5 extends FixedSize(Group22, 5, bit32WithFlagTime)(flags, count32, time48) with ConversionToCounter
object Group22Var6 extends FixedSize(Group22, 6, bit16WithFlagTime)(flags, count16, time48) with ConversionToCounter
