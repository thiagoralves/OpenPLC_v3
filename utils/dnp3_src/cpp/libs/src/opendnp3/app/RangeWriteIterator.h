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
#ifndef OPENDNP3_RANGEWRITEITERATOR_H
#define OPENDNP3_RANGEWRITEITERATOR_H

#include <openpal/serialization/Format.h>
#include <openpal/serialization/Serializer.h>

namespace opendnp3
{

// A facade for writing APDUs to an external buffer
template <class IndexType, class WriteType>
class RangeWriteIterator
{
public:

	static RangeWriteIterator Null()
	{
		return RangeWriteIterator();
	}

	RangeWriteIterator() : start(0), count(0), isValid(false), pPosition(nullptr)
	{}

	RangeWriteIterator(typename IndexType::Type start_, const openpal::Serializer<WriteType>& serializer_, openpal::WSlice& position) :
		start(start_),
		serializer(serializer_),
		count(0),
		isValid(position.Size() >= 2 * IndexType::SIZE),
		range(position),
		pPosition(&position)
	{
		if (isValid)
		{
			openpal::Format::Write(range, start);
			pPosition->Advance(2 * IndexType::SIZE);
		}
	}

	~RangeWriteIterator()
	{
		if (isValid && count > 0)
		{
			auto stop = start + count - 1;
			IndexType::Write(range, static_cast<typename IndexType::Type>(stop));
		}
	}

	bool Write(const WriteType& value)
	{
		if (isValid && (pPosition->Size() >= serializer.Size()) && (count <= IndexType::Max))
		{
			serializer.Write(value, *pPosition);
			++count;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool IsValid() const
	{
		return isValid;
	}

private:

	typename IndexType::Type start;
	openpal::Serializer<WriteType> serializer;
	uint32_t count;

	bool isValid;

	openpal::WSlice range;  // make a copy to record where we write the range
	openpal::WSlice* pPosition;
};

}

#endif
