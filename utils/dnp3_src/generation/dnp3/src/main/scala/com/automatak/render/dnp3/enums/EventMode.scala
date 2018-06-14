package com.automatak.render.dnp3.enums

import com.automatak.render._


object EventMode {

  private val comments = List(
    "Describes how a transaction behaves with respect to event generation"
  )

  def apply(): EnumModel = EnumModel("EventMode", comments, EnumModel.UInt8, codes, None, Hex)

  private val codes = List(
    EnumValue("Detect", 0, "Detect events using the specific mechanism for that type"),
    EnumValue("Force", 1, "Force the creation of an event bypassing detection mechanism"),
    EnumValue("Suppress", 2, "Never produce an event regardless of changes")
  )

}



