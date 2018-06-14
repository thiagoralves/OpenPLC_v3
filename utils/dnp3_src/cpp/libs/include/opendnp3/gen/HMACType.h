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

#ifndef OPENDNP3_HMACTYPE_H
#define OPENDNP3_HMACTYPE_H

#include <cstdint>

namespace opendnp3 {

/**
  Enumerates possible algorithms used to calculate the HMAC
*/
enum class HMACType : uint8_t
{
  /// No HMAC value in this message
  NO_MAC_VALUE = 0x0,
  /// Networked version of SHA1
  HMAC_SHA1_TRUNC_10 = 0x2,
  /// Serial version of SHA256
  HMAC_SHA256_TRUNC_8 = 0x3,
  /// Networked version of SHA256
  HMAC_SHA256_TRUNC_16 = 0x4,
  /// Serial version of SHA1
  HMAC_SHA1_TRUNC_8 = 0x5,
  /// 12 octet output for serial or network
  AES_GMAC = 0x6,
  /// Unknown HMAC algorithm
  UNKNOWN = 0xFF
};

uint8_t HMACTypeToType(HMACType arg);
HMACType HMACTypeFromType(uint8_t arg);
char const* HMACTypeToString(HMACType arg);

}

#endif
