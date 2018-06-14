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

#include "opendnp3/gen/MasterTaskType.h"

namespace opendnp3 {

char const* MasterTaskTypeToString(MasterTaskType arg)
{
  switch(arg)
  {
    case(MasterTaskType::CLEAR_RESTART):
      return "CLEAR_RESTART";
    case(MasterTaskType::DISABLE_UNSOLICITED):
      return "DISABLE_UNSOLICITED";
    case(MasterTaskType::ASSIGN_CLASS):
      return "ASSIGN_CLASS";
    case(MasterTaskType::STARTUP_INTEGRITY_POLL):
      return "STARTUP_INTEGRITY_POLL";
    case(MasterTaskType::SERIAL_TIME_SYNC):
      return "SERIAL_TIME_SYNC";
    case(MasterTaskType::ENABLE_UNSOLICITED):
      return "ENABLE_UNSOLICITED";
    case(MasterTaskType::AUTO_EVENT_SCAN):
      return "AUTO_EVENT_SCAN";
    case(MasterTaskType::USER_TASK):
      return "USER_TASK";
    case(MasterTaskType::SET_SESSION_KEYS):
      return "SET_SESSION_KEYS";
    default:
      return "UNDEFINED";
  }
}

}
