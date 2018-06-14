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

namespace Automatak.DNP3.Interface
{
  /// <summary>
  /// Link layer function code enumeration
  /// </summary>
  public enum LinkFunction : byte
  {
    PRI_RESET_LINK_STATES = 0x40,
    PRI_TEST_LINK_STATES = 0x42,
    PRI_CONFIRMED_USER_DATA = 0x43,
    PRI_UNCONFIRMED_USER_DATA = 0x44,
    PRI_REQUEST_LINK_STATUS = 0x49,
    SEC_ACK = 0x0,
    SEC_NACK = 0x1,
    SEC_LINK_STATUS = 0xB,
    SEC_NOT_SUPPORTED = 0xF,
    INVALID = 0xFF
  }
}
