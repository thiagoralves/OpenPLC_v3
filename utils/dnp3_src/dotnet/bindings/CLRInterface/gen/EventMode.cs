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
  /// Describes how a transaction behaves with respect to event generation
  /// </summary>
  public enum EventMode : byte
  {
    /// <summary>
    /// Detect events using the specific mechanism for that type
    /// </summary>
    Detect = 0x0,
    /// <summary>
    /// Force the creation of an event bypassing detection mechanism
    /// </summary>
    Force = 0x1,
    /// <summary>
    /// Never produce an event regardless of changes
    /// </summary>
    Suppress = 0x2
  }
}
