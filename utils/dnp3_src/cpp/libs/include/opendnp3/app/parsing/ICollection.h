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
#ifndef OPENDNP3_ICOLLECTION_H
#define OPENDNP3_ICOLLECTION_H

#include <cstddef>

namespace opendnp3
{

/**
* Abstract way of visiting elements of a collection
*
*/
template <class T>
class IVisitor
{
public:

	virtual void OnValue(const T& value) = 0;
};

/**
* A visitor implemented as an abstract functor
*
*/
template <class T, class Fun>
class FunctorVisitor : public IVisitor<T>
{

public:

	FunctorVisitor(const Fun& fun_) : fun(fun_) {}

	virtual void OnValue(const T& value) override final
	{
		fun(value);
	}

private:
	Fun fun;
};

/**
* An interface representing an abstract immutable collection of things of type T.
*
* The user can only read these values via callback to receive each element.
*/
template <class T>
class ICollection
{
public:

	/**
	* The number of elements in the collection
	*/
	virtual size_t Count() const = 0;

	/**
	* Visit all the elements of a collection
	*/
	virtual void Foreach(IVisitor<T>& visitor) const = 0;

	/**
		visit all of the elements of a collection
	*/
	template <class Fun>
	void ForeachItem(const Fun& fun) const
	{
		FunctorVisitor<T, Fun> visitor(fun);
		this->Foreach(visitor);
	}

	/**
		Retrieve the only value from the collection.
	*/
	bool ReadOnlyValue(T& value) const
	{
		if (this->Count() == 1)
		{
			auto assignValue = [&value](const T & item)
			{
				value = item;
			};
			this->ForeachItem(assignValue);
			return true;
		}
		else
		{
			return false;
		}
	}
};

}

#endif
