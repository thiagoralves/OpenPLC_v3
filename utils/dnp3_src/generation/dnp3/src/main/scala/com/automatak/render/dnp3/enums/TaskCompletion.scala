package com.automatak.render.dnp3.enums

import com.automatak.render._


object TaskCompletion {

  private val comments = List("Enum that describes if a master task succeeded or failed")

  private val defaultValue = EnumValue("FAILURE_NO_COMMS", 255, "There is no communication with the outstation, so the task was not attempted")

  def apply(): EnumModel = EnumModel("TaskCompletion", comments, EnumModel.UInt8, codes, Some(defaultValue), Base10)

  private val codes = List(
       EnumValue("SUCCESS", 0, "A valid response was received from the outstation"),
       EnumValue("FAILURE_BAD_RESPONSE", 1, "A response was received from the outstation, but it was not valid"),
       EnumValue("FAILURE_RESPONSE_TIMEOUT", 2, "The task request did not receive a response within the timeout"),
       EnumValue("FAILURE_START_TIMEOUT", 3, "The start timeout expired before the task could begin running"),
       EnumValue("FAILURE_NO_USER", 4, "The task could not run because the specified user was not defined on the master (SA only)"),
       EnumValue("FAILURE_INTERNAL_ERROR", 5, "The task failed because of some unexpected internal issue like bad configuration data"),
       EnumValue("FAILURE_BAD_AUTHENTICATION", 6, "The outstation rejected the operation do to a lack of valid session keys or the user not existing on the outstation (SA only)"),
       EnumValue("FAILURE_NOT_AUTHORIZED", 7, "The outstation rejected the operation because the specified user is not authorized for the request that was made (SA only)")
  )

}



