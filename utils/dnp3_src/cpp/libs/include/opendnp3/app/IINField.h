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
#ifndef OPENDNP3_IINFIELD_H
#define OPENDNP3_IINFIELD_H

#include <cstdint>

namespace opendnp3
{

enum class IINBit
{
	ALL_STATIONS = 0,
	CLASS1_EVENTS,
	CLASS2_EVENTS,
	CLASS3_EVENTS,
	NEED_TIME,
	LOCAL_CONTROL,
	DEVICE_TROUBLE,
	DEVICE_RESTART,
	FUNC_NOT_SUPPORTED,
	OBJECT_UNKNOWN,
	PARAM_ERROR,
	EVENT_BUFFER_OVERFLOW,
	ALREADY_EXECUTING,
	CONFIG_CORRUPT,
	RESERVED1,
	RESERVED2 = 15
};

/** DNP3 two-byte IIN field.
*/
class IINField
{

private:

	enum class LSBMask : uint8_t
	{
		ALL_STATIONS = 0x01,
		CLASS1_EVENTS = 0x02,
		CLASS2_EVENTS = 0x04,
		CLASS3_EVENTS = 0x08,
		NEED_TIME = 0x10,
		LOCAL_CONTROL = 0x20,
		DEVICE_TROUBLE = 0x40,
		DEVICE_RESTART = 0x80,
	};

	enum class MSBMask : uint8_t
	{
		FUNC_NOT_SUPPORTED = 0x01,
		OBJECT_UNKNOWN = 0x02,
		PARAM_ERROR = 0x04,
		EVENT_BUFFER_OVERFLOW = 0x08,
		ALREADY_EXECUTING = 0x10,
		CONFIG_CORRUPT = 0x20,
		RESERVED1 = 0x40,
		RESERVED2 = 0x80,

		//special mask that indicates if there was any error processing a request
		REQUEST_ERROR_MASK = FUNC_NOT_SUPPORTED | OBJECT_UNKNOWN | PARAM_ERROR
	};


public:

	static IINField Empty()
	{
		return IINField(0, 0);
	}

	IINField(IINBit bit) : LSB(0), MSB(0)
	{
		this->SetBit(bit);
	}

	IINField(uint8_t aLSB, uint8_t aMSB) : 	LSB(aLSB), MSB(aMSB)
	{}

	IINField() : LSB(0), MSB(0)
	{}

	bool IsSet(IINBit bit) const;

	bool IsClear(IINBit bit) const
	{
		return !IsSet(bit);
	}

	void SetBit(IINBit bit);
	void ClearBit(IINBit bit);

	void SetBitToValue(IINBit bit, bool value);

	bool operator==(const IINField& arRHS) const;

	bool Any() const
	{
		return (LSB | MSB) != 0;
	}

	void Clear()
	{
		LSB = MSB = 0;
	}

	bool HasRequestError() const
	{
		return Get(MSBMask::REQUEST_ERROR_MASK);
	}

	IINField operator|(const IINField& aIIN) const
	{
		return IINField(LSB | aIIN.LSB, MSB | aIIN.MSB);
	}

	IINField& operator|=(const IINField& aIIN)
	{
		MSB |= aIIN.MSB;
		LSB |= aIIN.LSB;
		return *this;
	}

	IINField operator&(const IINField& aIIN) const
	{
		return IINField(LSB & aIIN.LSB, MSB & aIIN.MSB);
	}

	IINField& operator&=(const IINField& aIIN)
	{
		MSB &= aIIN.MSB;
		LSB &= aIIN.LSB;
		return *this;
	}

	IINField operator~() const
	{
		return IINField(~LSB, ~MSB);
	}

	uint8_t LSB;
	uint8_t MSB;

private:

	static const uint8_t REQUEST_ERROR_MASK;

	inline bool Get(LSBMask bit) const
	{
		return (LSB & static_cast<uint8_t>(bit)) != 0;
	}

	inline bool Get(MSBMask bit) const
	{
		return (MSB & static_cast<uint8_t>(bit)) != 0;
	}

	inline void Set(LSBMask bit)
	{
		LSB |= static_cast<uint8_t>(bit);
	}
	inline void Set(MSBMask bit)
	{
		MSB |= static_cast<uint8_t>(bit);
	}

	inline void Clear(LSBMask bit)
	{
		LSB &= ~static_cast<uint8_t>(bit);
	}
	inline void Clear(MSBMask bit)
	{
		MSB &= ~static_cast<uint8_t>(bit);
	}
};

}

#endif
