package com.automatak.render.dnp3.enums

import com.automatak.render._


object OperateType {

  private val comments = List(
    "Various ways that an outstation can receive a request to operate a BO or AO point"
  )

  def apply(): EnumModel = EnumModel("OperateType", comments, EnumModel.UInt8, codes, None, Hex)

  private val codes = List(

    EnumValue("SelectBeforeOperate", 0x00, "The outstation received a valid prior SELECT followed by OPERATE"),
    EnumValue("DirectOperate",0x01, "The outstation received a direct operate request"),
    EnumValue("DirectOperateNoAck", 0x02, "The outstation received a direct operate no ack request")

  )

}



