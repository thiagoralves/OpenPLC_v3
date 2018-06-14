package com.automatak.render.dnp3.objects.groups

import com.automatak.render.dnp3.objects.generators.ConversionToBinaryCommandEvent
import com.automatak.render.dnp3.objects.{FixedSize, FixedSizeField, ObjectGroup}
import com.automatak.render.dnp3.objects.VariationNames._

import FixedSizeField._


object Group13 extends ObjectGroup {
  def objects = List(Group13Var1, Group13Var2)
  def group: Byte = 13
  def desc: String = "Binary Command Event"
  def isEventGroup: Boolean = true
}

object Group13Var1 extends FixedSize(Group13, 1, withoutTime)(flags) with ConversionToBinaryCommandEvent
object Group13Var2 extends FixedSize(Group13, 2, withTime)(flags, time48) with ConversionToBinaryCommandEvent