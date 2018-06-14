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
#ifndef OPENPAL_WSLICE_H
#define OPENPAL_WSLICE_H

#include "HasSize.h"

#include <cstdint>

namespace openpal
{

class RSlice;

/**
*	Represents a write-able slice of a buffer located elsewhere. Mediates writing to the buffer
*	to prevent overruns and other errors.
*/
class WSlice : public HasSize<uint32_t>
{
public:

	static WSlice Empty();

	void SetAllTo(uint8_t value);

	WSlice();
	WSlice(uint8_t* pBuffer, uint32_t size);

	void Clear();

	uint32_t Advance(uint32_t count);

	WSlice Skip(uint32_t count) const;

	RSlice ToRSlice() const;

	operator uint8_t* ()
	{
		return pBuffer;
	};

	operator uint8_t const* () const
	{
		return pBuffer;
	};

private:

	uint8_t* pBuffer;
};


}

#endif
