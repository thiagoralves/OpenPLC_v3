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

#ifndef OPENDNP3_CONTROLCODE_H
#define OPENDNP3_CONTROLCODE_H

#include <cstdint>

namespace opendnp3 {

/**
  Defines the interoperable values of the Control Code
  Refer to pages 506-510 of 1815 for a full description
*/
enum class ControlCode : uint8_t
{
  /// Does not initiate an action or change an in-progress or pending command.
  NUL = 0x0,
  /// Cancel in-progress and pending commands. Take no additional action.
  NUL_CANCEL = 0x20,
  /// For activation model, set output to active for the duration of the On-time. For both complementary models, return NOT_SUPPORTED status.
  PULSE_ON = 0x1,
  /// Cancel in-progress and pending commands, process the remainder of the fields as if the control code were PULSE_ON
  PULSE_ON_CANCEL = 0x21,
  /// Non-interoperable code. Do not use for new applications. return NOT_SUPPORTED
  PULSE_OFF = 0x2,
  /// Non-interoperable code. Do not use for new applications. Cancel in-progress and pending commands, process remainder of fields as if the control code were PULSE_OFF
  PULSE_OFF_CANCEL = 0x22,
  /// For activation model, set output to active for the duration of the On-time. For complementary latch model, set the output to active. For complementary two-output model, set the close output to active for the duration of the On-time.
  LATCH_ON = 0x3,
  /// Cancel in-progress and pending commands, process the remainder of the fields as if the control code were LATCH_ON.
  LATCH_ON_CANCEL = 0x23,
  /// For activation model, set output to active for the duration of the On-time. For complementary latch model, set the output to inactive. For complementary two-output model, set the trip output to active for the duration of the On-time.
  LATCH_OFF = 0x4,
  /// Cancel in-progress and pending commands, process the remainder of the fields as if the control code were LATCH_OFF.
  LATCH_OFF_CANCEL = 0x24,
  /// For activation model, set output to active for the duration of the On-time. For complementary latch model, set the output to active. For complementary two-output model, set the close output to active for the duration of the On-time.
  CLOSE_PULSE_ON = 0x41,
  /// Cancel in-progress and pending commands, process the remainder of the fields as if the control code were CLOSE_PULSE_ON.
  CLOSE_PULSE_ON_CANCEL = 0x61,
  /// For activation model, set output to active for the duration of the On-time. For complementary latch model, set the output to inactive. For complementary two-output model, set the trip output to active for the duration of the On-time.
  TRIP_PULSE_ON = 0x81,
  /// Cancel in-progress and pending commands, process the remainder of the fields as if the control code were TRIP_PULSE_ON.
  TRIP_PULSE_ON_CANCEL = 0xA1,
  ///  undefined command (used by DNP standard)
  UNDEFINED = 0xFF
};

uint8_t ControlCodeToType(ControlCode arg);
ControlCode ControlCodeFromType(uint8_t arg);
char const* ControlCodeToString(ControlCode arg);

}

#endif
