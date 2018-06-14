package com.automatak.render.dnp3.enums

import com.automatak.render._


object StaticTypeBitmask {

  private val comments = List(
    "Bitmask values for all the static types"
  )

  def apply(): EnumModel = EnumModel("StaticTypeBitmask", comments, EnumModel.UInt16, EnumModel.BitfieldValues(names), None, Hex)

  private val names = List(
    "BinaryInput",
    "DoubleBinaryInput",
    "Counter",
    "FrozenCounter",
    "AnalogInput",
    "BinaryOutputStatus",
    "AnalogOutputStatus",
    "TimeAndInterval"
  )

}



