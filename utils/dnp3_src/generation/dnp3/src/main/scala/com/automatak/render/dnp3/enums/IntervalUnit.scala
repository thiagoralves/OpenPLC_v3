package com.automatak.render.dnp3.enums

import com.automatak.render._


object IntervalUnit {

  private val comments = List(
    "Time internal units"
  )

  private val defaultValue = EnumValue("Undefined", 127, "11-127 are reserved for future use")

  def apply(): EnumModel = EnumModel("IntervalUnits", comments, EnumModel.UInt8, codes, Some(defaultValue), Hex)

  private val codes = List(
    EnumValue("NoRepeat", 0, "The outstation does not repeat the action regardless of the value in the interval count"),
    EnumValue("Milliseconds", 1, "the interval is always counted relative to the start time and is constant regardless of the clock time set at the outstation"),
    EnumValue("Seconds", 2, "At the same millisecond within the second that is specified in the start time"),
    EnumValue("Minutes", 3, "At the same second and millisecond within the minute that is specified in the start time"),
    EnumValue("Hours", 4, "At the same minute, second and millisecond within the hour that is specified in the start time"),
    EnumValue("Days", 5, "At the same time of day that is specified in the start time"),
    EnumValue("Weeks", 6, "On the same day of the week at the same time of day that is specified in the start time"),
    EnumValue("Months7", 7, "On the same day of each month at the same time of day that is specified in the start time"),
    EnumValue("Months8", 8, "At the same time of day on the same day of the week after the beginning of the month as the day specified in the start time"),
    EnumValue("Months9", 9, "Months on Same Day of Week from End of Month - The outstation shall interpret this setting as in Months8, but the day of the week shall be measured from the end of the month,"),
    EnumValue("Seasons", 10, "The definition of a season is specific to the outstation")
  )

}



