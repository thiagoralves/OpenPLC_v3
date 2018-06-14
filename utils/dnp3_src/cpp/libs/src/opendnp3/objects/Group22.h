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

#ifndef OPENDNP3_GROUP22_H
#define OPENDNP3_GROUP22_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/MeasurementTypeSpecs.h"

namespace opendnp3 {

// Counter Event - Any Variation
struct Group22Var0
{
  static GroupVariationID ID() { return GroupVariationID(22,0); }
};

// Counter Event - 32-bit With Flag
struct Group22Var1
{
  static GroupVariationID ID() { return GroupVariationID(22,1); }

  Group22Var1();

  static uint32_t Size() { return 5; }
  static bool Read(openpal::RSlice&, Group22Var1&);
  static bool Write(const Group22Var1&, openpal::WSlice&);

  typedef uint32_t ValueType;
  uint8_t flags;
  uint32_t value;

  typedef Counter Target;
  typedef CounterSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Counter&);
  static bool WriteTarget(const Counter&, openpal::WSlice&);
  static DNP3Serializer<Counter> Inst() { return DNP3Serializer<Counter>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Counter Event - 16-bit With Flag
struct Group22Var2
{
  static GroupVariationID ID() { return GroupVariationID(22,2); }

  Group22Var2();

  static uint32_t Size() { return 3; }
  static bool Read(openpal::RSlice&, Group22Var2&);
  static bool Write(const Group22Var2&, openpal::WSlice&);

  typedef uint16_t ValueType;
  uint8_t flags;
  uint16_t value;

  typedef Counter Target;
  typedef CounterSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Counter&);
  static bool WriteTarget(const Counter&, openpal::WSlice&);
  static DNP3Serializer<Counter> Inst() { return DNP3Serializer<Counter>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Counter Event - 32-bit With Flag and Time
struct Group22Var5
{
  static GroupVariationID ID() { return GroupVariationID(22,5); }

  Group22Var5();

  static uint32_t Size() { return 11; }
  static bool Read(openpal::RSlice&, Group22Var5&);
  static bool Write(const Group22Var5&, openpal::WSlice&);

  typedef uint32_t ValueType;
  uint8_t flags;
  uint32_t value;
  DNPTime time;

  typedef Counter Target;
  typedef CounterSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Counter&);
  static bool WriteTarget(const Counter&, openpal::WSlice&);
  static DNP3Serializer<Counter> Inst() { return DNP3Serializer<Counter>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Counter Event - 16-bit With Flag and Time
struct Group22Var6
{
  static GroupVariationID ID() { return GroupVariationID(22,6); }

  Group22Var6();

  static uint32_t Size() { return 9; }
  static bool Read(openpal::RSlice&, Group22Var6&);
  static bool Write(const Group22Var6&, openpal::WSlice&);

  typedef uint16_t ValueType;
  uint8_t flags;
  uint16_t value;
  DNPTime time;

  typedef Counter Target;
  typedef CounterSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Counter&);
  static bool WriteTarget(const Counter&, openpal::WSlice&);
  static DNP3Serializer<Counter> Inst() { return DNP3Serializer<Counter>(ID(), Size(), &ReadTarget, &WriteTarget); }
};


}

#endif
