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
  /// List the various states that an individual command object can be in after an SBO or direct operate request
  /// </summary>
  public enum CommandPointState : byte
  {
    /// <summary>
    /// No corresponding response was ever received for this command point
    /// </summary>
    INIT = 0,
    /// <summary>
    /// A response to a select request was received and matched, but the operate did not complete
    /// </summary>
    SELECT_SUCCESS = 1,
    /// <summary>
    /// A response to a select operation did not contain the same value that was sent
    /// </summary>
    SELECT_MISMATCH = 2,
    /// <summary>
    /// A response to a select operation contained a command status other than success
    /// </summary>
    SELECT_FAIL = 3,
    /// <summary>
    /// A response to an operate or direct operate did not match the request
    /// </summary>
    OPERATE_FAIL = 4,
    /// <summary>
    /// A matching response was received to the operate
    /// </summary>
    SUCCESS = 5
  }
}
