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

#include "opendnp3/gen/FlowControl.h"

namespace opendnp3 {

uint8_t FlowControlToType(FlowControl arg)
{
  return static_cast<uint8_t>(arg);
}
FlowControl FlowControlFromType(uint8_t arg)
{
  switch(arg)
  {
    case(1):
      return FlowControl::Hardware;
    case(2):
      return FlowControl::XONXOFF;
    default:
      return FlowControl::None;
  }
}
char const* FlowControlToString(FlowControl arg)
{
  switch(arg)
  {
    case(FlowControl::Hardware):
      return "Hardware";
    case(FlowControl::XONXOFF):
      return "XONXOFF";
    default:
      return "None";
  }
}

}
