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

#include "Group1.h"

#include <openpal/serialization/Format.h>
#include <openpal/serialization/Parse.h>
#include "opendnp3/app/MeasurementFactory.h"
#include "opendnp3/app/WriteConversions.h"

using namespace openpal;

namespace opendnp3 {

// ------- Group1Var2 -------

Group1Var2::Group1Var2() : flags(0)
{}

bool Group1Var2::Read(RSlice& buffer, Group1Var2& output)
{
  return Parse::Many(buffer, output.flags);
}

bool Group1Var2::Write(const Group1Var2& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags);
}

bool Group1Var2::ReadTarget(RSlice& buff, Binary& output)
{
  Group1Var2 value;
  if(Read(buff, value))
  {
    output = BinaryFactory::From(value.flags);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group1Var2::WriteTarget(const Binary& value, openpal::WSlice& buff)
{
  return Group1Var2::Write(ConvertGroup1Var2::Apply(value), buff);
}


}
