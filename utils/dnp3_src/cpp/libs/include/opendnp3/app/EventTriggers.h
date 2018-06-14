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
#ifndef OPENDNP3_EVENTTRIGGERS_H
#define OPENDNP3_EVENTTRIGGERS_H

#include "opendnp3/app/BaseMeasurementTypes.h"

namespace opendnp3
{
namespace measurements
{
template <class T, class U>
bool IsEvent(const T& val1, const T& val2, T deadband)
{
	// T can be unsigned data type so std::abs won't work since it only directly supports signed data types
	// If one uses std::abs and T is unsigned one will get an ambiguous override error.

	U diff = (val2 > val1) ? (static_cast<U>(val2) - static_cast<U>(val1)) : (static_cast<U>(val1) - static_cast<U>(val2));

	return diff > deadband;
}


bool IsEvent(const TypedMeasurement<double>& newMeas, const TypedMeasurement<double>& oldMeas, double deadband);

}
}

#endif
