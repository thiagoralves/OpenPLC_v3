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

#include "Group121.h"

#include <openpal/serialization/Format.h>
#include <openpal/serialization/Parse.h>
#include "opendnp3/app/MeasurementFactory.h"
#include "opendnp3/app/WriteConversions.h"

using namespace openpal;

namespace opendnp3 {

// ------- Group121Var1 -------

Group121Var1::Group121Var1() : flags(0), assocId(0), value(0)
{}

bool Group121Var1::Read(RSlice& buffer, Group121Var1& output)
{
  return Parse::Many(buffer, output.flags, output.assocId, output.value);
}

bool Group121Var1::Write(const Group121Var1& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.assocId, arg.value);
}

bool Group121Var1::ReadTarget(RSlice& buff, SecurityStat& output)
{
  Group121Var1 value;
  if(Read(buff, value))
  {
    output = SecurityStatFactory::From(value.flags, value.assocId, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group121Var1::WriteTarget(const SecurityStat& value, openpal::WSlice& buff)
{
  return Group121Var1::Write(ConvertGroup121Var1::Apply(value), buff);
}


}
