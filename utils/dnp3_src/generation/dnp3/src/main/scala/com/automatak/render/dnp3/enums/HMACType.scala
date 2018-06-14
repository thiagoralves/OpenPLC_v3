package com.automatak.render.dnp3.enums

import com.automatak.render._


object HMACType {

  private val comments = List(
    "Enumerates possible algorithms used to calculate the HMAC"
  )

  def apply(): EnumModel = EnumModel("HMACType", comments, EnumModel.UInt8, codes, Some(defaultValue), Hex)

  private val defaultValue = EnumValue("UNKNOWN", 255, "Unknown HMAC algorithm")

  private val codes = List(
    EnumValue("NO_MAC_VALUE", 0, "No HMAC value in this message"),
    EnumValue("HMAC_SHA1_TRUNC_10", 2, "Networked version of SHA1"),
    EnumValue("HMAC_SHA256_TRUNC_8", 3, "Serial version of SHA256"),
    EnumValue("HMAC_SHA256_TRUNC_16", 4, "Networked version of SHA256"),
    EnumValue("HMAC_SHA1_TRUNC_8", 5, "Serial version of SHA1"),
    EnumValue("AES_GMAC", 6, "12 octet output for serial or network")
  )

}



