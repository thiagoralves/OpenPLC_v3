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

#ifndef OPENDNP3_GROUP4_H
#define OPENDNP3_GROUP4_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/MeasurementTypeSpecs.h"

namespace opendnp3 {

// Double-bit Binary Input Event - Any Variation
struct Group4Var0
{
  static GroupVariationID ID() { return GroupVariationID(4,0); }
};

// Double-bit Binary Input Event - Without Time
struct Group4Var1
{
  static GroupVariationID ID() { return GroupVariationID(4,1); }

  Group4Var1();

  static uint32_t Size() { return 1; }
  static bool Read(openpal::RSlice&, Group4Var1&);
  static bool Write(const Group4Var1&, openpal::WSlice&);

  uint8_t flags;

  typedef DoubleBitBinary Target;
  typedef DoubleBitBinarySpec Spec;
  static bool ReadTarget(openpal::RSlice&, DoubleBitBinary&);
  static bool WriteTarget(const DoubleBitBinary&, openpal::WSlice&);
  static DNP3Serializer<DoubleBitBinary> Inst() { return DNP3Serializer<DoubleBitBinary>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Double-bit Binary Input Event - With Absolute Time
struct Group4Var2
{
  static GroupVariationID ID() { return GroupVariationID(4,2); }

  Group4Var2();

  static uint32_t Size() { return 7; }
  static bool Read(openpal::RSlice&, Group4Var2&);
  static bool Write(const Group4Var2&, openpal::WSlice&);

  uint8_t flags;
  DNPTime time;

  typedef DoubleBitBinary Target;
  typedef DoubleBitBinarySpec Spec;
  static bool ReadTarget(openpal::RSlice&, DoubleBitBinary&);
  static bool WriteTarget(const DoubleBitBinary&, openpal::WSlice&);
  static DNP3Serializer<DoubleBitBinary> Inst() { return DNP3Serializer<DoubleBitBinary>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Double-bit Binary Input Event - With Relative Time
struct Group4Var3
{
  static GroupVariationID ID() { return GroupVariationID(4,3); }

  Group4Var3();

  static uint32_t Size() { return 3; }
  static bool Read(openpal::RSlice&, Group4Var3&);
  static bool Write(const Group4Var3&, openpal::WSlice&);

  uint8_t flags;
  uint16_t time;

  typedef DoubleBitBinary Target;
  typedef DoubleBitBinarySpec Spec;
  static bool ReadTarget(openpal::RSlice&, DoubleBitBinary&);
  static bool WriteTarget(const DoubleBitBinary&, openpal::WSlice&);
  static DNP3Serializer<DoubleBitBinary> Inst() { return DNP3Serializer<DoubleBitBinary>(ID(), Size(), &ReadTarget, &WriteTarget); }
};


}

#endif
