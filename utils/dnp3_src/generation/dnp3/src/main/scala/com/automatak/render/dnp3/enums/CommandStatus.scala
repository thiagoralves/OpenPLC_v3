package com.automatak.render.dnp3.enums

import com.automatak.render._


object CommandStatus {

  private val comments = List(
    "An enumeration of result codes received from an outstation in response to command request.",
    "These correspond to those defined in the DNP3 standard"
  )

  def apply(): EnumModel = EnumModel("CommandStatus", comments, EnumModel.UInt8, codes, Some(defaultValue), Base10)

  private val defaultValue = EnumValue("UNDEFINED", 127, "10 to 126 are currently reserved")

  private val codes = List(
    EnumValue("SUCCESS", 0, "command was accepted, initiated, or queued"),
    EnumValue("TIMEOUT", 1, "command timed out before completing"),
    EnumValue("NO_SELECT", 2, "command requires being selected before operate, configuration issue"),
    EnumValue("FORMAT_ERROR", 3, "bad control code or timing values"),
    EnumValue("NOT_SUPPORTED", 4, "command is not implemented"),
    EnumValue("ALREADY_ACTIVE", 5, "command is all ready in progress or its all ready in that mode"),
    EnumValue("HARDWARE_ERROR", 6, "something is stopping the command, often a local/remote interlock"),
    EnumValue("LOCAL", 7, "the function governed by the control is in local only control"),
    EnumValue("TOO_MANY_OPS", 8, "the command has been done too often and has been throttled"),
    EnumValue("NOT_AUTHORIZED", 9, "the command was rejected because the device denied it or an RTU intercepted it"),
    EnumValue("AUTOMATION_INHIBIT", 10, "command not accepted because it was prevented or inhibited by a local automation process, such as interlocking logic or synchrocheck"),
    EnumValue("PROCESSING_LIMITED", 11, "command not accepted because the device cannot process any more activities than are presently in progress"),
    EnumValue("OUT_OF_RANGE", 12, "command not accepted because the value is outside the acceptable range permitted for this point"),
    EnumValue("DOWNSTREAM_LOCAL", 13, "command not accepted because the outstation is forwarding the request to another downstream device which reported LOCAL"),
    EnumValue("ALREADY_COMPLETE", 14, "command not accepted because the outstation has already completed the requested operation"),
    EnumValue("BLOCKED", 15, "command not accepted because the requested function is specifically blocked at the outstation"),
    EnumValue("CANCELLED", 16, "command not accepted because the operation was cancelled"),
    EnumValue("BLOCKED_OTHER_MASTER", 17, "command not accepted because another master is communicating with the outstation and has exclusive rights to operate this control point"),
    EnumValue("DOWNSTREAM_FAIL", 18, "command not accepted because the outstation is forwarding the request to another downstream device which cannot be reached or is otherwise incapable of performing the request"),
    EnumValue("NON_PARTICIPATING", 126, "(deprecated) indicates the outstation shall not issue or perform the control operation")
  )


}



