package com.automatak.render.dnp3.enums

import com.automatak.render._

object LinkStatus {

  private val comments = List("Enumeration for reset/unreset states of a link layer")

  def apply(): EnumModel = EnumModel("LinkStatus", comments, EnumModel.UInt8, codes, None, Base10)

  private val codes = List(
    EnumValue("UNRESET", 0, "DOWN"),
    EnumValue("RESET", 1, "UP")
  )

}



