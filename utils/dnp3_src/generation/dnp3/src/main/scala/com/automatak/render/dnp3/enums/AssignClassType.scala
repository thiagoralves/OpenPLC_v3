package com.automatak.render.dnp3.enums

import com.automatak.render._


object AssignClassType {

  private val comments = List(
    "groups that can be used inconjunction with the ASSIGN_CLASS function code"
  )

  def apply(): EnumModel = EnumModel("AssignClassType", comments, EnumModel.UInt8, codes, None, Hex)

  private val codes = EnumValues.from(List(
    "BinaryInput",
    "DoubleBinaryInput",
    "Counter",
    "FrozenCounter",
    "AnalogInput",
    "BinaryOutputStatus",
    "AnalogOutputStatus"
  ))

}



