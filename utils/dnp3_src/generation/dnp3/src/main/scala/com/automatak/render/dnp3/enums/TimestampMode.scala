package com.automatak.render.dnp3.enums

import com.automatak.render._

object TimestampMode {

  private val comments = List("Indicates the validity of timestamp values for an entire object header")

  val defaultValue = EnumValue("INVALID", 0, "Timestamp is not valid, ignore the value and use a local timestamp")

  def apply(): EnumModel = EnumModel("TimestampMode", comments, EnumModel.UInt8, codes, Some(defaultValue), Base10)

  private val codes = List(
    EnumValue("SYNCHRONIZED", 1, "The timestamp is UTC synchronized at the remote device"),
    EnumValue("UNSYNCHRONIZED", 2, "The device indicate the timestamp may be unsynchronized")
  )

}



