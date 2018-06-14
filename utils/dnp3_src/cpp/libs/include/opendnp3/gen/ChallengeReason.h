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

#ifndef OPENDNP3_CHALLENGEREASON_H
#define OPENDNP3_CHALLENGEREASON_H

#include <cstdint>

namespace opendnp3 {

/**
  Enumerates reasons for a sec-auth challenge
*/
enum class ChallengeReason : uint8_t
{
  /// Challenging a critical function
  CRITICAL = 0x1,
  /// Unknown reason
  UNKNOWN = 0xFF
};

uint8_t ChallengeReasonToType(ChallengeReason arg);
ChallengeReason ChallengeReasonFromType(uint8_t arg);
char const* ChallengeReasonToString(ChallengeReason arg);

}

#endif
