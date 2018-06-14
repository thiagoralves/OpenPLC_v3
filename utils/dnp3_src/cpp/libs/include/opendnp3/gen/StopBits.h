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

#ifndef OPENDNP3_STOPBITS_H
#define OPENDNP3_STOPBITS_H

#include <cstdint>

namespace opendnp3 {

/**
  Enumeration for setting serial port stop bits
*/
enum class StopBits : uint8_t
{
  One = 1,
  OnePointFive = 2,
  Two = 3,
  None = 0
};

uint8_t StopBitsToType(StopBits arg);
StopBits StopBitsFromType(uint8_t arg);
char const* StopBitsToString(StopBits arg);

}

#endif
