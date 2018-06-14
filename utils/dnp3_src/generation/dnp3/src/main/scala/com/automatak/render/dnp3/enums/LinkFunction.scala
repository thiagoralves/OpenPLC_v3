package com.automatak.render.dnp3.enums

import com.automatak.render._


object LinkFunction {

  private val comments = List("Link layer function code enumeration")

  def apply(): EnumModel = EnumModel("LinkFunction", comments, EnumModel.UInt8, codes, Some(defaultValue), Hex)

  private val defaultValue = EnumValue("INVALID", 0xFF)

  val codes = List(
    EnumValue("PRI_RESET_LINK_STATES", 0x40),
    EnumValue("PRI_TEST_LINK_STATES", 0x42),
    EnumValue("PRI_CONFIRMED_USER_DATA", 0x43),
    EnumValue("PRI_UNCONFIRMED_USER_DATA", 0x44),
    EnumValue("PRI_REQUEST_LINK_STATUS", 0x49),
    EnumValue("SEC_ACK", 0x00),
    EnumValue("SEC_NACK", 0x01),
    EnumValue("SEC_LINK_STATUS", 0x0B),
    EnumValue("SEC_NOT_SUPPORTED", 0x0F)
  )

}



