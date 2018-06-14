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

#include "Group50.h"

#include <openpal/serialization/Format.h>
#include <openpal/serialization/Parse.h>
#include "opendnp3/app/MeasurementFactory.h"
#include "opendnp3/app/WriteConversions.h"

using namespace openpal;

namespace opendnp3 {

// ------- Group50Var1 -------

Group50Var1::Group50Var1() : time(0)
{}

bool Group50Var1::Read(RSlice& buffer, Group50Var1& output)
{
  return Parse::Many(buffer, output.time);
}

bool Group50Var1::Write(const Group50Var1& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.time);
}

// ------- Group50Var4 -------

Group50Var4::Group50Var4() : time(0), interval(0), units(0)
{}

bool Group50Var4::Read(RSlice& buffer, Group50Var4& output)
{
  return Parse::Many(buffer, output.time, output.interval, output.units);
}

bool Group50Var4::Write(const Group50Var4& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.time, arg.interval, arg.units);
}

bool Group50Var4::ReadTarget(RSlice& buff, TimeAndInterval& output)
{
  Group50Var4 value;
  if(Read(buff, value))
  {
    output = TimeAndIntervalFactory::From(value.time, value.interval, value.units);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group50Var4::WriteTarget(const TimeAndInterval& value, openpal::WSlice& buff)
{
  return Group50Var4::Write(ConvertGroup50Var4::Apply(value), buff);
}


}
