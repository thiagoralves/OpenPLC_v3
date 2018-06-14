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
public enum StaticTimeAndIntervalVariation
{
  Group50Var4(0);

  private final int id;

  public int toType()
  {
    return id;
  }

  StaticTimeAndIntervalVariation(int id)
  {
    this.id = id;
  }

  public static StaticTimeAndIntervalVariation fromType(int arg)
  {
    switch(arg)
    {
      case(0):
        return Group50Var4;
      default:
        return Group50Var4;
    }
  }
}
