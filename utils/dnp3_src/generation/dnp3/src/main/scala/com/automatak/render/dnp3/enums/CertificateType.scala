package com.automatak.render.dnp3.enums

import com.automatak.render._


object CertificateType {

  private val comments = List(
    "Specifies the reason that an auth error message was transmitted"
  )

  def apply(): EnumModel = EnumModel("CertificateType", comments, EnumModel.UInt8, codes, Some(defaultValue), Hex)

  private val defaultValue = EnumValue("UNKNOWN", 0, "Unknown certificate type")

  private val codes = List(
    EnumValue("ID_CERTIFICATE", 1, "ID certificate"),
    EnumValue("ATTRIBUTE_CERTIFICATE", 2, "Attribute certificate")
  )

}



