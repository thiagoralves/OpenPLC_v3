package com.automatak.render.dnp3.enums

import com.automatak.render._

object RestartMode {

  private val comments = List("Enumeration describing restart mode support of an outstation")

  def apply(): EnumModel = EnumModel("RestartMode", comments, EnumModel.UInt8, codes, None, Base10)

  private val codes = List(
    EnumValue("UNSUPPORTED", 0, "Device does not support restart"),
    EnumValue("SUPPORTED_DELAY_FINE", 1, "Supports restart, and time returned is a fine time delay"),
    EnumValue("SUPPORTED_DELAY_COARSE", 2, "Supports restart, and time returned is a coarse time delay")
  )

}



