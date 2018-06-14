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
* Indicates the validity of timestamp values for an entire object header
*/
public enum TimestampMode
{
  /**
  * The timestamp is UTC synchronized at the remote device
  */
  SYNCHRONIZED(1),
  /**
  * The device indicate the timestamp may be unsynchronized
  */
  UNSYNCHRONIZED(2),
  /**
  * Timestamp is not valid, ignore the value and use a local timestamp
  */
  INVALID(0);

  private final int id;

  public int toType()
  {
    return id;
  }

  TimestampMode(int id)
  {
    this.id = id;
  }

  public static TimestampMode fromType(int arg)
  {
    switch(arg)
    {
      case(1):
        return SYNCHRONIZED;
      case(2):
        return UNSYNCHRONIZED;
      default:
        return INVALID;
    }
  }
}
