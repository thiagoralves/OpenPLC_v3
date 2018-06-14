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
  /// Quality field bitmask for analog values
  /// </summary>
  public enum AnalogQuality : byte
  {
    /// <summary>
    /// set when the data is "good", meaning that rest of the system can trust the value
    /// </summary>
    ONLINE = 0x1,
    /// <summary>
    /// the quality all points get before we have established communication (or populated) the point
    /// </summary>
    RESTART = 0x2,
    /// <summary>
    /// set if communication has been lost with the source of the data (after establishing contact)
    /// </summary>
    COMM_LOST = 0x4,
    /// <summary>
    /// set if the value is being forced to a "fake" value somewhere in the system
    /// </summary>
    REMOTE_FORCED = 0x8,
    /// <summary>
    /// set if the value is being forced to a "fake" value on the original device
    /// </summary>
    LOCAL_FORCED = 0x10,
    /// <summary>
    /// set if a hardware input etc. is out of range and we are using a place holder value
    /// </summary>
    OVERRANGE = 0x20,
    /// <summary>
    /// set if calibration or reference voltage has been lost meaning readings are questionable
    /// </summary>
    REFERENCE_ERR = 0x40,
    /// <summary>
    /// reserved bit
    /// </summary>
    RESERVED = 0x80
  }
}
