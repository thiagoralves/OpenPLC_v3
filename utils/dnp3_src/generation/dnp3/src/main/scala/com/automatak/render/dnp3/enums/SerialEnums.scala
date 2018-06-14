package com.automatak.render.dnp3.enums

import com.automatak.render._


object Parity {

  private val comments = List("Enumeration for setting serial port parity")

  def apply(): EnumModel = EnumModel("Parity", comments, EnumModel.UInt8, codes, Some(default), Base10)

  def default = EnumValue("None", 0)

  private val codes = List(
    EnumValue("Even", 1),
    EnumValue("Odd", 2)
  )

}

object FlowControl {

  private val comments = List("Enumeration for setting serial port flow control")

  def apply(): EnumModel = EnumModel("FlowControl", comments, EnumModel.UInt8, codes, Some(default), Base10)

  def default = EnumValue("None", 0)

  private val codes = List(
    EnumValue("Hardware", 1),
    EnumValue("XONXOFF", 2)
  )

}


object StopBits {

  private val comments = List("Enumeration for setting serial port stop bits")

  def apply(): EnumModel = EnumModel("StopBits", comments, EnumModel.UInt8, codes, Some(default), Base10)

  def default = EnumValue("None", 0)

  private val codes = List(
    EnumValue("One", 1),
    EnumValue("OnePointFive", 2),
    EnumValue("Two", 3)
  )

}
