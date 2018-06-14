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
* Select contiguous or dis-contiguous index mode
*/
public enum IndexMode
{
  /**
  * Indices are contiguous. Most efficient as direct indexing is used.
  */
  Contiguous(0x0),
  /**
  * Indices are dis-contiguous. Resorts to binary search to find raw index.
  */
  Discontiguous(0x1);

  private final int id;

  public int toType()
  {
    return id;
  }

  IndexMode(int id)
  {
    this.id = id;
  }

  public static IndexMode fromType(int arg)
  {
    switch(arg)
    {
      case(0x0):
        return Contiguous;
      case(0x1):
        return Discontiguous;
      default:
        return Contiguous;
    }
  }
}
