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

#ifndef OPENDNP3_KEYWRAPALGORITHM_H
#define OPENDNP3_KEYWRAPALGORITHM_H

#include <cstdint>

namespace opendnp3 {

/**
  Enumerates possible key-wrap algorithms
*/
enum class KeyWrapAlgorithm : uint8_t
{
  /// AES 128 Key Wrap Algorithm
  AES_128 = 0x1,
  /// AES 256 Key Wrap Algorithm
  AES_256 = 0x2,
  UNDEFINED = 0x0
};

uint8_t KeyWrapAlgorithmToType(KeyWrapAlgorithm arg);
KeyWrapAlgorithm KeyWrapAlgorithmFromType(uint8_t arg);
char const* KeyWrapAlgorithmToString(KeyWrapAlgorithm arg);

}

#endif
