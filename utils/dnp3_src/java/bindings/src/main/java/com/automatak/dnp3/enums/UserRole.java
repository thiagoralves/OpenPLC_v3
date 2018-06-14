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
* Enumerates pre-defined rules in secure authentication
*/
public enum UserRole
{
  VIEWER(0),
  OPERATOR(1),
  ENGINEER(2),
  INSTALLER(3),
  SECADM(4),
  SECAUD(5),
  RBACMNT(6),
  SINGLE_USER(32768),
  UNDEFINED(32767);

  private final int id;

  public int toType()
  {
    return id;
  }

  UserRole(int id)
  {
    this.id = id;
  }

  public static UserRole fromType(int arg)
  {
    switch(arg)
    {
      case(0):
        return VIEWER;
      case(1):
        return OPERATOR;
      case(2):
        return ENGINEER;
      case(3):
        return INSTALLER;
      case(4):
        return SECADM;
      case(5):
        return SECAUD;
      case(6):
        return RBACMNT;
      case(32768):
        return SINGLE_USER;
      default:
        return UNDEFINED;
    }
  }
}
