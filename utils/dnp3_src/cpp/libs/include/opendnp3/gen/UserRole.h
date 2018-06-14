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

#ifndef OPENDNP3_USERROLE_H
#define OPENDNP3_USERROLE_H

#include <cstdint>

namespace opendnp3 {

/**
  Enumerates pre-defined rules in secure authentication
*/
enum class UserRole : uint16_t
{
  VIEWER = 0,
  OPERATOR = 1,
  ENGINEER = 2,
  INSTALLER = 3,
  SECADM = 4,
  SECAUD = 5,
  RBACMNT = 6,
  SINGLE_USER = 32768,
  UNDEFINED = 32767
};

uint16_t UserRoleToType(UserRole arg);
UserRole UserRoleFromType(uint16_t arg);
char const* UserRoleToString(UserRole arg);

}

#endif
