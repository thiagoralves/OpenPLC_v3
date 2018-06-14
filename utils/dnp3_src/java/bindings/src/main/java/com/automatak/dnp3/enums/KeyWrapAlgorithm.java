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
* Enumerates possible key-wrap algorithms
*/
public enum KeyWrapAlgorithm
{
  /**
  * AES 128 Key Wrap Algorithm
  */
  AES_128(0x1),
  /**
  * AES 256 Key Wrap Algorithm
  */
  AES_256(0x2),
  UNDEFINED(0x0);

  private final int id;

  public int toType()
  {
    return id;
  }

  KeyWrapAlgorithm(int id)
  {
    this.id = id;
  }

  public static KeyWrapAlgorithm fromType(int arg)
  {
    switch(arg)
    {
      case(0x1):
        return AES_128;
      case(0x2):
        return AES_256;
      default:
        return UNDEFINED;
    }
  }
}
