package com.automatak.render.dnp3.enums

import com.automatak.render.{Hex, EnumModel, EnumValue}


object QualifierCode {

  private val comments = List("Object header range/prefix as a single enumeration")
  private val defaultValue = EnumValue("UNDEFINED", 0xFF, None, Some("unknown"))

  def apply(): EnumModel = EnumModel("QualifierCode", comments, EnumModel.UInt8, codes, Some(defaultValue), Hex)

  private val codes = List(
    EnumValue("UINT8_START_STOP", 0x00, None, Some("8-bit start stop")),
    EnumValue("UINT16_START_STOP", 0x01, None, Some("16-bit start stop")),
    EnumValue("ALL_OBJECTS", 0x06, None, Some("all objects")),
    EnumValue("UINT8_CNT", 0x07, None, Some("8-bit count")),
    EnumValue("UINT16_CNT", 0x08, None, Some("16-bit count")),
    EnumValue("UINT8_CNT_UINT8_INDEX", 0x17, None, Some("8-bit count and prefix")),
    EnumValue("UINT16_CNT_UINT16_INDEX", 0x28, None, Some("16-bit count and prefix")),
    EnumValue("UINT16_FREE_FORMAT", 0x5B, None, Some("16-bit free format"))
  )

}
