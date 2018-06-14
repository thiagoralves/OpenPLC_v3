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

#include "opendnp3/gen/CommandStatus.h"

namespace opendnp3 {

uint8_t CommandStatusToType(CommandStatus arg)
{
  return static_cast<uint8_t>(arg);
}
CommandStatus CommandStatusFromType(uint8_t arg)
{
  switch(arg)
  {
    case(0):
      return CommandStatus::SUCCESS;
    case(1):
      return CommandStatus::TIMEOUT;
    case(2):
      return CommandStatus::NO_SELECT;
    case(3):
      return CommandStatus::FORMAT_ERROR;
    case(4):
      return CommandStatus::NOT_SUPPORTED;
    case(5):
      return CommandStatus::ALREADY_ACTIVE;
    case(6):
      return CommandStatus::HARDWARE_ERROR;
    case(7):
      return CommandStatus::LOCAL;
    case(8):
      return CommandStatus::TOO_MANY_OPS;
    case(9):
      return CommandStatus::NOT_AUTHORIZED;
    case(10):
      return CommandStatus::AUTOMATION_INHIBIT;
    case(11):
      return CommandStatus::PROCESSING_LIMITED;
    case(12):
      return CommandStatus::OUT_OF_RANGE;
    case(13):
      return CommandStatus::DOWNSTREAM_LOCAL;
    case(14):
      return CommandStatus::ALREADY_COMPLETE;
    case(15):
      return CommandStatus::BLOCKED;
    case(16):
      return CommandStatus::CANCELLED;
    case(17):
      return CommandStatus::BLOCKED_OTHER_MASTER;
    case(18):
      return CommandStatus::DOWNSTREAM_FAIL;
    case(126):
      return CommandStatus::NON_PARTICIPATING;
    default:
      return CommandStatus::UNDEFINED;
  }
}
char const* CommandStatusToString(CommandStatus arg)
{
  switch(arg)
  {
    case(CommandStatus::SUCCESS):
      return "SUCCESS";
    case(CommandStatus::TIMEOUT):
      return "TIMEOUT";
    case(CommandStatus::NO_SELECT):
      return "NO_SELECT";
    case(CommandStatus::FORMAT_ERROR):
      return "FORMAT_ERROR";
    case(CommandStatus::NOT_SUPPORTED):
      return "NOT_SUPPORTED";
    case(CommandStatus::ALREADY_ACTIVE):
      return "ALREADY_ACTIVE";
    case(CommandStatus::HARDWARE_ERROR):
      return "HARDWARE_ERROR";
    case(CommandStatus::LOCAL):
      return "LOCAL";
    case(CommandStatus::TOO_MANY_OPS):
      return "TOO_MANY_OPS";
    case(CommandStatus::NOT_AUTHORIZED):
      return "NOT_AUTHORIZED";
    case(CommandStatus::AUTOMATION_INHIBIT):
      return "AUTOMATION_INHIBIT";
    case(CommandStatus::PROCESSING_LIMITED):
      return "PROCESSING_LIMITED";
    case(CommandStatus::OUT_OF_RANGE):
      return "OUT_OF_RANGE";
    case(CommandStatus::DOWNSTREAM_LOCAL):
      return "DOWNSTREAM_LOCAL";
    case(CommandStatus::ALREADY_COMPLETE):
      return "ALREADY_COMPLETE";
    case(CommandStatus::BLOCKED):
      return "BLOCKED";
    case(CommandStatus::CANCELLED):
      return "CANCELLED";
    case(CommandStatus::BLOCKED_OTHER_MASTER):
      return "BLOCKED_OTHER_MASTER";
    case(CommandStatus::DOWNSTREAM_FAIL):
      return "DOWNSTREAM_FAIL";
    case(CommandStatus::NON_PARTICIPATING):
      return "NON_PARTICIPATING";
    default:
      return "UNDEFINED";
  }
}

}
