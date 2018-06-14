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

#include "opendnp3/gen/CommandPointState.h"

namespace opendnp3 {

char const* CommandPointStateToString(CommandPointState arg)
{
  switch(arg)
  {
    case(CommandPointState::INIT):
      return "INIT";
    case(CommandPointState::SELECT_SUCCESS):
      return "SELECT_SUCCESS";
    case(CommandPointState::SELECT_MISMATCH):
      return "SELECT_MISMATCH";
    case(CommandPointState::SELECT_FAIL):
      return "SELECT_FAIL";
    case(CommandPointState::OPERATE_FAIL):
      return "OPERATE_FAIL";
    case(CommandPointState::SUCCESS):
      return "SUCCESS";
    default:
      return "UNDEFINED";
  }
}

}
