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

#ifndef OPENDNP3_GROUP11_H
#define OPENDNP3_GROUP11_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/MeasurementTypeSpecs.h"

namespace opendnp3 {

// Binary Output Event - Any Variation
struct Group11Var0
{
  static GroupVariationID ID() { return GroupVariationID(11,0); }
};

// Binary Output Event - Output Status Without Time
struct Group11Var1
{
  static GroupVariationID ID() { return GroupVariationID(11,1); }

  Group11Var1();

  static uint32_t Size() { return 1; }
  static bool Read(openpal::RSlice&, Group11Var1&);
  static bool Write(const Group11Var1&, openpal::WSlice&);

  uint8_t flags;

  typedef BinaryOutputStatus Target;
  typedef BinaryOutputStatusSpec Spec;
  static bool ReadTarget(openpal::RSlice&, BinaryOutputStatus&);
  static bool WriteTarget(const BinaryOutputStatus&, openpal::WSlice&);
  static DNP3Serializer<BinaryOutputStatus> Inst() { return DNP3Serializer<BinaryOutputStatus>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Binary Output Event - Output Status With Time
struct Group11Var2
{
  static GroupVariationID ID() { return GroupVariationID(11,2); }

  Group11Var2();

  static uint32_t Size() { return 7; }
  static bool Read(openpal::RSlice&, Group11Var2&);
  static bool Write(const Group11Var2&, openpal::WSlice&);

  uint8_t flags;
  DNPTime time;

  typedef BinaryOutputStatus Target;
  typedef BinaryOutputStatusSpec Spec;
  static bool ReadTarget(openpal::RSlice&, BinaryOutputStatus&);
  static bool WriteTarget(const BinaryOutputStatus&, openpal::WSlice&);
  static DNP3Serializer<BinaryOutputStatus> Inst() { return DNP3Serializer<BinaryOutputStatus>(ID(), Size(), &ReadTarget, &WriteTarget); }
};


}

#endif
