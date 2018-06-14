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
  /// An enumeration of result codes received from an outstation in response to command request.
  /// These correspond to those defined in the DNP3 standard
  /// </summary>
  public enum CommandStatus : byte
  {
    /// <summary>
    /// command was accepted, initiated, or queued
    /// </summary>
    SUCCESS = 0,
    /// <summary>
    /// command timed out before completing
    /// </summary>
    TIMEOUT = 1,
    /// <summary>
    /// command requires being selected before operate, configuration issue
    /// </summary>
    NO_SELECT = 2,
    /// <summary>
    /// bad control code or timing values
    /// </summary>
    FORMAT_ERROR = 3,
    /// <summary>
    /// command is not implemented
    /// </summary>
    NOT_SUPPORTED = 4,
    /// <summary>
    /// command is all ready in progress or its all ready in that mode
    /// </summary>
    ALREADY_ACTIVE = 5,
    /// <summary>
    /// something is stopping the command, often a local/remote interlock
    /// </summary>
    HARDWARE_ERROR = 6,
    /// <summary>
    /// the function governed by the control is in local only control
    /// </summary>
    LOCAL = 7,
    /// <summary>
    /// the command has been done too often and has been throttled
    /// </summary>
    TOO_MANY_OPS = 8,
    /// <summary>
    /// the command was rejected because the device denied it or an RTU intercepted it
    /// </summary>
    NOT_AUTHORIZED = 9,
    /// <summary>
    /// command not accepted because it was prevented or inhibited by a local automation process, such as interlocking logic or synchrocheck
    /// </summary>
    AUTOMATION_INHIBIT = 10,
    /// <summary>
    /// command not accepted because the device cannot process any more activities than are presently in progress
    /// </summary>
    PROCESSING_LIMITED = 11,
    /// <summary>
    /// command not accepted because the value is outside the acceptable range permitted for this point
    /// </summary>
    OUT_OF_RANGE = 12,
    /// <summary>
    /// command not accepted because the outstation is forwarding the request to another downstream device which reported LOCAL
    /// </summary>
    DOWNSTREAM_LOCAL = 13,
    /// <summary>
    /// command not accepted because the outstation has already completed the requested operation
    /// </summary>
    ALREADY_COMPLETE = 14,
    /// <summary>
    /// command not accepted because the requested function is specifically blocked at the outstation
    /// </summary>
    BLOCKED = 15,
    /// <summary>
    /// command not accepted because the operation was cancelled
    /// </summary>
    CANCELLED = 16,
    /// <summary>
    /// command not accepted because another master is communicating with the outstation and has exclusive rights to operate this control point
    /// </summary>
    BLOCKED_OTHER_MASTER = 17,
    /// <summary>
    /// command not accepted because the outstation is forwarding the request to another downstream device which cannot be reached or is otherwise incapable of performing the request
    /// </summary>
    DOWNSTREAM_FAIL = 18,
    /// <summary>
    /// (deprecated) indicates the outstation shall not issue or perform the control operation
    /// </summary>
    NON_PARTICIPATING = 126,
    /// <summary>
    /// 10 to 126 are currently reserved
    /// </summary>
    UNDEFINED = 127
  }
}
