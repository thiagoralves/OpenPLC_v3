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

#ifndef OPENDNP3_INDEXMODE_H
#define OPENDNP3_INDEXMODE_H

#include <cstdint>

namespace opendnp3 {

/**
  Select contiguous or dis-contiguous index mode
*/
enum class IndexMode : uint8_t
{
  /// Indices are contiguous. Most efficient as direct indexing is used.
  Contiguous = 0x0,
  /// Indices are dis-contiguous. Resorts to binary search to find raw index.
  Discontiguous = 0x1
};


}

#endif
