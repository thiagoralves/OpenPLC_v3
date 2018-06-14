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

#ifndef OPENDNP3_GROUP50_H
#define OPENDNP3_GROUP50_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/MeasurementTypeSpecs.h"

namespace opendnp3 {

// Time and Date - Absolute Time
struct Group50Var1
{
  static GroupVariationID ID() { return GroupVariationID(50,1); }

  Group50Var1();

  static uint32_t Size() { return 6; }
  static bool Read(openpal::RSlice&, Group50Var1&);
  static bool Write(const Group50Var1&, openpal::WSlice&);

  DNPTime time;
};

// Time and Date - Indexed absolute time and long interval
struct Group50Var4
{
  static GroupVariationID ID() { return GroupVariationID(50,4); }

  Group50Var4();

  static uint32_t Size() { return 11; }
  static bool Read(openpal::RSlice&, Group50Var4&);
  static bool Write(const Group50Var4&, openpal::WSlice&);

  DNPTime time;
  uint32_t interval;
  uint8_t units;

  typedef TimeAndInterval Target;
  typedef TimeAndIntervalSpec Spec;
  static bool ReadTarget(openpal::RSlice&, TimeAndInterval&);
  static bool WriteTarget(const TimeAndInterval&, openpal::WSlice&);
  static DNP3Serializer<TimeAndInterval> Inst() { return DNP3Serializer<TimeAndInterval>(ID(), Size(), &ReadTarget, &WriteTarget); }
};


}

#endif
