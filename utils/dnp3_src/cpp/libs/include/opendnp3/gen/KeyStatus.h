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

#ifndef OPENDNP3_KEYSTATUS_H
#define OPENDNP3_KEYSTATUS_H

#include <cstdint>

namespace opendnp3 {

/**
  Describes the status of the two session keys as known by the outstation
*/
enum class KeyStatus : uint8_t
{
  /// Session keys are valid
  OK = 0x1,
  /// Sessions keys are not valid due to no init or interval expiration
  NOT_INIT = 0x2,
  /// Session keys are not valid due to communications failure
  COMM_FAIL = 0x3,
  /// Session keys are not valid due to an authentication failure
  AUTH_FAIL = 0x4,
  UNDEFINED = 0x0
};

uint8_t KeyStatusToType(KeyStatus arg);
KeyStatus KeyStatusFromType(uint8_t arg);
char const* KeyStatusToString(KeyStatus arg);

}

#endif
