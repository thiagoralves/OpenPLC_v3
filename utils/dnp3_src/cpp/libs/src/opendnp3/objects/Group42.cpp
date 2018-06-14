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

#include "Group42.h"

#include <openpal/serialization/Format.h>
#include <openpal/serialization/Parse.h>
#include "opendnp3/app/MeasurementFactory.h"
#include "opendnp3/app/WriteConversions.h"

using namespace openpal;

namespace opendnp3 {

// ------- Group42Var1 -------

Group42Var1::Group42Var1() : flags(0), value(0)
{}

bool Group42Var1::Read(RSlice& buffer, Group42Var1& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group42Var1::Write(const Group42Var1& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group42Var1::ReadTarget(RSlice& buff, AnalogOutputStatus& output)
{
  Group42Var1 value;
  if(Read(buff, value))
  {
    output = AnalogOutputStatusFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group42Var1::WriteTarget(const AnalogOutputStatus& value, openpal::WSlice& buff)
{
  return Group42Var1::Write(ConvertGroup42Var1::Apply(value), buff);
}

// ------- Group42Var2 -------

Group42Var2::Group42Var2() : flags(0), value(0)
{}

bool Group42Var2::Read(RSlice& buffer, Group42Var2& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group42Var2::Write(const Group42Var2& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group42Var2::ReadTarget(RSlice& buff, AnalogOutputStatus& output)
{
  Group42Var2 value;
  if(Read(buff, value))
  {
    output = AnalogOutputStatusFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group42Var2::WriteTarget(const AnalogOutputStatus& value, openpal::WSlice& buff)
{
  return Group42Var2::Write(ConvertGroup42Var2::Apply(value), buff);
}

// ------- Group42Var3 -------

Group42Var3::Group42Var3() : flags(0), value(0), time(0)
{}

bool Group42Var3::Read(RSlice& buffer, Group42Var3& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group42Var3::Write(const Group42Var3& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group42Var3::ReadTarget(RSlice& buff, AnalogOutputStatus& output)
{
  Group42Var3 value;
  if(Read(buff, value))
  {
    output = AnalogOutputStatusFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group42Var3::WriteTarget(const AnalogOutputStatus& value, openpal::WSlice& buff)
{
  return Group42Var3::Write(ConvertGroup42Var3::Apply(value), buff);
}

// ------- Group42Var4 -------

Group42Var4::Group42Var4() : flags(0), value(0), time(0)
{}

bool Group42Var4::Read(RSlice& buffer, Group42Var4& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group42Var4::Write(const Group42Var4& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group42Var4::ReadTarget(RSlice& buff, AnalogOutputStatus& output)
{
  Group42Var4 value;
  if(Read(buff, value))
  {
    output = AnalogOutputStatusFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group42Var4::WriteTarget(const AnalogOutputStatus& value, openpal::WSlice& buff)
{
  return Group42Var4::Write(ConvertGroup42Var4::Apply(value), buff);
}

// ------- Group42Var5 -------

Group42Var5::Group42Var5() : flags(0), value(0.0)
{}

bool Group42Var5::Read(RSlice& buffer, Group42Var5& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group42Var5::Write(const Group42Var5& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group42Var5::ReadTarget(RSlice& buff, AnalogOutputStatus& output)
{
  Group42Var5 value;
  if(Read(buff, value))
  {
    output = AnalogOutputStatusFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group42Var5::WriteTarget(const AnalogOutputStatus& value, openpal::WSlice& buff)
{
  return Group42Var5::Write(ConvertGroup42Var5::Apply(value), buff);
}

// ------- Group42Var6 -------

Group42Var6::Group42Var6() : flags(0), value(0.0)
{}

bool Group42Var6::Read(RSlice& buffer, Group42Var6& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group42Var6::Write(const Group42Var6& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group42Var6::ReadTarget(RSlice& buff, AnalogOutputStatus& output)
{
  Group42Var6 value;
  if(Read(buff, value))
  {
    output = AnalogOutputStatusFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group42Var6::WriteTarget(const AnalogOutputStatus& value, openpal::WSlice& buff)
{
  return Group42Var6::Write(ConvertGroup42Var6::Apply(value), buff);
}

// ------- Group42Var7 -------

Group42Var7::Group42Var7() : flags(0), value(0.0), time(0)
{}

bool Group42Var7::Read(RSlice& buffer, Group42Var7& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group42Var7::Write(const Group42Var7& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group42Var7::ReadTarget(RSlice& buff, AnalogOutputStatus& output)
{
  Group42Var7 value;
  if(Read(buff, value))
  {
    output = AnalogOutputStatusFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group42Var7::WriteTarget(const AnalogOutputStatus& value, openpal::WSlice& buff)
{
  return Group42Var7::Write(ConvertGroup42Var7::Apply(value), buff);
}

// ------- Group42Var8 -------

Group42Var8::Group42Var8() : flags(0), value(0.0), time(0)
{}

bool Group42Var8::Read(RSlice& buffer, Group42Var8& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group42Var8::Write(const Group42Var8& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group42Var8::ReadTarget(RSlice& buff, AnalogOutputStatus& output)
{
  Group42Var8 value;
  if(Read(buff, value))
  {
    output = AnalogOutputStatusFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group42Var8::WriteTarget(const AnalogOutputStatus& value, openpal::WSlice& buff)
{
  return Group42Var8::Write(ConvertGroup42Var8::Apply(value), buff);
}


}
