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
* Enumerates possible remote operations on users
*/
public enum UserOperation
{
  OP_ADD(0x1),
  OP_DELETE(0x2),
  OP_CHANGE(0x3),
  OP_UNDEFINED(0xFF);

  private final int id;

  public int toType()
  {
    return id;
  }

  UserOperation(int id)
  {
    this.id = id;
  }

  public static UserOperation fromType(int arg)
  {
    switch(arg)
    {
      case(0x1):
        return OP_ADD;
      case(0x2):
        return OP_DELETE;
      case(0x3):
        return OP_CHANGE;
      default:
        return OP_UNDEFINED;
    }
  }
}
