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

#ifndef OPENDNP3_MASTERTASKTYPE_H
#define OPENDNP3_MASTERTASKTYPE_H

#include <cstdint>

namespace opendnp3 {

/**
  Enumeration of internal tasks
*/
enum class MasterTaskType : uint8_t
{
  CLEAR_RESTART = 0,
  DISABLE_UNSOLICITED = 1,
  ASSIGN_CLASS = 2,
  STARTUP_INTEGRITY_POLL = 3,
  SERIAL_TIME_SYNC = 4,
  ENABLE_UNSOLICITED = 5,
  AUTO_EVENT_SCAN = 6,
  USER_TASK = 7,
  SET_SESSION_KEYS = 8
};

char const* MasterTaskTypeToString(MasterTaskType arg);

}

#endif
