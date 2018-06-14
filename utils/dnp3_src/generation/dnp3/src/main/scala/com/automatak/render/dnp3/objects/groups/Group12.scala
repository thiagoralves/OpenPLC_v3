package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._
import com.automatak.render.dnp3.objects.VariationNames._

import FixedSizeField._
import com.automatak.render.dnp3.objects.generators.ConversionToCROB

object Group12 extends ObjectGroup {
  def objects = List(Group12Var0, Group12Var1)
  def group: Byte = 12
  def desc: String = "Binary Command"
  def isEventGroup: Boolean = false
}

object Group12Var0 extends AnyVariation(Group12, 0)

object Group12Var1 extends FixedSize(Group12, 1, crob)(
  FixedSizeField("code", UInt8Field),
  FixedSizeField("count", UInt8Field),
  FixedSizeField("onTime", UInt32Field),
  FixedSizeField("offTime", UInt32Field),
  commandStatus
) with ConversionToCROB
