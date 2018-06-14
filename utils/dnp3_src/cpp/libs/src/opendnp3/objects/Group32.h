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

#ifndef OPENDNP3_GROUP32_H
#define OPENDNP3_GROUP32_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/MeasurementTypeSpecs.h"

namespace opendnp3 {

// Analog Input Event - Any Variation
struct Group32Var0
{
  static GroupVariationID ID() { return GroupVariationID(32,0); }
};

// Analog Input Event - 32-bit With Flag
struct Group32Var1
{
  static GroupVariationID ID() { return GroupVariationID(32,1); }

  Group32Var1();

  static uint32_t Size() { return 5; }
  static bool Read(openpal::RSlice&, Group32Var1&);
  static bool Write(const Group32Var1&, openpal::WSlice&);

  typedef int32_t ValueType;
  uint8_t flags;
  int32_t value;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input Event - 16-bit With Flag
struct Group32Var2
{
  static GroupVariationID ID() { return GroupVariationID(32,2); }

  Group32Var2();

  static uint32_t Size() { return 3; }
  static bool Read(openpal::RSlice&, Group32Var2&);
  static bool Write(const Group32Var2&, openpal::WSlice&);

  typedef int16_t ValueType;
  uint8_t flags;
  int16_t value;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input Event - 32-bit With Flag and Time
struct Group32Var3
{
  static GroupVariationID ID() { return GroupVariationID(32,3); }

  Group32Var3();

  static uint32_t Size() { return 11; }
  static bool Read(openpal::RSlice&, Group32Var3&);
  static bool Write(const Group32Var3&, openpal::WSlice&);

  typedef int32_t ValueType;
  uint8_t flags;
  int32_t value;
  DNPTime time;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input Event - 16-bit With Flag and Time
struct Group32Var4
{
  static GroupVariationID ID() { return GroupVariationID(32,4); }

  Group32Var4();

  static uint32_t Size() { return 9; }
  static bool Read(openpal::RSlice&, Group32Var4&);
  static bool Write(const Group32Var4&, openpal::WSlice&);

  typedef int16_t ValueType;
  uint8_t flags;
  int16_t value;
  DNPTime time;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input Event - Single-precision With Flag
struct Group32Var5
{
  static GroupVariationID ID() { return GroupVariationID(32,5); }

  Group32Var5();

  static uint32_t Size() { return 5; }
  static bool Read(openpal::RSlice&, Group32Var5&);
  static bool Write(const Group32Var5&, openpal::WSlice&);

  typedef float ValueType;
  uint8_t flags;
  float value;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input Event - Double-precision With Flag
struct Group32Var6
{
  static GroupVariationID ID() { return GroupVariationID(32,6); }

  Group32Var6();

  static uint32_t Size() { return 9; }
  static bool Read(openpal::RSlice&, Group32Var6&);
  static bool Write(const Group32Var6&, openpal::WSlice&);

  typedef double ValueType;
  uint8_t flags;
  double value;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input Event - Single-precision With Flag and Time
struct Group32Var7
{
  static GroupVariationID ID() { return GroupVariationID(32,7); }

  Group32Var7();

  static uint32_t Size() { return 11; }
  static bool Read(openpal::RSlice&, Group32Var7&);
  static bool Write(const Group32Var7&, openpal::WSlice&);

  typedef float ValueType;
  uint8_t flags;
  float value;
  DNPTime time;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Input Event - Double-precision With Flag and Time
struct Group32Var8
{
  static GroupVariationID ID() { return GroupVariationID(32,8); }

  Group32Var8();

  static uint32_t Size() { return 15; }
  static bool Read(openpal::RSlice&, Group32Var8&);
  static bool Write(const Group32Var8&, openpal::WSlice&);

  typedef double ValueType;
  uint8_t flags;
  double value;
  DNPTime time;

  typedef Analog Target;
  typedef AnalogSpec Spec;
  static bool ReadTarget(openpal::RSlice&, Analog&);
  static bool WriteTarget(const Analog&, openpal::WSlice&);
  static DNP3Serializer<Analog> Inst() { return DNP3Serializer<Analog>(ID(), Size(), &ReadTarget, &WriteTarget); }
};


}

#endif
