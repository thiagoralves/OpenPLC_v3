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
#ifndef OPENPAL_QUEUE_H
#define OPENPAL_QUEUE_H

#include <assert.h>

#include "openpal/container/Array.h"

namespace openpal
{

template <class ValueType, class IndexType>
class Queue
{

public:

	Queue(IndexType size) : count(0), first(0), nextInsert(0), buffer(size)
	{}

	IndexType Size() const
	{
		return count;
	}

	IndexType Capacity() const
	{
		return buffer.Size();
	}

	bool IsEmpty() const
	{
		return count == 0;
	}

	bool IsNotEmpty() const
	{
		return count > 0;
	}

	bool IsFull() const
	{
		return count == buffer.Size();
	}

	void Clear()
	{
		count = first = nextInsert = 0;
	}

	ValueType* Peek()
	{
		if (IsEmpty())
		{
			return nullptr;
		}
		else
		{
			return &buffer[first];
		}
	}

	ValueType* Pop()
	{
		if (IsEmpty())
		{
			return nullptr;
		}
		else
		{
			IndexType ret = first;
			first = (first + 1) % buffer.Size();
			--count;
			return &buffer[ret];
		}
	}

	bool Enqueue(const ValueType& value)
	{
		if (IsFull())
		{
			return false;
		}
		else
		{
			buffer[nextInsert] = value;
			nextInsert = (nextInsert + 1) % buffer.Size();
			++count;
			return true;
		}
	}

private:

	IndexType count;
	IndexType first;
	IndexType nextInsert;

	openpal::Array<ValueType, IndexType> buffer;
};

}

#endif
