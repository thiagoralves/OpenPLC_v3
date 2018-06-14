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

#ifndef OPENDNP3_KEYCHANGEMETHOD_H
#define OPENDNP3_KEYCHANGEMETHOD_H

#include <cstdint>

namespace opendnp3 {

/**
  Enumerates possible algorithms for changing the update key
*/
enum class KeyChangeMethod : uint8_t
{
  AES_128_SHA1_HMAC = 0x3,
  AES_256_SHA256_HMAC = 0x4,
  AES_256_AES_GMAC = 0x5,
  RSA_1024_DSA_SHA1_HMAC_SHA1 = 0x43,
  RSA_2048_DSA_SHA256_HMAC_SHA256 = 0x44,
  RSA_3072_DSA_SHA256_HMAC_SHA256 = 0x45,
  RSA_2048_DSA_SHA256_AES_GMAC = 0x46,
  RSA_3072_DSA_SHA256_AES_GMAC = 0x47,
  UNDEFINED = 0x0
};

uint8_t KeyChangeMethodToType(KeyChangeMethod arg);
KeyChangeMethod KeyChangeMethodFromType(uint8_t arg);
char const* KeyChangeMethodToString(KeyChangeMethod arg);

}

#endif
