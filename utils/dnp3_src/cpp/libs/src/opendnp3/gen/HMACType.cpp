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

#include "opendnp3/gen/HMACType.h"

namespace opendnp3 {

uint8_t HMACTypeToType(HMACType arg)
{
  return static_cast<uint8_t>(arg);
}
HMACType HMACTypeFromType(uint8_t arg)
{
  switch(arg)
  {
    case(0x0):
      return HMACType::NO_MAC_VALUE;
    case(0x2):
      return HMACType::HMAC_SHA1_TRUNC_10;
    case(0x3):
      return HMACType::HMAC_SHA256_TRUNC_8;
    case(0x4):
      return HMACType::HMAC_SHA256_TRUNC_16;
    case(0x5):
      return HMACType::HMAC_SHA1_TRUNC_8;
    case(0x6):
      return HMACType::AES_GMAC;
    default:
      return HMACType::UNKNOWN;
  }
}
char const* HMACTypeToString(HMACType arg)
{
  switch(arg)
  {
    case(HMACType::NO_MAC_VALUE):
      return "NO_MAC_VALUE";
    case(HMACType::HMAC_SHA1_TRUNC_10):
      return "HMAC_SHA1_TRUNC_10";
    case(HMACType::HMAC_SHA256_TRUNC_8):
      return "HMAC_SHA256_TRUNC_8";
    case(HMACType::HMAC_SHA256_TRUNC_16):
      return "HMAC_SHA256_TRUNC_16";
    case(HMACType::HMAC_SHA1_TRUNC_8):
      return "HMAC_SHA1_TRUNC_8";
    case(HMACType::AES_GMAC):
      return "AES_GMAC";
    default:
      return "UNKNOWN";
  }
}

}
