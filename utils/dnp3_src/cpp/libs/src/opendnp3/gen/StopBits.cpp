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

#include "opendnp3/gen/StopBits.h"

namespace opendnp3 {

uint8_t StopBitsToType(StopBits arg)
{
  return static_cast<uint8_t>(arg);
}
StopBits StopBitsFromType(uint8_t arg)
{
  switch(arg)
  {
    case(1):
      return StopBits::One;
    case(2):
      return StopBits::OnePointFive;
    case(3):
      return StopBits::Two;
    default:
      return StopBits::None;
  }
}
char const* StopBitsToString(StopBits arg)
{
  switch(arg)
  {
    case(StopBits::One):
      return "One";
    case(StopBits::OnePointFive):
      return "OnePointFive";
    case(StopBits::Two):
      return "Two";
    default:
      return "None";
  }
}

}
