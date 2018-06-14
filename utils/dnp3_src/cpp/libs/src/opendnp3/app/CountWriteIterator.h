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
#ifndef OPENDNP3_COUNTWRITEITERATOR_H
#define OPENDNP3_COUNTWRITEITERATOR_H

#include <openpal/serialization/Format.h>

namespace opendnp3
{

// A facade for writing APDUs to an external buffer
template <class CountType, class WriteType>
class CountWriteIterator
{
public:

	static CountWriteIterator Null()
	{
		return CountWriteIterator();
	}

	CountWriteIterator() : count(0), isValid(false), pPosition(nullptr)
	{}

	CountWriteIterator(const openpal::Serializer<WriteType>& serializer_, openpal::WSlice& position) :
		count(0),
		serializer(serializer_),
		isValid(position.Size() >= CountType::SIZE),
		countPosition(position),
		pPosition(&position)
	{
		if(isValid)
		{
			position.Advance(CountType::SIZE);
		}
	}

	~CountWriteIterator()
	{
		if (isValid)
		{
			openpal::Format::Write(countPosition, count);
		}
	}

	bool Write(const WriteType& value)
	{
		if (isValid && (serializer.Size() <= pPosition->Size()) && (count < CountType::Max))
		{
			serializer.Write(value, *this->pPosition);
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

	typename CountType::Type count;
	openpal::Serializer<WriteType> serializer;

	bool isValid;

	openpal::WSlice countPosition;  // make a copy to record where we write the count
	openpal::WSlice* pPosition;
};

}

#endif
