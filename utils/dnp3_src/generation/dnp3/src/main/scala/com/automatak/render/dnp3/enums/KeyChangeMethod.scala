package com.automatak.render.dnp3.enums

import com.automatak.render._


object KeyChangeMethod {

  private val comments = List("Enumerates possible algorithms for changing the update key")
  private val defaultValue = EnumValue("UNDEFINED", 0)

  def apply(): EnumModel = EnumModel("KeyChangeMethod", comments, EnumModel.UInt8, codes, Some(defaultValue), Hex)

  private val codes = List(
    EnumValue("AES_128_SHA1_HMAC", 3),
    EnumValue("AES_256_SHA256_HMAC", 4),
    EnumValue("AES_256_AES_GMAC", 5),
    EnumValue("RSA_1024_DSA_SHA1_HMAC_SHA1", 67),
    EnumValue("RSA_2048_DSA_SHA256_HMAC_SHA256", 68),
    EnumValue("RSA_3072_DSA_SHA256_HMAC_SHA256", 69),
    EnumValue("RSA_2048_DSA_SHA256_AES_GMAC", 70),
    EnumValue("RSA_3072_DSA_SHA256_AES_GMAC", 71)
  )

}



