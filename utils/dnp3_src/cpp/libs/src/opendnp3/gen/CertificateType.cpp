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

#include "opendnp3/gen/CertificateType.h"

namespace opendnp3 {

uint8_t CertificateTypeToType(CertificateType arg)
{
  return static_cast<uint8_t>(arg);
}
CertificateType CertificateTypeFromType(uint8_t arg)
{
  switch(arg)
  {
    case(0x1):
      return CertificateType::ID_CERTIFICATE;
    case(0x2):
      return CertificateType::ATTRIBUTE_CERTIFICATE;
    default:
      return CertificateType::UNKNOWN;
  }
}
char const* CertificateTypeToString(CertificateType arg)
{
  switch(arg)
  {
    case(CertificateType::ID_CERTIFICATE):
      return "ID_CERTIFICATE";
    case(CertificateType::ATTRIBUTE_CERTIFICATE):
      return "ATTRIBUTE_CERTIFICATE";
    default:
      return "UNKNOWN";
  }
}

}
