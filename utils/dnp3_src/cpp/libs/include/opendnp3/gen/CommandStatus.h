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

#ifndef OPENDNP3_COMMANDSTATUS_H
#define OPENDNP3_COMMANDSTATUS_H

#include <cstdint>

namespace opendnp3 {

/**
  An enumeration of result codes received from an outstation in response to command request.
  These correspond to those defined in the DNP3 standard
*/
enum class CommandStatus : uint8_t
{
  /// command was accepted, initiated, or queued
  SUCCESS = 0,
  /// command timed out before completing
  TIMEOUT = 1,
  /// command requires being selected before operate, configuration issue
  NO_SELECT = 2,
  /// bad control code or timing values
  FORMAT_ERROR = 3,
  /// command is not implemented
  NOT_SUPPORTED = 4,
  /// command is all ready in progress or its all ready in that mode
  ALREADY_ACTIVE = 5,
  /// something is stopping the command, often a local/remote interlock
  HARDWARE_ERROR = 6,
  /// the function governed by the control is in local only control
  LOCAL = 7,
  /// the command has been done too often and has been throttled
  TOO_MANY_OPS = 8,
  /// the command was rejected because the device denied it or an RTU intercepted it
  NOT_AUTHORIZED = 9,
  /// command not accepted because it was prevented or inhibited by a local automation process, such as interlocking logic or synchrocheck
  AUTOMATION_INHIBIT = 10,
  /// command not accepted because the device cannot process any more activities than are presently in progress
  PROCESSING_LIMITED = 11,
  /// command not accepted because the value is outside the acceptable range permitted for this point
  OUT_OF_RANGE = 12,
  /// command not accepted because the outstation is forwarding the request to another downstream device which reported LOCAL
  DOWNSTREAM_LOCAL = 13,
  /// command not accepted because the outstation has already completed the requested operation
  ALREADY_COMPLETE = 14,
  /// command not accepted because the requested function is specifically blocked at the outstation
  BLOCKED = 15,
  /// command not accepted because the operation was cancelled
  CANCELLED = 16,
  /// command not accepted because another master is communicating with the outstation and has exclusive rights to operate this control point
  BLOCKED_OTHER_MASTER = 17,
  /// command not accepted because the outstation is forwarding the request to another downstream device which cannot be reached or is otherwise incapable of performing the request
  DOWNSTREAM_FAIL = 18,
  /// (deprecated) indicates the outstation shall not issue or perform the control operation
  NON_PARTICIPATING = 126,
  /// 10 to 126 are currently reserved
  UNDEFINED = 127
};

uint8_t CommandStatusToType(CommandStatus arg);
CommandStatus CommandStatusFromType(uint8_t arg);
char const* CommandStatusToString(CommandStatus arg);

}

#endif
