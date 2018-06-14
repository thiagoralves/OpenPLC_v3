package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._
import com.automatak.render.dnp3.objects.VariationNames._

import FixedSizeField._
import com.automatak.render.dnp3.objects.generators.ConversionToBinaryOutputStatus

object Group11 extends ObjectGroup {
  def objects = List(Group11Var0, Group11Var1, Group11Var2)
  def group: Byte = 11
  def desc: String = "Binary Output Event"
  def isEventGroup: Boolean = true
}

object Group11Var0 extends AnyVariation(Group11, 0)
object Group11Var1 extends FixedSize(Group11, 1, outputStatusWithoutTime)(flags) with ConversionToBinaryOutputStatus
object Group11Var2 extends FixedSize(Group11, 2, outputStatusWithTime)(flags, time48) with ConversionToBinaryOutputStatus
