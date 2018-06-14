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

#include "opendnp3/gen/UserOperation.h"

namespace opendnp3 {

uint8_t UserOperationToType(UserOperation arg)
{
  return static_cast<uint8_t>(arg);
}
UserOperation UserOperationFromType(uint8_t arg)
{
  switch(arg)
  {
    case(0x1):
      return UserOperation::OP_ADD;
    case(0x2):
      return UserOperation::OP_DELETE;
    case(0x3):
      return UserOperation::OP_CHANGE;
    default:
      return UserOperation::OP_UNDEFINED;
  }
}
char const* UserOperationToString(UserOperation arg)
{
  switch(arg)
  {
    case(UserOperation::OP_ADD):
      return "OP_ADD";
    case(UserOperation::OP_DELETE):
      return "OP_DELETE";
    case(UserOperation::OP_CHANGE):
      return "OP_CHANGE";
    default:
      return "OP_UNDEFINED";
  }
}

}
