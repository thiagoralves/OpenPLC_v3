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

#include "Group32.h"

#include <openpal/serialization/Format.h>
#include <openpal/serialization/Parse.h>
#include "opendnp3/app/MeasurementFactory.h"
#include "opendnp3/app/WriteConversions.h"

using namespace openpal;

namespace opendnp3 {

// ------- Group32Var1 -------

Group32Var1::Group32Var1() : flags(0), value(0)
{}

bool Group32Var1::Read(RSlice& buffer, Group32Var1& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group32Var1::Write(const Group32Var1& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group32Var1::ReadTarget(RSlice& buff, Analog& output)
{
  Group32Var1 value;
  if(Read(buff, value))
  {
    output = AnalogFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group32Var1::WriteTarget(const Analog& value, openpal::WSlice& buff)
{
  return Group32Var1::Write(ConvertGroup32Var1::Apply(value), buff);
}

// ------- Group32Var2 -------

Group32Var2::Group32Var2() : flags(0), value(0)
{}

bool Group32Var2::Read(RSlice& buffer, Group32Var2& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group32Var2::Write(const Group32Var2& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group32Var2::ReadTarget(RSlice& buff, Analog& output)
{
  Group32Var2 value;
  if(Read(buff, value))
  {
    output = AnalogFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group32Var2::WriteTarget(const Analog& value, openpal::WSlice& buff)
{
  return Group32Var2::Write(ConvertGroup32Var2::Apply(value), buff);
}

// ------- Group32Var3 -------

Group32Var3::Group32Var3() : flags(0), value(0), time(0)
{}

bool Group32Var3::Read(RSlice& buffer, Group32Var3& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group32Var3::Write(const Group32Var3& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group32Var3::ReadTarget(RSlice& buff, Analog& output)
{
  Group32Var3 value;
  if(Read(buff, value))
  {
    output = AnalogFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group32Var3::WriteTarget(const Analog& value, openpal::WSlice& buff)
{
  return Group32Var3::Write(ConvertGroup32Var3::Apply(value), buff);
}

// ------- Group32Var4 -------

Group32Var4::Group32Var4() : flags(0), value(0), time(0)
{}

bool Group32Var4::Read(RSlice& buffer, Group32Var4& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group32Var4::Write(const Group32Var4& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group32Var4::ReadTarget(RSlice& buff, Analog& output)
{
  Group32Var4 value;
  if(Read(buff, value))
  {
    output = AnalogFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group32Var4::WriteTarget(const Analog& value, openpal::WSlice& buff)
{
  return Group32Var4::Write(ConvertGroup32Var4::Apply(value), buff);
}

// ------- Group32Var5 -------

Group32Var5::Group32Var5() : flags(0), value(0.0)
{}

bool Group32Var5::Read(RSlice& buffer, Group32Var5& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group32Var5::Write(const Group32Var5& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group32Var5::ReadTarget(RSlice& buff, Analog& output)
{
  Group32Var5 value;
  if(Read(buff, value))
  {
    output = AnalogFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group32Var5::WriteTarget(const Analog& value, openpal::WSlice& buff)
{
  return Group32Var5::Write(ConvertGroup32Var5::Apply(value), buff);
}

// ------- Group32Var6 -------

Group32Var6::Group32Var6() : flags(0), value(0.0)
{}

bool Group32Var6::Read(RSlice& buffer, Group32Var6& output)
{
  return Parse::Many(buffer, output.flags, output.value);
}

bool Group32Var6::Write(const Group32Var6& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value);
}

bool Group32Var6::ReadTarget(RSlice& buff, Analog& output)
{
  Group32Var6 value;
  if(Read(buff, value))
  {
    output = AnalogFactory::From(value.flags, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group32Var6::WriteTarget(const Analog& value, openpal::WSlice& buff)
{
  return Group32Var6::Write(ConvertGroup32Var6::Apply(value), buff);
}

// ------- Group32Var7 -------

Group32Var7::Group32Var7() : flags(0), value(0.0), time(0)
{}

bool Group32Var7::Read(RSlice& buffer, Group32Var7& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group32Var7::Write(const Group32Var7& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group32Var7::ReadTarget(RSlice& buff, Analog& output)
{
  Group32Var7 value;
  if(Read(buff, value))
  {
    output = AnalogFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group32Var7::WriteTarget(const Analog& value, openpal::WSlice& buff)
{
  return Group32Var7::Write(ConvertGroup32Var7::Apply(value), buff);
}

// ------- Group32Var8 -------

Group32Var8::Group32Var8() : flags(0), value(0.0), time(0)
{}

bool Group32Var8::Read(RSlice& buffer, Group32Var8& output)
{
  return Parse::Many(buffer, output.flags, output.value, output.time);
}

bool Group32Var8::Write(const Group32Var8& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.value, arg.time);
}

bool Group32Var8::ReadTarget(RSlice& buff, Analog& output)
{
  Group32Var8 value;
  if(Read(buff, value))
  {
    output = AnalogFactory::From(value.flags, value.value, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group32Var8::WriteTarget(const Analog& value, openpal::WSlice& buff)
{
  return Group32Var8::Write(ConvertGroup32Var8::Apply(value), buff);
}


}
