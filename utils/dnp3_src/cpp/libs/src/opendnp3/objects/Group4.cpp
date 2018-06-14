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

#include "Group4.h"

#include <openpal/serialization/Format.h>
#include <openpal/serialization/Parse.h>
#include "opendnp3/app/MeasurementFactory.h"
#include "opendnp3/app/WriteConversions.h"

using namespace openpal;

namespace opendnp3 {

// ------- Group4Var1 -------

Group4Var1::Group4Var1() : flags(0)
{}

bool Group4Var1::Read(RSlice& buffer, Group4Var1& output)
{
  return Parse::Many(buffer, output.flags);
}

bool Group4Var1::Write(const Group4Var1& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags);
}

bool Group4Var1::ReadTarget(RSlice& buff, DoubleBitBinary& output)
{
  Group4Var1 value;
  if(Read(buff, value))
  {
    output = DoubleBitBinaryFactory::From(value.flags);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group4Var1::WriteTarget(const DoubleBitBinary& value, openpal::WSlice& buff)
{
  return Group4Var1::Write(ConvertGroup4Var1::Apply(value), buff);
}

// ------- Group4Var2 -------

Group4Var2::Group4Var2() : flags(0), time(0)
{}

bool Group4Var2::Read(RSlice& buffer, Group4Var2& output)
{
  return Parse::Many(buffer, output.flags, output.time);
}

bool Group4Var2::Write(const Group4Var2& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.time);
}

bool Group4Var2::ReadTarget(RSlice& buff, DoubleBitBinary& output)
{
  Group4Var2 value;
  if(Read(buff, value))
  {
    output = DoubleBitBinaryFactory::From(value.flags, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group4Var2::WriteTarget(const DoubleBitBinary& value, openpal::WSlice& buff)
{
  return Group4Var2::Write(ConvertGroup4Var2::Apply(value), buff);
}

// ------- Group4Var3 -------

Group4Var3::Group4Var3() : flags(0), time(0)
{}

bool Group4Var3::Read(RSlice& buffer, Group4Var3& output)
{
  return Parse::Many(buffer, output.flags, output.time);
}

bool Group4Var3::Write(const Group4Var3& arg, openpal::WSlice& buffer)
{
  return Format::Many(buffer, arg.flags, arg.time);
}

bool Group4Var3::ReadTarget(RSlice& buff, DoubleBitBinary& output)
{
  Group4Var3 value;
  if(Read(buff, value))
  {
    output = DoubleBitBinaryFactory::From(value.flags, value.time);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group4Var3::WriteTarget(const DoubleBitBinary& value, openpal::WSlice& buff)
{
  return Group4Var3::Write(ConvertGroup4Var3::Apply(value), buff);
}


}
