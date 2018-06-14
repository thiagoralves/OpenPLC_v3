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

#include "Group12.h"

#include <openpal/serialization/Format.h>
#include <openpal/serialization/Parse.h>
#include "opendnp3/app/MeasurementFactory.h"
#include "opendnp3/app/WriteConversions.h"

using namespace openpal;

namespace opendnp3 {

// ------- Group12Var1 -------

Group12Var1::Group12Var1() : code(0), count(0), onTime(0), offTime(0), status(0)
{}

bool Group12Var1::Read(RSlice& buffer, Group12Var1& output)
{
  return Parse::Many(buffer, output.code, output.count, output.onTime, output.offTime, output.status);
}

bool Group12Var1::Write(const Group12Var1& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.code, arg.count, arg.onTime, arg.offTime, arg.status);
}

bool Group12Var1::ReadTarget(RSlice& buff, ControlRelayOutputBlock& output)
{
  Group12Var1 value;
  if(Read(buff, value))
  {
    output = ControlRelayOutputBlockFactory::From(value.code, value.count, value.onTime, value.offTime, value.status);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group12Var1::WriteTarget(const ControlRelayOutputBlock& value, openpal::WSlice& buff)
{
  return Group12Var1::Write(ConvertGroup12Var1::Apply(value), buff);
}


}
