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

#include "opendnp3/gen/QualifierCode.h"

namespace opendnp3 {

uint8_t QualifierCodeToType(QualifierCode arg)
{
  return static_cast<uint8_t>(arg);
}
QualifierCode QualifierCodeFromType(uint8_t arg)
{
  switch(arg)
  {
    case(0x0):
      return QualifierCode::UINT8_START_STOP;
    case(0x1):
      return QualifierCode::UINT16_START_STOP;
    case(0x6):
      return QualifierCode::ALL_OBJECTS;
    case(0x7):
      return QualifierCode::UINT8_CNT;
    case(0x8):
      return QualifierCode::UINT16_CNT;
    case(0x17):
      return QualifierCode::UINT8_CNT_UINT8_INDEX;
    case(0x28):
      return QualifierCode::UINT16_CNT_UINT16_INDEX;
    case(0x5B):
      return QualifierCode::UINT16_FREE_FORMAT;
    default:
      return QualifierCode::UNDEFINED;
  }
}
char const* QualifierCodeToString(QualifierCode arg)
{
  switch(arg)
  {
    case(QualifierCode::UINT8_START_STOP):
      return "8-bit start stop";
    case(QualifierCode::UINT16_START_STOP):
      return "16-bit start stop";
    case(QualifierCode::ALL_OBJECTS):
      return "all objects";
    case(QualifierCode::UINT8_CNT):
      return "8-bit count";
    case(QualifierCode::UINT16_CNT):
      return "16-bit count";
    case(QualifierCode::UINT8_CNT_UINT8_INDEX):
      return "8-bit count and prefix";
    case(QualifierCode::UINT16_CNT_UINT16_INDEX):
      return "16-bit count and prefix";
    case(QualifierCode::UINT16_FREE_FORMAT):
      return "16-bit free format";
    default:
      return "unknown";
  }
}

}
