package com.automatak.render.dnp3.enums

import com.automatak.render._

object LogLevel {

  private val comments = List("Enumeration for log levels")

  def apply(): EnumModel = EnumModel("LogLevel", comments, EnumModel.UInt32, codes, None, Hex)

  private val codes = EnumValues.bitmask(levels)

  private def levels = List("Event", "Error", "Warning", "Info", "Interpret", "Comm", "Debug")


}



