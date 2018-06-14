package com.automatak.render.dnp3.enums

import com.automatak.render._


object KeyStatus {

  private val comments = List("Describes the status of the two session keys as known by the outstation")

  private val defaultValue = EnumValue("UNDEFINED", 0)

  def apply(): EnumModel = EnumModel("KeyStatus", comments, EnumModel.UInt8, codes, Some(defaultValue), Hex)

  private val codes = List(
    EnumValue("OK", 1, "Session keys are valid"),
    EnumValue("NOT_INIT", 2, "Sessions keys are not valid due to no init or interval expiration"),
    EnumValue("COMM_FAIL", 3, "Session keys are not valid due to communications failure"),
    EnumValue("AUTH_FAIL", 4, "Session keys are not valid due to an authentication failure")
  )

}



