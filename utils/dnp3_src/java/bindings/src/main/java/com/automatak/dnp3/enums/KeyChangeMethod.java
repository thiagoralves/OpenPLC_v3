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

package com.automatak.dnp3.enums;
/**
* Enumerates possible algorithms for changing the update key
*/
public enum KeyChangeMethod
{
  AES_128_SHA1_HMAC(0x3),
  AES_256_SHA256_HMAC(0x4),
  AES_256_AES_GMAC(0x5),
  RSA_1024_DSA_SHA1_HMAC_SHA1(0x43),
  RSA_2048_DSA_SHA256_HMAC_SHA256(0x44),
  RSA_3072_DSA_SHA256_HMAC_SHA256(0x45),
  RSA_2048_DSA_SHA256_AES_GMAC(0x46),
  RSA_3072_DSA_SHA256_AES_GMAC(0x47),
  UNDEFINED(0x0);

  private final int id;

  public int toType()
  {
    return id;
  }

  KeyChangeMethod(int id)
  {
    this.id = id;
  }

  public static KeyChangeMethod fromType(int arg)
  {
    switch(arg)
    {
      case(0x3):
        return AES_128_SHA1_HMAC;
      case(0x4):
        return AES_256_SHA256_HMAC;
      case(0x5):
        return AES_256_AES_GMAC;
      case(0x43):
        return RSA_1024_DSA_SHA1_HMAC_SHA1;
      case(0x44):
        return RSA_2048_DSA_SHA256_HMAC_SHA256;
      case(0x45):
        return RSA_3072_DSA_SHA256_HMAC_SHA256;
      case(0x46):
        return RSA_2048_DSA_SHA256_AES_GMAC;
      case(0x47):
        return RSA_3072_DSA_SHA256_AES_GMAC;
      default:
        return UNDEFINED;
    }
  }
}
