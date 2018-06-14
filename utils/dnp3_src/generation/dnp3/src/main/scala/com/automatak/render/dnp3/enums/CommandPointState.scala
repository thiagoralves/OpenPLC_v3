package com.automatak.render.dnp3.enums

import com.automatak.render._


object CommandPointState {

  private val comments = List(
    "List the various states that an individual command object can be in after an SBO or direct operate request"
  )

  def apply(): EnumModel = EnumModel("CommandPointState", comments, EnumModel.UInt8, EnumValues.fromPairs(codes), None, Base10)

  private val codes = List(
    "INIT" -> "No corresponding response was ever received for this command point",
    "SELECT_SUCCESS" -> "A response to a select request was received and matched, but the operate did not complete",
    "SELECT_MISMATCH" -> "A response to a select operation did not contain the same value that was sent",
    "SELECT_FAIL" -> "A response to a select operation contained a command status other than success",
    "OPERATE_FAIL" -> "A response to an operate or direct operate did not match the request",
    "SUCCESS" -> "A matching response was received to the operate"
  )


}



