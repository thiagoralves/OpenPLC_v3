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

#ifndef OPENDNP3_AUTHERRORCODE_H
#define OPENDNP3_AUTHERRORCODE_H

#include <cstdint>

namespace opendnp3 {

/**
  Specifies the reason that an auth error message was transmitted
*/
enum class AuthErrorCode : uint8_t
{
  /// Supplied auth information 
  AUTHENTICATION_FAILED = 0x1,
  /// Aggressive mode not supported on this link
  AGGRESSIVE_MODE_UNSUPPORTED = 0x4,
  /// The specified MAC algorithm is not supported
  MAC_NOT_SUPPORTED = 0x5,
  /// The key-wrap algorithm is not supported
  KEY_WRAP_NOT_SUPPORTED = 0x6,
  /// Authentication passed, but that user not authorized for requested operation
  AUTHORIZATION_FAILED = 0x7,
  /// The outstation does not support specified key change method on this link
  UPDATE_KEY_METHOD_NOT_PERMITTED = 0x8,
  /// The digital signature supplied in a user status change object was invalid
  INVALID_SIGNATURE = 0x9,
  /// The certification data supplied in a user status change object was invalid
  INVALID_CERTIFICATION_DATA = 0xA,
  /// The master attempted to change the Update Key of a user without a valid prior user status change
  UNKNOWN_USER = 0xB,
  /// The master on a different association has requred session key status too often
  MAX_SESSION_KEY_STATUS_REQUESTS_EXCEEDED = 0xC,
  /// Unknown error code
  UNKNOWN = 0x0
};

uint8_t AuthErrorCodeToType(AuthErrorCode arg);
AuthErrorCode AuthErrorCodeFromType(uint8_t arg);
char const* AuthErrorCodeToString(AuthErrorCode arg);

}

#endif
