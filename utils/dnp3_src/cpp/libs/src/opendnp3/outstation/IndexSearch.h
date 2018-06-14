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
#ifndef OPENDNP3_INDEXSEARCH_H
#define OPENDNP3_INDEXSEARCH_H

#include "opendnp3/outstation/Cell.h"

#include "opendnp3/app/Range.h"
#include "opendnp3/app/MeasurementTypes.h"

#include <openpal/container/ArrayView.h>
#include <openpal/util/Limits.h>
#include <openpal/util/Uncopyable.h>

namespace opendnp3
{

/**
* Implements a binary search for virtual indices in a discontiguous database
*/
class IndexSearch : private openpal::StaticOnly
{

public:

	class Result
	{
	public:

		Result(bool match_, uint16_t index_) : match(match_), index(index_)
		{}

		const bool match;
		const uint16_t index;

	private:

		Result() = delete;
	};

	template <class T>
	static Range FindRawRange(const openpal::ArrayView<Cell<T>, uint16_t>& view, const Range& range);

	template <class T>
	static Result FindClosestRawIndex(const openpal::ArrayView<Cell<T>, uint16_t>& view, uint16_t vIndex);

private:

	static uint16_t GetMidpoint(uint16_t lower, uint16_t upper)
	{
		return ((upper - lower) / 2) + lower;
	}
};

template <class T>
Range IndexSearch::FindRawRange(const openpal::ArrayView<Cell<T>, uint16_t>& view, const Range& range)
{
	if (range.IsValid() && view.IsNotEmpty())
	{
		uint16_t start = FindClosestRawIndex(view, range.start).index;
		uint16_t stop = FindClosestRawIndex(view, range.stop).index;

		if (view[start].config.vIndex < range.start)
		{
			if (start < openpal::MaxValue<uint16_t>())
			{
				++start;
			}
			else
			{
				return Range::Invalid();
			}
		}

		if (view[stop].config.vIndex > range.stop)
		{
			if (stop > 0)
			{
				--stop;
			}
			else
			{
				return Range::Invalid();
			}
		}

		return (view.Contains(start) && view.Contains(stop)) ? Range::From(start, stop) : Range::Invalid();
	}
	else
	{
		return Range::Invalid();
	}
}

template <class T>
IndexSearch::Result IndexSearch::FindClosestRawIndex(const openpal::ArrayView<Cell<T>, uint16_t>& view, uint16_t vIndex)
{
	if (view.IsEmpty())
	{
		return Result(false, 0);
	}
	else
	{
		uint16_t lower = 0;
		uint16_t upper = view.Size() - 1;

		uint16_t midpoint = 0;

		while (lower <= upper)
		{
			midpoint = GetMidpoint(lower, upper);

			auto index = view[midpoint].config.vIndex;

			if (index == vIndex)
			{
				return Result(true, midpoint);
			}
			else
			{
				if (index < vIndex) // search the upper array
				{
					if (lower < openpal::MaxValue<uint16_t>())
					{
						lower = midpoint + 1;
					}
					else
					{
						//we're at the upper limit
						return Result(false, midpoint);
					}
				}
				else
				{
					if (upper > 0)
					{
						upper = midpoint - 1;
					}
					else
					{
						//we're at the lower limit
						return Result(false, midpoint);
					}
				}
			}
		}

		return Result(false, midpoint);
	}
}

}

#endif
