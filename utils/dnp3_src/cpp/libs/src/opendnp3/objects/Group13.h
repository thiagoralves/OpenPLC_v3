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

#ifndef OPENDNP3_GROUP13_H
#define OPENDNP3_GROUP13_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/BinaryCommandEvent.h"

namespace opendnp3 {

// Binary Command Event - Without Time
struct Group13Var1
{
  static GroupVariationID ID() { return GroupVariationID(13,1); }

  Group13Var1();

  static uint32_t Size() { return 1; }
  static bool Read(openpal::RSlice&, Group13Var1&);
  static bool Write(const Group13Var1&, openpal::WSlice&);

  uint8_t flags;

  typedef BinaryCommandEvent Target;
  static bool ReadTarget(openpal::RSlice&, BinaryCommandEvent&);
  static bool WriteTarget(const BinaryCommandEvent&, openpal::WSlice&);
  static DNP3Serializer<BinaryCommandEvent> Inst() { return DNP3Serializer<BinaryCommandEvent>(ID(), Size(), &ReadTarget, &WriteTarget); }
};

// Binary Command Event - With Time
struct Group13Var2
{
  static GroupVariationID ID() { return GroupVariationID(13,2); }

  Group13Var2();

  static uint32_t Size() { return 7; }
  static bool Read(openpal::RSlice&, Group13Var2&);
  static bool Write(const Group13Var2&, openpal::WSlice&);

  uint8_t flags;
  DNPTime time;

  typedef BinaryCommandEvent Target;
  static bool ReadTarget(openpal::RSlice&, BinaryCommandEvent&);
  static bool WriteTarget(const BinaryCommandEvent&, openpal::WSlice&);
  static DNP3Serializer<BinaryCommandEvent> Inst() { return DNP3Serializer<BinaryCommandEvent>(ID(), Size(), &ReadTarget, &WriteTarget); }
};


}

#endif
