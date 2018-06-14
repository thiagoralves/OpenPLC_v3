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

#include "Group22.h"

#include <openpal/serialization/Format.h>
#include <openpal/serialization/Parse.h>
#include "opendnp3/app/MeasurementFactory.h"
#include "opendnp3/app/WriteConversions.h"

using namespace openpal;

namespace opendnp3 {

// ------- Group22Var1 -------

Group22Var1::Group22Var1() : flags(0), value(0)
{}

bool Group22Var1::Read(RSlice& buffer, Group22Var1& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group22Var1::Write(const Group22Var1& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group22Var1::ReadTarget(RSlice& buff, Counter& output)
{
  Group22Var1 value;
  if(Read(buff, value))
  {
    output = CounterFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group22Var1::WriteTarget(const Counter& value, openpal::WSlice& buff)
{
  return Group22Var1::Write(ConvertGroup22Var1::Apply(value), buff);
}

// ------- Group22Var2 -------

Group22Var2::Group22Var2() : flags(0), value(0)
{}

bool Group22Var2::Read(RSlice& buffer, Group22Var2& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group22Var2::Write(const Group22Var2& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group22Var2::ReadTarget(RSlice& buff, Counter& output)
{
  Group22Var2 value;
  if(Read(buff, value))
  {
    output = CounterFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group22Var2::WriteTarget(const Counter& value, openpal::WSlice& buff)
{
  return Group22Var2::Write(ConvertGroup22Var2::Apply(value), buff);
}

// ------- Group22Var5 -------

Group22Var5::Group22Var5() : flags(0), value(0), time(0)
{}

bool Group22Var5::Read(RSlice& buffer, Group22Var5& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group22Var5::Write(const Group22Var5& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group22Var5::ReadTarget(RSlice& buff, Counter& output)
{
  Group22Var5 value;
  if(Read(buff, value))
  {
    output = CounterFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group22Var5::WriteTarget(const Counter& value, openpal::WSlice& buff)
{
  return Group22Var5::Write(ConvertGroup22Var5::Apply(value), buff);
}

// ------- Group22Var6 -------

Group22Var6::Group22Var6() : flags(0), value(0), time(0)
{}

bool Group22Var6::Read(RSlice& buffer, Group22Var6& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group22Var6::Write(const Group22Var6& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group22Var6::ReadTarget(RSlice& buff, Counter& output)
{
  Group22Var6 value;
  if(Read(buff, value))
  {
    output = CounterFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group22Var6::WriteTarget(const Counter& value, openpal::WSlice& buff)
{
  return Group22Var6::Write(ConvertGroup22Var6::Apply(value), buff);
}


}
