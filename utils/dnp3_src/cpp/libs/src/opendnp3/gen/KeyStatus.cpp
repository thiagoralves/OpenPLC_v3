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

#include "opendnp3/gen/KeyStatus.h"

namespace opendnp3 {

uint8_t KeyStatusToType(KeyStatus arg)
{
  return static_cast<uint8_t>(arg);
}
KeyStatus KeyStatusFromType(uint8_t arg)
{
  switch(arg)
  {
    case(0x1):
      return KeyStatus::OK;
    case(0x2):
      return KeyStatus::NOT_INIT;
    case(0x3):
      return KeyStatus::COMM_FAIL;
    case(0x4):
      return KeyStatus::AUTH_FAIL;
    default:
      return KeyStatus::UNDEFINED;
  }
}
char const* KeyStatusToString(KeyStatus arg)
{
  switch(arg)
  {
    case(KeyStatus::OK):
      return "OK";
    case(KeyStatus::NOT_INIT):
      return "NOT_INIT";
    case(KeyStatus::COMM_FAIL):
      return "COMM_FAIL";
    case(KeyStatus::AUTH_FAIL):
      return "AUTH_FAIL";
    default:
      return "UNDEFINED";
  }
}

}
