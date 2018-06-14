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

#ifndef OPENDNP3_CERTIFICATETYPE_H
#define OPENDNP3_CERTIFICATETYPE_H

#include <cstdint>

namespace opendnp3 {

/**
  Specifies the reason that an auth error message was transmitted
*/
enum class CertificateType : uint8_t
{
  /// ID certificate
  ID_CERTIFICATE = 0x1,
  /// Attribute certificate
  ATTRIBUTE_CERTIFICATE = 0x2,
  /// Unknown certificate type
  UNKNOWN = 0x0
};

uint8_t CertificateTypeToType(CertificateType arg);
CertificateType CertificateTypeFromType(uint8_t arg);
char const* CertificateTypeToString(CertificateType arg);

}

#endif
