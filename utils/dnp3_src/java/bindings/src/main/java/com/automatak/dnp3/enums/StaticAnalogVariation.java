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
*/
public enum StaticAnalogVariation
{
  Group30Var1(0),
  Group30Var2(1),
  Group30Var3(2),
  Group30Var4(3),
  Group30Var5(4),
  Group30Var6(5);

  private final int id;

  public int toType()
  {
    return id;
  }

  StaticAnalogVariation(int id)
  {
    this.id = id;
  }

  public static StaticAnalogVariation fromType(int arg)
  {
    switch(arg)
    {
      case(0):
        return Group30Var1;
      case(1):
        return Group30Var2;
      case(2):
        return Group30Var3;
      case(3):
        return Group30Var4;
      case(4):
        return Group30Var5;
      case(5):
        return Group30Var6;
      default:
        return Group30Var1;
    }
  }
}
