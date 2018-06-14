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

#include "opendnp3/gen/LinkFunction.h"

namespace opendnp3 {

uint8_t LinkFunctionToType(LinkFunction arg)
{
  return static_cast<uint8_t>(arg);
}
LinkFunction LinkFunctionFromType(uint8_t arg)
{
  switch(arg)
  {
    case(0x40):
      return LinkFunction::PRI_RESET_LINK_STATES;
    case(0x42):
      return LinkFunction::PRI_TEST_LINK_STATES;
    case(0x43):
      return LinkFunction::PRI_CONFIRMED_USER_DATA;
    case(0x44):
      return LinkFunction::PRI_UNCONFIRMED_USER_DATA;
    case(0x49):
      return LinkFunction::PRI_REQUEST_LINK_STATUS;
    case(0x0):
      return LinkFunction::SEC_ACK;
    case(0x1):
      return LinkFunction::SEC_NACK;
    case(0xB):
      return LinkFunction::SEC_LINK_STATUS;
    case(0xF):
      return LinkFunction::SEC_NOT_SUPPORTED;
    default:
      return LinkFunction::INVALID;
  }
}
char const* LinkFunctionToString(LinkFunction arg)
{
  switch(arg)
  {
    case(LinkFunction::PRI_RESET_LINK_STATES):
      return "PRI_RESET_LINK_STATES";
    case(LinkFunction::PRI_TEST_LINK_STATES):
      return "PRI_TEST_LINK_STATES";
    case(LinkFunction::PRI_CONFIRMED_USER_DATA):
      return "PRI_CONFIRMED_USER_DATA";
    case(LinkFunction::PRI_UNCONFIRMED_USER_DATA):
      return "PRI_UNCONFIRMED_USER_DATA";
    case(LinkFunction::PRI_REQUEST_LINK_STATUS):
      return "PRI_REQUEST_LINK_STATUS";
    case(LinkFunction::SEC_ACK):
      return "SEC_ACK";
    case(LinkFunction::SEC_NACK):
      return "SEC_NACK";
    case(LinkFunction::SEC_LINK_STATUS):
      return "SEC_LINK_STATUS";
    case(LinkFunction::SEC_NOT_SUPPORTED):
      return "SEC_NOT_SUPPORTED";
    default:
      return "INVALID";
  }
}

}
