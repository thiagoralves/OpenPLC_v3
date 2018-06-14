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
#ifndef OPENDNP3_ITRANSPORTSEGMENT_H
#define OPENDNP3_ITRANSPORTSEGMENT_H

#include <openpal/container/RSlice.h>

namespace opendnp3
{

class ITransportSegment
{

public:

	virtual ~ITransportSegment() {}

	virtual bool HasValue() const = 0;

	// Read the current segment with a specified max size
	virtual openpal::RSlice GetSegment() = 0;

	// move to the next segment, true if more segments available
	virtual bool Advance() = 0;
};

}

#endif
