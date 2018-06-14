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
  /// Enumeration for possible states of a double bit value
  /// </summary>
  public enum DoubleBit : byte
  {
    /// <summary>
    /// Transitioning between end conditions
    /// </summary>
    INTERMEDIATE = 0x0,
    /// <summary>
    /// End condition, determined to be OFF
    /// </summary>
    DETERMINED_OFF = 0x1,
    /// <summary>
    /// End condition, determined to be ON
    /// </summary>
    DETERMINED_ON = 0x2,
    /// <summary>
    /// Abnormal or custom condition
    /// </summary>
    INDETERMINATE = 0x3
  }
}
