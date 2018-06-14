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

#ifndef OPENDNP3_POINTCLASS_H
#define OPENDNP3_POINTCLASS_H

#include <cstdint>

namespace opendnp3 {

/**
  Class assignment for a measurement point
*/
enum class PointClass : uint8_t
{
  /// No event class assignment
  Class0 = 0x1,
  /// Assigned to event class 1
  Class1 = 0x2,
  /// Assigned to event class 2
  Class2 = 0x4,
  /// Assigned to event class 3
  Class3 = 0x8
};


}

#endif
