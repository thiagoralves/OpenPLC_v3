package com.automatak.render.dnp3.enums

import com.automatak.render._


object PointClass {

  private val comments = List(
    "Class assignment for a measurement point"
  )

  def apply(): EnumModel = EnumModel("PointClass", comments, EnumModel.UInt8, codes, None, Hex)

  private val codes = List(
    EnumValue("Class0", 0x01, "No event class assignment"),
    EnumValue("Class1", 0x02, "Assigned to event class 1"),
    EnumValue("Class2", 0x04, "Assigned to event class 2"),
    EnumValue("Class3", 0x08, "Assigned to event class 3")
  )

}



