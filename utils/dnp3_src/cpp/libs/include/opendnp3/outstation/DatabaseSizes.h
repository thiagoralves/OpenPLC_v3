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
#ifndef OPENDNP3_DATABASESIZES_H
#define OPENDNP3_DATABASESIZES_H

#include <cstdint>

namespace opendnp3
{

/**
* Specifies the number and type of measurements in an outstation database.
*/
struct DatabaseSizes
{
	static DatabaseSizes BinaryOnly(uint16_t count)
	{
		return DatabaseSizes(count, 0, 0, 0, 0, 0, 0, 0);
	}

	static DatabaseSizes DoubleBinaryOnly(uint16_t count)
	{
		return DatabaseSizes(0, count, 0, 0, 0, 0, 0, 0);
	}

	static DatabaseSizes AnalogOnly(uint16_t count)
	{
		return DatabaseSizes(0, 0, count, 0, 0, 0, 0, 0);
	}

	static DatabaseSizes CounterOnly(uint16_t count)
	{
		return DatabaseSizes(0, 0, 0, count, 0, 0, 0, 0);
	}

	static DatabaseSizes FrozenCounterOnly(uint16_t count)
	{
		return DatabaseSizes(0, 0, 0, 0, count, 0, 0, 0);
	}

	static DatabaseSizes BinaryOutputStatusOnly(uint16_t count)
	{
		return DatabaseSizes(0, 0, 0, 0, 0, count, 0, 0);
	}

	static DatabaseSizes AnalogOutputStatusOnly(uint16_t count)
	{
		return DatabaseSizes(0, 0, 0, 0, 0, 0, count, 0);
	}

	static DatabaseSizes TimeAndIntervalOnly(uint16_t count)
	{
		return DatabaseSizes(0, 0, 0, 0, 0, 0, 0, count);
	}

	static DatabaseSizes AllTypes(uint16_t count)
	{
		return DatabaseSizes(count, count, count, count, count, count, count, count);
	}

	static DatabaseSizes Empty()
	{
		return DatabaseSizes(0, 0, 0, 0, 0, 0, 0, 0);
	}

	DatabaseSizes(uint16_t numBinary,
	              uint16_t numDoubleBinary,
	              uint16_t numAnalog,
	              uint16_t numCounter,
	              uint16_t numFrozenCounter,
	              uint16_t numBinaryOutputStatus,
	              uint16_t numAnalogOutputStatus,
	              uint16_t numTimeAndInterval) :

		numBinary(numBinary),
		numDoubleBinary(numDoubleBinary),
		numAnalog(numAnalog),
		numCounter(numCounter),
		numFrozenCounter(numFrozenCounter),
		numBinaryOutputStatus(numBinaryOutputStatus),
		numAnalogOutputStatus(numAnalogOutputStatus),
		numTimeAndInterval(numTimeAndInterval)
	{}

	uint16_t numBinary;
	uint16_t numDoubleBinary;
	uint16_t numAnalog;
	uint16_t numCounter;
	uint16_t numFrozenCounter;
	uint16_t numBinaryOutputStatus;
	uint16_t numAnalogOutputStatus;
	uint16_t numTimeAndInterval;

};

}

#endif
