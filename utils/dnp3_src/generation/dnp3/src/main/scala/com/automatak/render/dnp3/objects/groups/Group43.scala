package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects.generators.ConversionToAnalogCommandEvent
import com.automatak.render.dnp3.objects.{FixedSize, FixedSizeField, ObjectGroup}

import FixedSizeField._
import com.automatak.render.dnp3.objects.VariationNames._

//analog output events
object Group43 extends ObjectGroup {
  def objects = List(Group43Var1, Group43Var2, Group43Var3, Group43Var4, Group43Var5, Group43Var6, Group43Var7, Group43Var8)
  def group: Byte = 43
  def desc: String = "Analog Command Event"
  def isEventGroup: Boolean = true
}

object Group43Var1 extends FixedSize(Group43, 1, bit32)(commandStatus, value32) with ConversionToAnalogCommandEvent
object Group43Var2 extends FixedSize(Group43, 2, bit16)(commandStatus, value16) with ConversionToAnalogCommandEvent
object Group43Var3 extends FixedSize(Group43, 3, bit32WithTime)(commandStatus, value32, time48) with ConversionToAnalogCommandEvent
object Group43Var4 extends FixedSize(Group43, 4, bit16WithTime)(commandStatus, value16, time48) with ConversionToAnalogCommandEvent
object Group43Var5 extends FixedSize(Group43, 5, singlePrecision)(commandStatus, float32) with ConversionToAnalogCommandEvent
object Group43Var6 extends FixedSize(Group43, 6, doublePrecision)(commandStatus, float64) with ConversionToAnalogCommandEvent
object Group43Var7 extends FixedSize(Group43, 7, singlePrecisionWithTime)(commandStatus, float32, time48) with ConversionToAnalogCommandEvent
object Group43Var8 extends FixedSize(Group43, 8, doublePrecisionWithTime)(commandStatus, float64, time48) with ConversionToAnalogCommandEvent