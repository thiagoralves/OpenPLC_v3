package com.automatak.render.dnp3.enums

import com.automatak.render.{EnumValue, Base10, EnumModel}


object SecurityStatIndex {

  private val comments = List("Indices of the SA security statistics")

  def apply(): EnumModel = EnumModel("SecurityStatIndex", comments, EnumModel.UInt8, codes, None, Base10)

  private val codes = List(
    EnumValue("UNEXPECTED_MESSAGES", 0, "The other device has responded with a message that was not the expected next step in the state machine."),
    EnumValue("AUTHORIZATION_FAILURES", 1, "The other device has replied with the correct authentication information, so the user is authentic, but the user is not authorized to perform the requested operation."),
    EnumValue("AUTHENTICATION_FAILURES", 2, "The other device has provided invalid authentication information such as an incorrect MAC"),
    EnumValue("REPLY_TIMEOUTS", 3, "The other device has not replied within the configured time required"),
    EnumValue("REKEYS_DUE_TO_AUTH_FAILUE", 4, "An Authentication Failure has occurred that causes the master station to change the session keys"),
    EnumValue("TOTAL_MESSAGES_TX", 5, "The device sends an Application Layer fragment"),
    EnumValue("TOTAL_MESSAGES_RX", 6, "The device receives an Application Layer fragment"),
    EnumValue("CRITICAL_MESSAGES_TX", 7, "The device receives a Challenge message or transmits an Aggressive Mode Request message"),
    EnumValue("CRITICAL_MESSAGES_RX", 8, "The device transmits a Challenge message or receives an Aggressive Mode Request message"),
    EnumValue("DISCARED_MESSAGES", 9, "The device discards a received message"),
    EnumValue("ERROR_MESSAGES_TX", 10, "The device has sent a fragment containing an Error object indicating an authentication failure or potential configuration error"),
    EnumValue("ERROR_MESSAGES_RX", 11, "The device has received an Error object"),
    EnumValue("SUCCESSFUL_AUTHS", 12, "The device successfully authenticates a message"),
    EnumValue("SESSION_KEY_CHANGES", 13, "A users session key is successfully changed"),
    EnumValue("FAILED_SESSION_KEY_CHANGES", 14, "A failure occurs while changing a session key"),
    EnumValue("UPDATE_KEY_CHANGES", 15, "The master and authority change the Update Key for a user"),
    EnumValue("FAILED_UPDATE_KEY_CHANGES", 16, "The master and authority fail to change the Update Key for a user"),
    EnumValue("REKEYS_DUE_TO_RESTART", 17, "Only used by a master. Set to zero in outstations. The master rekeyed the session keys because the outstation restarted")
  )

}
