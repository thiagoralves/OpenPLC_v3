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
#ifndef OPENDNP3_OCTETDATA_H
#define OPENDNP3_OCTETDATA_H

#include <cstdint>

#include <openpal/container/RSlice.h>
#include <openpal/container/StaticBuffer.h>

namespace opendnp3
{

/**
* A base-class for bitstrings containing up to 255 bytes
*/
class OctetData
{
public:

	const static uint8_t MAX_SIZE = 255;

	OctetData();
	OctetData(const openpal::RSlice& input);

	openpal::RSlice ToRSlice() const;

private:

	void Initialize(const openpal::RSlice& buffer);

	openpal::StaticBuffer<MAX_SIZE> buffer;
	uint8_t size;
};

}



#endif

