package com.automatak.render.dnp3.enums

import com.automatak.render.{EnumValue, Base10, EnumModel}

object GroupVariationType {

  private val comments = List("Enumeration for possible states of a channel")

  private val undefined = EnumValue("UNKNOWN", 5, "object type is undefined")

  def apply(): EnumModel = EnumModel("GroupVariationType", comments, EnumModel.UInt8, types, Some(undefined), Base10)

  private val types = List(
    EnumValue("FIXED_SIZE", 0, "fixed size type"),
    EnumValue("SINGLE_BITFIELD", 1, "single bit-field type"),
    EnumValue("DOUBLE_BITFIELD", 2, "double bit-field type"),
    EnumValue("SIZELESS", 3, "object has no size and is only used in requests"),
    EnumValue("SIZED_BY_VARIATION", 4, "the size of the object is the object's variation"),
    EnumValue("VARIABLE_SIZE", 5, "object size varies and requires free-format header")
  )

}