package com.automatak.render.dnp3.enums

import com.automatak.render._


object AuthErrorCode {

  private val comments = List(
    "Specifies the reason that an auth error message was transmitted"
  )

  def apply(): EnumModel = EnumModel("AuthErrorCode", comments, EnumModel.UInt8, codes, Some(defaultValue), Hex)

  private val defaultValue = EnumValue("UNKNOWN", 0, "Unknown error code")

  private val codes = List(
    EnumValue("AUTHENTICATION_FAILED", 1, "Supplied auth information "),
    EnumValue("AGGRESSIVE_MODE_UNSUPPORTED", 4, "Aggressive mode not supported on this link"),
    EnumValue("MAC_NOT_SUPPORTED", 5, "The specified MAC algorithm is not supported"),
    EnumValue("KEY_WRAP_NOT_SUPPORTED", 6, "The key-wrap algorithm is not supported"),
    EnumValue("AUTHORIZATION_FAILED", 7, "Authentication passed, but that user not authorized for requested operation"),
    EnumValue("UPDATE_KEY_METHOD_NOT_PERMITTED", 8, "The outstation does not support specified key change method on this link"),
    EnumValue("INVALID_SIGNATURE", 9, "The digital signature supplied in a user status change object was invalid"),
    EnumValue("INVALID_CERTIFICATION_DATA", 10, "The certification data supplied in a user status change object was invalid"),
    EnumValue("UNKNOWN_USER", 11, "The master attempted to change the Update Key of a user without a valid prior user status change"),
    EnumValue("MAX_SESSION_KEY_STATUS_REQUESTS_EXCEEDED", 12, "The master on a different association has requred session key status too often")
  )

}



