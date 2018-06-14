package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._

import FixedSizeField._
import com.automatak.render.dnp3.objects.VariationNames._
import com.automatak.render.dnp3.objects.generators.ConversionToAnalog

// analog input event
object Group32 extends ObjectGroup {
  def objects = List(Group32Var0, Group32Var1, Group32Var2, Group32Var3, Group32Var4, Group32Var5, Group32Var6, Group32Var7, Group32Var8)
  def group: Byte = 32
  def desc: String = "Analog Input Event"
  def isEventGroup: Boolean = true
}

object Group32Var0 extends AnyVariation(Group32, 0)
object Group32Var1 extends FixedSize(Group32, 1, bit32WithFlag)(flags, value32) with ConversionToAnalog
object Group32Var2 extends FixedSize(Group32, 2, bit16WithFlag)(flags, value16) with ConversionToAnalog
object Group32Var3 extends FixedSize(Group32, 3, bit32WithFlagTime)(flags, value32, time48) with ConversionToAnalog
object Group32Var4 extends FixedSize(Group32, 4, bit16WithFlagTime)(flags, value16, time48) with ConversionToAnalog
object Group32Var5 extends FixedSize(Group32, 5, singlePrecisionWithFlag)(flags, float32) with ConversionToAnalog
object Group32Var6 extends FixedSize(Group32, 6, doublePrecisionWithFlag)(flags, float64) with ConversionToAnalog
object Group32Var7 extends FixedSize(Group32, 7, singlePrecisionWithFlagTime)(flags, float32, time48) with ConversionToAnalog
object Group32Var8 extends FixedSize(Group32, 8, doublePrecisionWithFlagTime)(flags, float64, time48) with ConversionToAnalog
