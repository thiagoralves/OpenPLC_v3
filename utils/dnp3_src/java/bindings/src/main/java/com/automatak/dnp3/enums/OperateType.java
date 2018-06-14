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
* Various ways that an outstation can receive a request to operate a BO or AO point
*/
public enum OperateType
{
  /**
  * The outstation received a valid prior SELECT followed by OPERATE
  */
  SelectBeforeOperate(0x0),
  /**
  * The outstation received a direct operate request
  */
  DirectOperate(0x1),
  /**
  * The outstation received a direct operate no ack request
  */
  DirectOperateNoAck(0x2);

  private final int id;

  public int toType()
  {
    return id;
  }

  OperateType(int id)
  {
    this.id = id;
  }

  public static OperateType fromType(int arg)
  {
    switch(arg)
    {
      case(0x0):
        return SelectBeforeOperate;
      case(0x1):
        return DirectOperate;
      case(0x2):
        return DirectOperateNoAck;
      default:
        return SelectBeforeOperate;
    }
  }
}
