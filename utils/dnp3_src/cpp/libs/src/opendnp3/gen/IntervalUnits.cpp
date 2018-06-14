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

#include "opendnp3/gen/IntervalUnits.h"

namespace opendnp3 {

uint8_t IntervalUnitsToType(IntervalUnits arg)
{
  return static_cast<uint8_t>(arg);
}
IntervalUnits IntervalUnitsFromType(uint8_t arg)
{
  switch(arg)
  {
    case(0x0):
      return IntervalUnits::NoRepeat;
    case(0x1):
      return IntervalUnits::Milliseconds;
    case(0x2):
      return IntervalUnits::Seconds;
    case(0x3):
      return IntervalUnits::Minutes;
    case(0x4):
      return IntervalUnits::Hours;
    case(0x5):
      return IntervalUnits::Days;
    case(0x6):
      return IntervalUnits::Weeks;
    case(0x7):
      return IntervalUnits::Months7;
    case(0x8):
      return IntervalUnits::Months8;
    case(0x9):
      return IntervalUnits::Months9;
    case(0xA):
      return IntervalUnits::Seasons;
    default:
      return IntervalUnits::Undefined;
  }
}
char const* IntervalUnitsToString(IntervalUnits arg)
{
  switch(arg)
  {
    case(IntervalUnits::NoRepeat):
      return "NoRepeat";
    case(IntervalUnits::Milliseconds):
      return "Milliseconds";
    case(IntervalUnits::Seconds):
      return "Seconds";
    case(IntervalUnits::Minutes):
      return "Minutes";
    case(IntervalUnits::Hours):
      return "Hours";
    case(IntervalUnits::Days):
      return "Days";
    case(IntervalUnits::Weeks):
      return "Weeks";
    case(IntervalUnits::Months7):
      return "Months7";
    case(IntervalUnits::Months8):
      return "Months8";
    case(IntervalUnits::Months9):
      return "Months9";
    case(IntervalUnits::Seasons):
      return "Seasons";
    default:
      return "Undefined";
  }
}

}
