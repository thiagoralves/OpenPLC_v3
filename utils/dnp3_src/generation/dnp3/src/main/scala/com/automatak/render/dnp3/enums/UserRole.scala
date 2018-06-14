package com.automatak.render.dnp3.enums

import com.automatak.render.{Base10, EnumModel, EnumValue}


object UserRole {
  private val comments = List("Enumerates pre-defined rules in secure authentication")
  private val defaultValue = EnumValue("UNDEFINED", 32767)

  def apply(): EnumModel = EnumModel("UserRole", comments, EnumModel.UInt16, codes, Some(defaultValue), Base10)

  private val codes = List(
    EnumValue("VIEWER", 0),
    EnumValue("OPERATOR", 1),
    EnumValue("ENGINEER", 2),
    EnumValue("INSTALLER", 3),
    EnumValue("SECADM", 4),
    EnumValue("SECAUD", 5),
    EnumValue("RBACMNT", 6),
    EnumValue("SINGLE_USER", 32768)
  )
}
