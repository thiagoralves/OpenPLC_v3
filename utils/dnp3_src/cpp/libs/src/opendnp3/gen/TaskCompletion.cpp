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

#include "opendnp3/gen/TaskCompletion.h"

namespace opendnp3 {

char const* TaskCompletionToString(TaskCompletion arg)
{
  switch(arg)
  {
    case(TaskCompletion::SUCCESS):
      return "SUCCESS";
    case(TaskCompletion::FAILURE_BAD_RESPONSE):
      return "FAILURE_BAD_RESPONSE";
    case(TaskCompletion::FAILURE_RESPONSE_TIMEOUT):
      return "FAILURE_RESPONSE_TIMEOUT";
    case(TaskCompletion::FAILURE_START_TIMEOUT):
      return "FAILURE_START_TIMEOUT";
    case(TaskCompletion::FAILURE_NO_USER):
      return "FAILURE_NO_USER";
    case(TaskCompletion::FAILURE_INTERNAL_ERROR):
      return "FAILURE_INTERNAL_ERROR";
    case(TaskCompletion::FAILURE_BAD_AUTHENTICATION):
      return "FAILURE_BAD_AUTHENTICATION";
    case(TaskCompletion::FAILURE_NOT_AUTHORIZED):
      return "FAILURE_NOT_AUTHORIZED";
    default:
      return "FAILURE_NO_COMMS";
  }
}

}
