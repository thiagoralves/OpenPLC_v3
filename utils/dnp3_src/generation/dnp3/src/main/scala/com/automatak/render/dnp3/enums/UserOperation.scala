package com.automatak.render.dnp3.enums

import com.automatak.render._


object UserOperation {

  private val comments = List("Enumerates possible remote operations on users")
  private val defaultValue = EnumValue("OP_UNDEFINED", 0xFF)

  def apply(): EnumModel = EnumModel("UserOperation", comments, EnumModel.UInt8, codes, Some(defaultValue), Hex)

  private val codes = List(
    EnumValue("OP_ADD", 1),
    EnumValue("OP_DELETE", 2),
    EnumValue("OP_CHANGE", 3)
  )

}



