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
* Object header range/prefix as a single enumeration
*/
public enum QualifierCode
{
  UINT8_START_STOP(0x0),
  UINT16_START_STOP(0x1),
  ALL_OBJECTS(0x6),
  UINT8_CNT(0x7),
  UINT16_CNT(0x8),
  UINT8_CNT_UINT8_INDEX(0x17),
  UINT16_CNT_UINT16_INDEX(0x28),
  UINT16_FREE_FORMAT(0x5B),
  UNDEFINED(0xFF);

  private final int id;

  public int toType()
  {
    return id;
  }

  QualifierCode(int id)
  {
    this.id = id;
  }

  public static QualifierCode fromType(int arg)
  {
    switch(arg)
    {
      case(0x0):
        return UINT8_START_STOP;
      case(0x1):
        return UINT16_START_STOP;
      case(0x6):
        return ALL_OBJECTS;
      case(0x7):
        return UINT8_CNT;
      case(0x8):
        return UINT16_CNT;
      case(0x17):
        return UINT8_CNT_UINT8_INDEX;
      case(0x28):
        return UINT16_CNT_UINT16_INDEX;
      case(0x5B):
        return UINT16_FREE_FORMAT;
      default:
        return UNDEFINED;
    }
  }
}
