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

#ifndef OPENDNP3_GROUP30_H
#define OPENDNP3_GROUP30_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/MeasurementTypeSpecs.h"

namespace opendnp3 {

// Analog Input - Any Variation
struct Group30Var0
{
  static GroupVariationID ID() { return GroupVariationID(30,0); }
};

// Analog Input - 32-bit With Flag
struct Group30Var1
{
  static GroupVariationID ID() { return GroupVariationID(30,1); }

  Group30Var1();

  static uint32_t Size() { return 5; }
  static bool Read(openpal::RSlice&, Group30Var1&);
  static bool Write(const Group30Var1&, openpal::WSlice&);

  typedef int32_t ValueType;
  uint8_t flags;
  int32_t value;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input - 16-bit With Flag
struct Group30Var2
{
  static GroupVariationID ID() { return GroupVariationID(30,2); }

  Group30Var2();

  static uint32_t Size() { return 3; }
  static bool Read(openpal::RSlice&, Group30Var2&);
  static bool Write(const Group30Var2&, openpal::WSlice&);

  typedef int16_t ValueType;
  uint8_t flags;
  int16_t value;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input - 32-bit Without Flag
struct Group30Var3
{
  static GroupVariationID ID() { return GroupVariationID(30,3); }

  Group30Var3();

  static uint32_t Size() { return 4; }
  static bool Read(openpal::RSlice&, Group30Var3&);
  static bool Write(const Group30Var3&, openpal::WSlice&);

  typedef int32_t ValueType;
  int32_t value;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input - 16-bit Without Flag
struct Group30Var4
{
  static GroupVariationID ID() { return GroupVariationID(30,4); }

  Group30Var4();

  static uint32_t Size() { return 2; }
  static bool Read(openpal::RSlice&, Group30Var4&);
  static bool Write(const Group30Var4&, openpal::WSlice&);

  typedef int16_t ValueType;
  int16_t value;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input - Single-precision With Flag
struct Group30Var5
{
  static GroupVariationID ID() { return GroupVariationID(30,5); }

  Group30Var5();

  static uint32_t Size() { return 5; }
  static bool Read(openpal::RSlice&, Group30Var5&);
  static bool Write(const Group30Var5&, openpal::WSlice&);

  typedef float ValueType;
  uint8_t flags;
  float value;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input - Double-precision With Flag
struct Group30Var6
{
  static GroupVariationID ID() { return GroupVariationID(30,6); }

  Group30Var6();

  static uint32_t Size() { return 9; }
  static bool Read(openpal::RSlice&, Group30Var6&);
  static bool Write(const Group30Var6&, openpal::WSlice&);

  typedef double ValueType;
  uint8_t flags;
  double value;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};


}

#endif
