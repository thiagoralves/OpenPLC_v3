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

#include "Group23.h"

#include <openpal/serialization/Format.h>
#include <openpal/serialization/Parse.h>
#include "opendnp3/app/MeasurementFactory.h"
#include "opendnp3/app/WriteConversions.h"

using namespace openpal;

namespace opendnp3 {

// ------- Group23Var1 -------

Group23Var1::Group23Var1() : flags(0), value(0)
{}

bool Group23Var1::Read(RSlice& buffer, Group23Var1& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group23Var1::Write(const Group23Var1& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group23Var1::ReadTarget(RSlice& buff, FrozenCounter& output)
{
  Group23Var1 value;
  if(Read(buff, value))
  {
    output = FrozenCounterFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group23Var1::WriteTarget(const FrozenCounter& value, openpal::WSlice& buff)
{
  return Group23Var1::Write(ConvertGroup23Var1::Apply(value), buff);
}

// ------- Group23Var2 -------

Group23Var2::Group23Var2() : flags(0), value(0)
{}

bool Group23Var2::Read(RSlice& buffer, Group23Var2& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group23Var2::Write(const Group23Var2& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group23Var2::ReadTarget(RSlice& buff, FrozenCounter& output)
{
  Group23Var2 value;
  if(Read(buff, value))
  {
    output = FrozenCounterFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group23Var2::WriteTarget(const FrozenCounter& value, openpal::WSlice& buff)
{
  return Group23Var2::Write(ConvertGroup23Var2::Apply(value), buff);
}

// ------- Group23Var5 -------

Group23Var5::Group23Var5() : flags(0), value(0), time(0)
{}

bool Group23Var5::Read(RSlice& buffer, Group23Var5& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group23Var5::Write(const Group23Var5& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group23Var5::ReadTarget(RSlice& buff, FrozenCounter& output)
{
  Group23Var5 value;
  if(Read(buff, value))
  {
    output = FrozenCounterFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group23Var5::WriteTarget(const FrozenCounter& value, openpal::WSlice& buff)
{
  return Group23Var5::Write(ConvertGroup23Var5::Apply(value), buff);
}

// ------- Group23Var6 -------

Group23Var6::Group23Var6() : flags(0), value(0), time(0)
{}

bool Group23Var6::Read(RSlice& buffer, Group23Var6& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group23Var6::Write(const Group23Var6& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group23Var6::ReadTarget(RSlice& buff, FrozenCounter& output)
{
  Group23Var6 value;
  if(Read(buff, value))
  {
    output = FrozenCounterFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group23Var6::WriteTarget(const FrozenCounter& value, openpal::WSlice& buff)
{
  return Group23Var6::Write(ConvertGroup23Var6::Apply(value), buff);
}


}
