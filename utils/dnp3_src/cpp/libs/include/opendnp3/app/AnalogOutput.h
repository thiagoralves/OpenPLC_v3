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
#ifndef OPENDNP3_ANALOGOUTPUT_H
#define OPENDNP3_ANALOGOUTPUT_H

#include "opendnp3/gen/CommandStatus.h"

namespace opendnp3
{

/**
 * The object to represent a setpoint request from the master. Think of
 * this like turning a dial on the front of a machine to desired setting.
 */
template <class T>
class AnalogOutput
{
public:

	AnalogOutput() :
		value(0),
		status(CommandStatus::SUCCESS)
	{}

	AnalogOutput(T value_) :
		value(value_),
		status(CommandStatus::SUCCESS)
	{}

	AnalogOutput(T value_, CommandStatus status_) :
		value(value_),
		status(status_)
	{}

	bool ValuesEqual(const AnalogOutput<T>& lhs) const
	{
		return value == lhs.value;
	}

	T value;

	/**
	* The status value defaults to CS_SUCCESS for requests
	*/
	CommandStatus status;
};

/**
*	16-bit signed integer analog output. The underlying serialization is Group41, Variation 2
*/
class AnalogOutputInt16 : public AnalogOutput<int16_t>
{
public:

	AnalogOutputInt16();
	AnalogOutputInt16(int16_t);
	AnalogOutputInt16(int16_t, CommandStatus);

	bool operator==(const AnalogOutputInt16& arRHS) const;
};

/**
*	32-bit signed integer analog output. The underlying serialization is Group41, Variation 1
*/
class AnalogOutputInt32 : public AnalogOutput<int32_t>
{
public:

	AnalogOutputInt32();
	AnalogOutputInt32(int32_t);
	AnalogOutputInt32(int32_t, CommandStatus);

	bool operator==(const AnalogOutputInt32& arRHS) const;
};

/**
*	Single precision analog output. The underlying serialization is Group41, Variation 3
*/
class AnalogOutputFloat32 : public AnalogOutput<float>
{
public:

	AnalogOutputFloat32();
	AnalogOutputFloat32(float);
	AnalogOutputFloat32(float, CommandStatus);

	bool operator==(const AnalogOutputFloat32& arRHS) const;
};

/**
*	Double precision analog output. The underlying serialization is Group41, Variation 3
*/
class AnalogOutputDouble64 : public AnalogOutput<double>
{
public:

	AnalogOutputDouble64();
	AnalogOutputDouble64(double);
	AnalogOutputDouble64(double, CommandStatus);

	bool operator==(const AnalogOutputDouble64& arRHS) const;
};


}



#endif
