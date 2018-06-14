package com.automatak.render.dnp3.enums

import com.automatak.render._

object RestartType {

  private val comments = List("Enumeration describing restart operation to perform on an outstation")

  def apply(): EnumModel = EnumModel("RestartType", comments, EnumModel.UInt8, codes, None, Base10)

  private val codes = List(
    EnumValue("COLD", 0, "Full reboot"),
    EnumValue("WARM", 1, "Warm reboot of process only")
  )

}



