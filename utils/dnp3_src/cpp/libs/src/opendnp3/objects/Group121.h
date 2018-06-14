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

#ifndef OPENDNP3_GROUP121_H
#define OPENDNP3_GROUP121_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/MeasurementTypeSpecs.h"

namespace opendnp3 {

// Security statistic - Any Variation
struct Group121Var0
{
  static GroupVariationID ID() { return GroupVariationID(121,0); }
};

// Security statistic - 32-bit With Flag
struct Group121Var1
{
  static GroupVariationID ID() { return GroupVariationID(121,1); }

  Group121Var1();

  static uint32_t Size() { return 7; }
  static bool Read(openpal::RSlice&, Group121Var1&);
  static bool Write(const Group121Var1&, openpal::WSlice&);

  typedef uint32_t ValueType;
  uint8_t flags;
  uint16_t assocId;
  uint32_t value;

  typedef SecurityStat Target;
  typedef SecurityStatSpec Spec;
  static bool ReadTarget(openpal::RSlice&, SecurityStat&);
  static bool WriteTarget(const SecurityStat&, openpal::WSlice&);
  static DNP3Serializer<SecurityStat> Inst() { return DNP3Serializer<SecurityStat>(ID(), Size(), &ReadTarget, &WriteTarget); }
};


}

#endif
