package com.automatak.render.dnp3.enums

import com.automatak.render._

object DoubleBit {

  private val comments = List("Enumeration for possible states of a double bit value")

  def apply(): EnumModel = EnumModel("DoubleBit", comments, EnumModel.UInt8, codes, Some(defaultValue), Hex)

  def defaultValue = EnumValue("INDETERMINATE", 3, "Abnormal or custom condition")

  private val codes = List(
    EnumValue("INTERMEDIATE", 0, "Transitioning between end conditions"),
    EnumValue("DETERMINED_OFF", 1, "End condition, determined to be OFF"),
    EnumValue("DETERMINED_ON", 2, "End condition, determined to be ON")
  )

}



