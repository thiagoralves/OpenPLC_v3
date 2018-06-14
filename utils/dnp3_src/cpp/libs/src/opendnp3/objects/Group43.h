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

#ifndef OPENDNP3_GROUP43_H
#define OPENDNP3_GROUP43_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/AnalogCommandEvent.h"

namespace opendnp3 {

// Analog Command Event - 32-bit
struct Group43Var1
{
  static GroupVariationID ID() { return GroupVariationID(43,1); }

  Group43Var1();

  static uint32_t Size() { return 5; }
  static bool Read(openpal::RSlice&, Group43Var1&);
  static bool Write(const Group43Var1&, openpal::WSlice&);

  typedef int32_t ValueType;
  uint8_t status;
  int32_t value;

  typedef AnalogCommandEvent Target;
  static bool ReadTarget(openpal::RSlice&, AnalogCommandEvent&);
  static bool WriteTarget(const AnalogCommandEvent&, openpal::WSlice&);
  static DNP3Serializer<AnalogCommandEvent> Inst() { return DNP3Serializer<AnalogCommandEvent>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Command Event - 16-bit
struct Group43Var2
{
  static GroupVariationID ID() { return GroupVariationID(43,2); }

  Group43Var2();

  static uint32_t Size() { return 3; }
  static bool Read(openpal::RSlice&, Group43Var2&);
  static bool Write(const Group43Var2&, openpal::WSlice&);

  typedef int16_t ValueType;
  uint8_t status;
  int16_t value;

  typedef AnalogCommandEvent Target;
  static bool ReadTarget(openpal::RSlice&, AnalogCommandEvent&);
  static bool WriteTarget(const AnalogCommandEvent&, openpal::WSlice&);
  static DNP3Serializer<AnalogCommandEvent> Inst() { return DNP3Serializer<AnalogCommandEvent>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Command Event - 32-bit With Time
struct Group43Var3
{
  static GroupVariationID ID() { return GroupVariationID(43,3); }

  Group43Var3();

  static uint32_t Size() { return 11; }
  static bool Read(openpal::RSlice&, Group43Var3&);
  static bool Write(const Group43Var3&, openpal::WSlice&);

  typedef int32_t ValueType;
  uint8_t status;
  int32_t value;
  DNPTime time;

  typedef AnalogCommandEvent Target;
  static bool ReadTarget(openpal::RSlice&, AnalogCommandEvent&);
  static bool WriteTarget(const AnalogCommandEvent&, openpal::WSlice&);
  static DNP3Serializer<AnalogCommandEvent> Inst() { return DNP3Serializer<AnalogCommandEvent>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Command Event - 16-bit With Time
struct Group43Var4
{
  static GroupVariationID ID() { return GroupVariationID(43,4); }

  Group43Var4();

  static uint32_t Size() { return 9; }
  static bool Read(openpal::RSlice&, Group43Var4&);
  static bool Write(const Group43Var4&, openpal::WSlice&);

  typedef int16_t ValueType;
  uint8_t status;
  int16_t value;
  DNPTime time;

  typedef AnalogCommandEvent Target;
  static bool ReadTarget(openpal::RSlice&, AnalogCommandEvent&);
  static bool WriteTarget(const AnalogCommandEvent&, openpal::WSlice&);
  static DNP3Serializer<AnalogCommandEvent> Inst() { return DNP3Serializer<AnalogCommandEvent>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Command Event - Single-precision
struct Group43Var5
{
  static GroupVariationID ID() { return GroupVariationID(43,5); }

  Group43Var5();

  static uint32_t Size() { return 5; }
  static bool Read(openpal::RSlice&, Group43Var5&);
  static bool Write(const Group43Var5&, openpal::WSlice&);

  typedef float ValueType;
  uint8_t status;
  float value;

  typedef AnalogCommandEvent Target;
  static bool ReadTarget(openpal::RSlice&, AnalogCommandEvent&);
  static bool WriteTarget(const AnalogCommandEvent&, openpal::WSlice&);
  static DNP3Serializer<AnalogCommandEvent> Inst() { return DNP3Serializer<AnalogCommandEvent>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Command Event - Double-precision
struct Group43Var6
{
  static GroupVariationID ID() { return GroupVariationID(43,6); }

  Group43Var6();

  static uint32_t Size() { return 9; }
  static bool Read(openpal::RSlice&, Group43Var6&);
  static bool Write(const Group43Var6&, openpal::WSlice&);

  typedef double ValueType;
  uint8_t status;
  double value;

  typedef AnalogCommandEvent Target;
  static bool ReadTarget(openpal::RSlice&, AnalogCommandEvent&);
  static bool WriteTarget(const AnalogCommandEvent&, openpal::WSlice&);
  static DNP3Serializer<AnalogCommandEvent> Inst() { return DNP3Serializer<AnalogCommandEvent>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Command Event - Single-precision With Time
struct Group43Var7
{
  static GroupVariationID ID() { return GroupVariationID(43,7); }

  Group43Var7();

  static uint32_t Size() { return 11; }
  static bool Read(openpal::RSlice&, Group43Var7&);
  static bool Write(const Group43Var7&, openpal::WSlice&);

  typedef float ValueType;
  uint8_t status;
  float value;
  DNPTime time;

  typedef AnalogCommandEvent Target;
  static bool ReadTarget(openpal::RSlice&, AnalogCommandEvent&);
  static bool WriteTarget(const AnalogCommandEvent&, openpal::WSlice&);
  static DNP3Serializer<AnalogCommandEvent> Inst() { return DNP3Serializer<AnalogCommandEvent>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Analog Command Event - Double-precision With Time
struct Group43Var8
{
  static GroupVariationID ID() { return GroupVariationID(43,8); }

  Group43Var8();

  static uint32_t Size() { return 15; }
  static bool Read(openpal::RSlice&, Group43Var8&);
  static bool Write(const Group43Var8&, openpal::WSlice&);

  typedef double ValueType;
  uint8_t status;
  double value;
  DNPTime time;

  typedef AnalogCommandEvent Target;
  static bool ReadTarget(openpal::RSlice&, AnalogCommandEvent&);
  static bool WriteTarget(const AnalogCommandEvent&, openpal::WSlice&);
  static DNP3Serializer<AnalogCommandEvent> Inst() { return DNP3Serializer<AnalogCommandEvent>(ID(), Size(), &ReadTarget, &WriteTarget); }
};


}

#endif
