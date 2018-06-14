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
  /// Indicates the validity of timestamp values for an entire object header
  /// </summary>
  public enum TimestampMode : byte
  {
    /// <summary>
    /// The timestamp is UTC synchronized at the remote device
    /// </summary>
    SYNCHRONIZED = 1,
    /// <summary>
    /// The device indicate the timestamp may be unsynchronized
    /// </summary>
    UNSYNCHRONIZED = 2,
    /// <summary>
    /// Timestamp is not valid, ignore the value and use a local timestamp
    /// </summary>
    INVALID = 0
  }
}
