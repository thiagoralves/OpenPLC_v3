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

#include "opendnp3/gen/DoubleBit.h"

namespace opendnp3 {

uint8_t DoubleBitToType(DoubleBit arg)
{
  return static_cast<uint8_t>(arg);
}
DoubleBit DoubleBitFromType(uint8_t arg)
{
  switch(arg)
  {
    case(0x0):
      return DoubleBit::INTERMEDIATE;
    case(0x1):
      return DoubleBit::DETERMINED_OFF;
    case(0x2):
      return DoubleBit::DETERMINED_ON;
    default:
      return DoubleBit::INDETERMINATE;
  }
}
char const* DoubleBitToString(DoubleBit arg)
{
  switch(arg)
  {
    case(DoubleBit::INTERMEDIATE):
      return "INTERMEDIATE";
    case(DoubleBit::DETERMINED_OFF):
      return "DETERMINED_OFF";
    case(DoubleBit::DETERMINED_ON):
      return "DETERMINED_ON";
    default:
      return "INDETERMINATE";
  }
}

}
