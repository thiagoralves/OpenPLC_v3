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

#ifndef OPENDNP3_GROUP12_H
#define OPENDNP3_GROUP12_H

#include "opendnp3/app/GroupVariationID.h"
#include <openpal/container/RSlice.h>
#include <openpal/container/WSlice.h>
#include "opendnp3/app/DNPTime.h"
#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/ControlRelayOutputBlock.h"

namespace opendnp3 {

// Binary Command - Any Variation
struct Group12Var0
{
  static GroupVariationID ID() { return GroupVariationID(12,0); }
};

// Binary Command - CROB
struct Group12Var1
{
  static GroupVariationID ID() { return GroupVariationID(12,1); }

  Group12Var1();

  static uint32_t Size() { return 11; }
  static bool Read(openpal::RSlice&, Group12Var1&);
  static bool Write(const Group12Var1&, openpal::WSlice&);

  uint8_t code;
  uint8_t count;
  uint32_t onTime;
  uint32_t offTime;
  uint8_t status;

  typedef ControlRelayOutputBlock Target;
  static bool ReadTarget(openpal::RSlice&, ControlRelayOutputBlock&);
  static bool WriteTarget(const ControlRelayOutputBlock&, openpal::WSlice&);
  static DNP3Serializer<ControlRelayOutputBlock> Inst() { return DNP3Serializer<ControlRelayOutputBlock>(ID(), Size(), &ReadTarget, &WriteTarget); }
};


}

#endif
