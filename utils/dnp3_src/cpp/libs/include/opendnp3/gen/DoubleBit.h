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

#ifndef OPENDNP3_DOUBLEBIT_H
#define OPENDNP3_DOUBLEBIT_H

#include <cstdint>

namespace opendnp3 {

/**
  Enumeration for possible states of a double bit value
*/
enum class DoubleBit : uint8_t
{
  /// Transitioning between end conditions
  INTERMEDIATE = 0x0,
  /// End condition, determined to be OFF
  DETERMINED_OFF = 0x1,
  /// End condition, determined to be ON
  DETERMINED_ON = 0x2,
  /// Abnormal or custom condition
  INDETERMINATE = 0x3
};

uint8_t DoubleBitToType(DoubleBit arg);
DoubleBit DoubleBitFromType(uint8_t arg);
char const* DoubleBitToString(DoubleBit arg);

}

#endif
