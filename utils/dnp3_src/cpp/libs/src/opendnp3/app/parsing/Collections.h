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
#ifndef OPENDNP3_COLLECTIONS_H
#define OPENDNP3_COLLECTIONS_H

#include "opendnp3/app/parsing/ICollection.h"

namespace opendnp3
{

/**
* A simple collection derived from an underlying array
*/
template <class T>
class ArrayCollection : public ICollection<T>
{
public:

	ArrayCollection(const T* pArray_, size_t count) : pArray(pArray_), COUNT(count)
	{}


	virtual size_t Count() const override final
	{
		return COUNT;
	}

	virtual void Foreach(IVisitor<T>& visitor) const override final
	{
		for (uint32_t i = 0; i < COUNT; ++i)
		{
			visitor.OnValue(pArray[i]);
		}
	}

private:

	const T* pArray;
	const size_t COUNT;
};

template <class T, class U, class Transform>
class TransformedCollection : public ICollection < U >
{
public:

	TransformedCollection(const ICollection<T>& input, Transform transform) :
		input(&input),
		transform(transform)
	{}

	virtual size_t Count() const override final
	{
		return input->Count();
	}

	virtual void Foreach(IVisitor<U>& visitor) const override final
	{
		auto process = [this, &visitor](const T & elem)
		{
			visitor.OnValue(transform(elem));
		};
		input->ForeachItem(process);
	}

private:

	const ICollection<T>* input;
	Transform transform;

};

template <class T, class U, class Transform>
TransformedCollection<T, U, Transform> Map(const ICollection<T>& input, Transform transform)
{
	return TransformedCollection<T, U, Transform>(input, transform);
}



}

#endif
