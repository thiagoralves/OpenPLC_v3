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

#ifndef OPENDNP3_GROUP41_H
#define OPENDNP3_GROUP41_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/AnalogOutput.h"

namespace opendnp3 {

// Analog Output - Any Variation
struct Group41Var0
{
  static GroupVariationID ID() { return GroupVariationID(41,0); }
};

// Analog Output - 32-bit With Flag
struct Group41Var1
{
  static GroupVariationID ID() { return GroupVariationID(41,1); }

  Group41Var1();

  static uint32_t Size() { return 5; }
  static bool Read(openpal::RSlice&, Group41Var1&);
  static bool Write(const Group41Var1&, openpal::WSlice&);

  typedef int32_t ValueType;
  int32_t value;
  uint8_t status;

  typedef AnalogOutputInt32 Target;
  static bool ReadTarget(openpal::RSlice&, AnalogOutputInt32&);
  static bool WriteTarget(const AnalogOutputInt32&, openpal::WSlice&);
  static DNP3Serializer<AnalogOutputInt32> Inst() { return DNP3Serializer<AnalogOutputInt32>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Output - 16-bit With Flag
struct Group41Var2
{
  static GroupVariationID ID() { return GroupVariationID(41,2); }

  Group41Var2();

  static uint32_t Size() { return 3; }
  static bool Read(openpal::RSlice&, Group41Var2&);
  static bool Write(const Group41Var2&, openpal::WSlice&);

  typedef int16_t ValueType;
  int16_t value;
  uint8_t status;

  typedef AnalogOutputInt16 Target;
  static bool ReadTarget(openpal::RSlice&, AnalogOutputInt16&);
  static bool WriteTarget(const AnalogOutputInt16&, openpal::WSlice&);
  static DNP3Serializer<AnalogOutputInt16> Inst() { return DNP3Serializer<AnalogOutputInt16>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Output - Single-precision
struct Group41Var3
{
  static GroupVariationID ID() { return GroupVariationID(41,3); }

  Group41Var3();

  static uint32_t Size() { return 5; }
  static bool Read(openpal::RSlice&, Group41Var3&);
  static bool Write(const Group41Var3&, openpal::WSlice&);

  typedef float ValueType;
  float value;
  uint8_t status;

  typedef AnalogOutputFloat32 Target;
  static bool ReadTarget(openpal::RSlice&, AnalogOutputFloat32&);
  static bool WriteTarget(const AnalogOutputFloat32&, openpal::WSlice&);
  static DNP3Serializer<AnalogOutputFloat32> Inst() { return DNP3Serializer<AnalogOutputFloat32>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Output - Double-precision
struct Group41Var4
{
  static GroupVariationID ID() { return GroupVariationID(41,4); }

  Group41Var4();

  static uint32_t Size() { return 9; }
  static bool Read(openpal::RSlice&, Group41Var4&);
  static bool Write(const Group41Var4&, openpal::WSlice&);

  typedef double ValueType;
  double value;
  uint8_t status;

  typedef AnalogOutputDouble64 Target;
  static bool ReadTarget(openpal::RSlice&, AnalogOutputDouble64&);
  static bool WriteTarget(const AnalogOutputDouble64&, openpal::WSlice&);
  static DNP3Serializer<AnalogOutputDouble64> Inst() { return DNP3Serializer<AnalogOutputDouble64>(ID(), Size(), &ReadTarget, &WriteTarget); }
};


}

#endif
