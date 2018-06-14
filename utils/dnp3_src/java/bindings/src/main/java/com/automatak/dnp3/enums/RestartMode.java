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
* Enumeration describing restart mode support of an outstation
*/
public enum RestartMode
{
  /**
  * Device does not support restart
  */
  UNSUPPORTED(0),
  /**
  * Supports restart, and time returned is a fine time delay
  */
  SUPPORTED_DELAY_FINE(1),
  /**
  * Supports restart, and time returned is a coarse time delay
  */
  SUPPORTED_DELAY_COARSE(2);

  private final int id;

  public int toType()
  {
    return id;
  }

  RestartMode(int id)
  {
    this.id = id;
  }

  public static RestartMode fromType(int arg)
  {
    switch(arg)
    {
      case(0):
        return UNSUPPORTED;
      case(1):
        return SUPPORTED_DELAY_FINE;
      case(2):
        return SUPPORTED_DELAY_COARSE;
      default:
        return UNSUPPORTED;
    }
  }
}
