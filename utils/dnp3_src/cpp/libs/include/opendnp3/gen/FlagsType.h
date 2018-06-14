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

#ifndef OPENDNP3_FLAGSTYPE_H
#define OPENDNP3_FLAGSTYPE_H

#include <cstdint>

namespace opendnp3 {

/**
  enumerates all types that have flags
*/
enum class FlagsType : uint8_t
{
  DoubleBinaryInput = 0x1,
  Counter = 0x2,
  FrozenCounter = 0x3,
  AnalogInput = 0x4,
  BinaryOutputStatus = 0x5,
  AnalogOutputStatus = 0x6,
  BinaryInput = 0x0
};

uint8_t FlagsTypeToType(FlagsType arg);
FlagsType FlagsTypeFromType(uint8_t arg);
char const* FlagsTypeToString(FlagsType arg);

}

#endif
