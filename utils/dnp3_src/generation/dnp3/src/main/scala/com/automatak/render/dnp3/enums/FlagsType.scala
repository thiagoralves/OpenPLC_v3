package com.automatak.render.dnp3.enums

import com.automatak.render._


object FlagsType {

  private val comments = List("enumerates all types that have flags")

  def apply(): EnumModel = EnumModel("FlagsType", comments, EnumModel.UInt8, codes, Some(default), Hex)

  def default = EnumValue("BinaryInput", 0)

  private val codes = List(
    EnumValue("DoubleBinaryInput", 1),
    EnumValue("Counter", 2),
    EnumValue("FrozenCounter", 3),
    EnumValue("AnalogInput", 4),
    EnumValue("BinaryOutputStatus", 5),
    EnumValue("AnalogOutputStatus", 6)
  )

}



