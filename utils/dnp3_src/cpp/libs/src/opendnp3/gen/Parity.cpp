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

#include "opendnp3/gen/Parity.h"

namespace opendnp3 {

uint8_t ParityToType(Parity arg)
{
  return static_cast<uint8_t>(arg);
}
Parity ParityFromType(uint8_t arg)
{
  switch(arg)
  {
    case(1):
      return Parity::Even;
    case(2):
      return Parity::Odd;
    default:
      return Parity::None;
  }
}
char const* ParityToString(Parity arg)
{
  switch(arg)
  {
    case(Parity::Even):
      return "Even";
    case(Parity::Odd):
      return "Odd";
    default:
      return "None";
  }
}

}
