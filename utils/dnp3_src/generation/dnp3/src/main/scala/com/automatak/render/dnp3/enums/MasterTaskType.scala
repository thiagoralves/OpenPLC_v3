package com.automatak.render.dnp3.enums

import com.automatak.render._


object MasterTaskType {

  private val comments = List("Enumeration of internal tasks")

  def apply(): EnumModel = EnumModel("MasterTaskType", comments, EnumModel.UInt8, codes, None, Base10)

  private val codes = EnumValues.from(
    List(
      "CLEAR_RESTART",
      "DISABLE_UNSOLICITED",
      "ASSIGN_CLASS",
      "STARTUP_INTEGRITY_POLL",
      "SERIAL_TIME_SYNC",
      "ENABLE_UNSOLICITED",
      "AUTO_EVENT_SCAN",
      "USER_TASK",
      "SET_SESSION_KEYS"
    )
  )

}



