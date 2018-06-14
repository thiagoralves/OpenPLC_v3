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
  /// Enumeration describing restart mode support of an outstation
  /// </summary>
  public enum RestartMode : byte
  {
    /// <summary>
    /// Device does not support restart
    /// </summary>
    UNSUPPORTED = 0,
    /// <summary>
    /// Supports restart, and time returned is a fine time delay
    /// </summary>
    SUPPORTED_DELAY_FINE = 1,
    /// <summary>
    /// Supports restart, and time returned is a coarse time delay
    /// </summary>
    SUPPORTED_DELAY_COARSE = 2
  }
}
