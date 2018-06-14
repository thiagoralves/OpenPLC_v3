//
//  _   _         ______    _ _ _   _             _ _ _
// | \ | |       |  ____|  | (_) | (_)           | | | |
// |  \| | ___   | |__   __| |_| |_ _ _ __   __ _| | | |
// | . ` |/ _ \  |  __| / _` | | __| | '_ \ / _` | | | |
// | |\  | (_) | | |___| (_| | | |_| | | | | (_| |_|_|_|
// |_| \_|\___/  |______\__,_|_|\__|_|_| |_|\__, (_|_|_)
//                                           __/ |
//                                          |___/
// 
// This file is auto-generated. Do not edit manually
// 
// Copyright 2013 Automatak LLC
// 
// Automatak LLC (www.automatak.com) licenses this file
// to you under the the Apache License Version 2.0 (the "License"):
// 
// http://www.apache.org/licenses/LICENSE-2.0.html
//

package com.automatak.dnp3.enums;
/**
* Time internal units
*/
public enum IntervalUnits
{
  /**
  * The outstation does not repeat the action regardless of the value in the interval count
  */
  NoRepeat(0x0),
  /**
  * the interval is always counted relative to the start time and is constant regardless of the clock time set at the outstation
  */
  Milliseconds(0x1),
  /**
  * At the same millisecond within the second that is specified in the start time
  */
  Seconds(0x2),
  /**
  * At the same second and millisecond within the minute that is specified in the start time
  */
  Minutes(0x3),
  /**
  * At the same minute, second and millisecond within the hour that is specified in the start time
  */
  Hours(0x4),
  /**
  * At the same time of day that is specified in the start time
  */
  Days(0x5),
  /**
  * On the same day of the week at the same time of day that is specified in the start time
  */
  Weeks(0x6),
  /**
  * On the same day of each month at the same time of day that is specified in the start time
  */
  Months7(0x7),
  /**
  * At the same time of day on the same day of the week after the beginning of the month as the day specified in the start time
  */
  Months8(0x8),
  /**
  * Months on Same Day of Week from End of Month - The outstation shall interpret this setting as in Months8, but the day of the week shall be measured from the end of the month,
  */
  Months9(0x9),
  /**
  * The definition of a season is specific to the outstation
  */
  Seasons(0xA),
  /**
  * 11-127 are reserved for future use
  */
  Undefined(0x7F);

  private final int id;

  public int toType()
  {
    return id;
  }

  IntervalUnits(int id)
  {
    this.id = id;
  }

  public static IntervalUnits fromType(int arg)
  {
    switch(arg)
    {
      case(0x0):
        return NoRepeat;
      case(0x1):
        return Milliseconds;
      case(0x2):
        return Seconds;
      case(0x3):
        return Minutes;
      case(0x4):
        return Hours;
      case(0x5):
        return Days;
      case(0x6):
        return Weeks;
      case(0x7):
        return Months7;
      case(0x8):
        return Months8;
      case(0x9):
        return Months9;
      case(0xA):
        return Seasons;
      default:
        return Undefined;
    }
  }
}
