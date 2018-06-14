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
* Enumeration of internal tasks
*/
public enum MasterTaskType
{
  CLEAR_RESTART(0),
  DISABLE_UNSOLICITED(1),
  ASSIGN_CLASS(2),
  STARTUP_INTEGRITY_POLL(3),
  SERIAL_TIME_SYNC(4),
  ENABLE_UNSOLICITED(5),
  AUTO_EVENT_SCAN(6),
  USER_TASK(7),
  SET_SESSION_KEYS(8);

  private final int id;

  public int toType()
  {
    return id;
  }

  MasterTaskType(int id)
  {
    this.id = id;
  }

  public static MasterTaskType fromType(int arg)
  {
    switch(arg)
    {
      case(0):
        return CLEAR_RESTART;
      case(1):
        return DISABLE_UNSOLICITED;
      case(2):
        return ASSIGN_CLASS;
      case(3):
        return STARTUP_INTEGRITY_POLL;
      case(4):
        return SERIAL_TIME_SYNC;
      case(5):
        return ENABLE_UNSOLICITED;
      case(6):
        return AUTO_EVENT_SCAN;
      case(7):
        return USER_TASK;
      case(8):
        return SET_SESSION_KEYS;
      default:
        return CLEAR_RESTART;
    }
  }
}
