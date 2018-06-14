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

#ifndef OPENDNP3_TIMESYNCMODE_H
#define OPENDNP3_TIMESYNCMODE_H

#include <cstdint>

namespace opendnp3 {

/**
  Determines what the master station does when it sees the NEED_TIME iin bit
*/
enum class TimeSyncMode : uint8_t
{
  /// synchronize the outstation's time using the serial time sync procedure
  SerialTimeSync = 1,
  /// don't perform a time-sync
  None = 0
};


}

#endif
