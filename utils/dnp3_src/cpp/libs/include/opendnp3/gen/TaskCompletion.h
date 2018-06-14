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

#ifndef OPENDNP3_TASKCOMPLETION_H
#define OPENDNP3_TASKCOMPLETION_H

#include <cstdint>

namespace opendnp3 {

/**
  Enum that describes if a master task succeeded or failed
*/
enum class TaskCompletion : uint8_t
{
  /// A valid response was received from the outstation
  SUCCESS = 0,
  /// A response was received from the outstation, but it was not valid
  FAILURE_BAD_RESPONSE = 1,
  /// The task request did not receive a response within the timeout
  FAILURE_RESPONSE_TIMEOUT = 2,
  /// The start timeout expired before the task could begin running
  FAILURE_START_TIMEOUT = 3,
  /// The task could not run because the specified user was not defined on the master (SA only)
  FAILURE_NO_USER = 4,
  /// The task failed because of some unexpected internal issue like bad configuration data
  FAILURE_INTERNAL_ERROR = 5,
  /// The outstation rejected the operation do to a lack of valid session keys or the user not existing on the outstation (SA only)
  FAILURE_BAD_AUTHENTICATION = 6,
  /// The outstation rejected the operation because the specified user is not authorized for the request that was made (SA only)
  FAILURE_NOT_AUTHORIZED = 7,
  /// There is no communication with the outstation, so the task was not attempted
  FAILURE_NO_COMMS = 255
};

char const* TaskCompletionToString(TaskCompletion arg);

}

#endif
