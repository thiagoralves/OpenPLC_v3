/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */
#ifndef OPENPAL_CONFIGURE_H
#define OPENPAL_CONFIGURE_H

#include <cstdint>

// Default configurations for the static erasure size.
// They are liberally set by default for x64
// but can be reduced for embedded systems.

#ifndef OPENPAL_ERASURE_MULTIPLE
#define OPENPAL_ERASURE_MULTIPLE 12
#endif

namespace openpal
{
namespace sizes
{

static const uint16_t MAX_ERASURE_SIZE = OPENPAL_ERASURE_MULTIPLE * sizeof(void*);

}
}

#ifdef AVR

#include <stddef.h>
#include <stdlib.h>

inline void* operator new(size_t, void* p)
{
	return p;
}

inline void* operator new(size_t size)
{
	return malloc(size);
}

inline void operator delete(void* ptr)
{
	free(ptr);
}

inline void* operator new[](size_t size)
{
	return malloc(size);
}

inline void operator delete[](void* ptr)
{
	free(ptr);
}

extern "C" void abort();

#else

#include <new>

#endif

#endif

