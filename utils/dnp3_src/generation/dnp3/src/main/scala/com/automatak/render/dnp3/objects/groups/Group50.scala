package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects._

import FixedSizeField._
import com.automatak.render.dnp3.objects.generators.ConversionToTimeAndInterval

// absolute time
object Group50 extends ObjectGroup {
  def objects = List(Group50Var1, Group50Var4)
  def group: Byte = 50
  def desc: String = "Time and Date"
  def isEventGroup: Boolean = false
}

object Group50Var1 extends FixedSize(Group50, 1, "Absolute Time")(time48)

object Group50Var4 extends FixedSize(Group50, 4, "Indexed absolute time and long interval")(
  time48,
  FixedSizeField("interval", UInt32Field),
  FixedSizeField("units", UInt8Field)
) with ConversionToTimeAndInterval

