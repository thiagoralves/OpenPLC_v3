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

#ifndef OPENDNP3_USEROPERATION_H
#define OPENDNP3_USEROPERATION_H

#include <cstdint>

namespace opendnp3 {

/**
  Enumerates possible remote operations on users
*/
enum class UserOperation : uint8_t
{
  OP_ADD = 0x1,
  OP_DELETE = 0x2,
  OP_CHANGE = 0x3,
  OP_UNDEFINED = 0xFF
};

uint8_t UserOperationToType(UserOperation arg);
UserOperation UserOperationFromType(uint8_t arg);
char const* UserOperationToString(UserOperation arg);

}

#endif
