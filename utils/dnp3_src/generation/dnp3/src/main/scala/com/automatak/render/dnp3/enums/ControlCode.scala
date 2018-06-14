package com.automatak.render.dnp3.enums

import com.automatak.render._


object ControlCode {

  private val comments = List(
    "Defines the interoperable values of the Control Code",
    "Refer to pages 506-510 of 1815 for a full description"
  )

  def apply(): EnumModel = EnumModel("ControlCode", comments, EnumModel.UInt8, codes, Some(defaultValue), Hex)

  private val defaultValue = EnumValue("UNDEFINED", 0xFF, " undefined command (used by DNP standard)")

  private val codes = List(

    EnumValue("NUL", 0x00, "Does not initiate an action or change an in-progress or pending command."),
    EnumValue("NUL_CANCEL",0x20, "Cancel in-progress and pending commands. Take no additional action."),

    EnumValue("PULSE_ON", 0x01, "For activation model, set output to active for the duration of the On-time. For both complementary models, return NOT_SUPPORTED status."),
    EnumValue("PULSE_ON_CANCEL", 0x21, "Cancel in-progress and pending commands, process the remainder of the fields as if the control code were PULSE_ON"),

    EnumValue("PULSE_OFF", 0x02, "Non-interoperable code. Do not use for new applications. return NOT_SUPPORTED"),
    EnumValue("PULSE_OFF_CANCEL", 0x22, "Non-interoperable code. Do not use for new applications. Cancel in-progress and pending commands, process remainder of fields as if the control code were PULSE_OFF"),

    EnumValue("LATCH_ON", 0x03, "For activation model, set output to active for the duration of the On-time. For complementary latch model, set the output to active. For complementary two-output model, set the close output to active for the duration of the On-time."),
    EnumValue("LATCH_ON_CANCEL", 0x23, "Cancel in-progress and pending commands, process the remainder of the fields as if the control code were LATCH_ON."),

    EnumValue("LATCH_OFF", 0x04, "For activation model, set output to active for the duration of the On-time. For complementary latch model, set the output to inactive. For complementary two-output model, set the trip output to active for the duration of the On-time."),
    EnumValue("LATCH_OFF_CANCEL", 0x24, "Cancel in-progress and pending commands, process the remainder of the fields as if the control code were LATCH_OFF."),

    EnumValue("CLOSE_PULSE_ON", 0x41, "For activation model, set output to active for the duration of the On-time. For complementary latch model, set the output to active. For complementary two-output model, set the close output to active for the duration of the On-time."),
    EnumValue("CLOSE_PULSE_ON_CANCEL", 0x61, "Cancel in-progress and pending commands, process the remainder of the fields as if the control code were CLOSE_PULSE_ON."),

    EnumValue("TRIP_PULSE_ON", 0x81, "For activation model, set output to active for the duration of the On-time. For complementary latch model, set the output to inactive. For complementary two-output model, set the trip output to active for the duration of the On-time."),
    EnumValue("TRIP_PULSE_ON_CANCEL", 0xA1, "Cancel in-progress and pending commands, process the remainder of the fields as if the control code were TRIP_PULSE_ON.")
  )

}



