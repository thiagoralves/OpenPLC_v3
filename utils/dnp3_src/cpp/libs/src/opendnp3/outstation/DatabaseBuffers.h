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
#ifndef OPENDNP3_DATABASEBUFFERS_H
#define OPENDNP3_DATABASEBUFFERS_H

#include "opendnp3/app/Range.h"

#include "opendnp3/gen/IndexMode.h"

#include "opendnp3/outstation/IndexSearch.h"
#include "opendnp3/outstation/DatabaseSizes.h"
#include "opendnp3/outstation/StaticBuffers.h"
#include "opendnp3/outstation/SelectedRanges.h"
#include "opendnp3/outstation/StaticTypeBitfield.h"

#include "opendnp3/outstation/IResponseLoader.h"
#include "opendnp3/outstation/IStaticSelector.h"
#include "opendnp3/outstation/IClassAssigner.h"
#include "opendnp3/outstation/StaticLoadFunctions.h"

namespace opendnp3
{

/**
The database coordinates all updates of measurement data
*/
class DatabaseBuffers : public IStaticSelector, public IResponseLoader, public IClassAssigner, private openpal::Uncopyable
{
public:

	DatabaseBuffers(const DatabaseSizes&, StaticTypeBitField allowedClass0Types, IndexMode indexMode);

	// ------- IStaticSelector -------------

	virtual IINField SelectAll(GroupVariation gv) override final;
	virtual IINField SelectRange(GroupVariation gv, const Range& range) override final;

	// ------- IResponseLoader -------------

	virtual bool Load(HeaderWriter& writer) override final;
	virtual bool HasAnySelection() const override final
	{
		return ranges.HasAnySelection();
	}

	// ------- IClassAssigner -------------

	virtual Range AssignClassToAll(AssignClassType type, PointClass clazz) override final;
	virtual Range AssignClassToRange(AssignClassType type, PointClass clazz, const Range& range) override final;

	//used to unselect selected points
	void Unselect();

	// stores the most revent values and event information
	StaticBuffers buffers;

private:

	StaticTypeBitField class0;
	IndexMode indexMode;

	SelectedRanges ranges;

	template <class Spec>
	bool LoadType(HeaderWriter& writer);

	template <class Spec>
	void Deselect()
	{
		auto range = ranges.Get<Spec>();
		if (range.IsValid())
		{
			auto view = buffers.GetArrayView<Spec>();
			for (uint16_t i = range.start; i <= range.stop; ++i)
			{
				view[i].selection.selected = false;
			}
			ranges.Clear<Spec>();
		}
	}

	//specialization for binary in cpp file
	template <class Spec>
	static typename Spec::static_variation_t  CheckForPromotion(const typename Spec::meas_t& value, typename Spec::static_variation_t variation)
	{
		return variation;
	}

	static Range RangeOf(uint16_t size);

	template <class T>
	IINField GenericSelect(
	    Range range,
	    openpal::ArrayView<Cell<T>, uint16_t> view,
	    bool useDefault,
	    typename T::static_variation_t variation
	);

	template <class Spec>
	Range AssignClassTo(PointClass clazz, const Range& range);

	template <class T>
	void SelectAllClass0()
	{
		if (class0.IsSet(T::StaticTypeEnum))
		{
			this->SelectAll<T>();
		}
	}

	template <class T>
	IINField SelectAll()
	{
		auto view = buffers.GetArrayView<T>();
		return GenericSelect(RangeOf(view.Size()), view, true, typename T::static_variation_t());
	}

	template <class T>
	IINField SelectAllUsing(typename T::static_variation_t variation)
	{
		auto view = buffers.GetArrayView<T>();
		return GenericSelect(RangeOf(view.Size()), view, false, variation);
	}

	template <class T>
	IINField SelectVirtualRange(const Range& range, bool usedefault, typename T::static_variation_t variation)
	{
		if (indexMode == IndexMode::Discontiguous)
		{
			auto view = buffers.GetArrayView<T>();
			auto mapped = IndexSearch::FindRawRange(view, range);
			if (mapped.IsValid())
			{
				// detect if any values were requested that aren't actually there
				IINField clipped = (range.Count() == mapped.Count()) ? IINField() : IINField(IINBit::PARAM_ERROR);
				return clipped | GenericSelect(mapped, buffers.GetArrayView<T>(), usedefault, variation);
			}
			else
			{
				return IINField(IINBit::PARAM_ERROR);
			}
		}
		else
		{
			return GenericSelect(range, buffers.GetArrayView<T>(), usedefault, variation);
		}
	}

	template <class T>
	IINField SelectRange(const Range& range)
	{
		return SelectVirtualRange<T>(range, true, typename T::static_variation_t());
	}

	template <class T>
	IINField SelectRangeUsing(const Range& range, typename T::static_variation_t variation)
	{
		return SelectVirtualRange<T>(range, false, variation);
	}
};

template <class T>
IINField DatabaseBuffers::GenericSelect(
    Range range,
    openpal::ArrayView<Cell<T>, uint16_t> view,
    bool useDefault,
    typename T::static_variation_t variation)
{
	if (range.IsValid())
	{
		auto allowed = range.Intersection(RangeOf(view.Size()));

		if (allowed.IsValid())
		{
			// return code depends on if the range was truncated to match the database
			IINField ret = allowed.Equals(range) ? IINField() : IINBit::PARAM_ERROR;

			for (uint16_t i = allowed.start; i <= allowed.stop; ++i)
			{
				if (view[i].selection.selected)
				{
					ret |= IINBit::PARAM_ERROR;
				}
				else
				{
					view[i].selection.selected = true;
					view[i].selection.value = view[i].value;
					auto var = useDefault ? view[i].config.svariation : variation;
					view[i].selection.variation = CheckForPromotion<T>(view[i].selection.value, var);
				}
			}

			ranges.Merge<T>(allowed);

			return ret;
		}
		else
		{
			return IINField(IINBit::PARAM_ERROR);
		}
	}
	else
	{
		return IINField();
	}
}

template <class T>
bool DatabaseBuffers::LoadType(HeaderWriter& writer)
{
	auto range = ranges.Get<T>();
	if (range.IsValid())
	{
		auto view = buffers.GetArrayView<T>();

		bool spaceRemaining = true;

		// ... load values, manipulate the range
		while (spaceRemaining && range.IsValid())
		{
			if (view[range.start].selection.selected)
			{
				/// lookup the specific write function based on the reporting variation
				auto writeFun = GetStaticWriter(view[range.start].selection.variation);

				// start writing a header, the invoked function will advance the range appropriately
				spaceRemaining = writeFun(view, writer, range);
			}
			else
			{
				// just skip over values that are not selected
				range.Advance();
			}
		}

		ranges.Set<T>(range);

		return spaceRemaining;
	}
	else
	{
		// no data to load
		return true;
	}
}

template <class Spec>
Range DatabaseBuffers::AssignClassTo(PointClass clazz, const Range& range)
{
	auto view = buffers.GetArrayView<Spec>();
	auto clipped = range.Intersection(RangeOf(view.Size()));
	for (auto i = clipped.start; i <= clipped.stop; ++i)
	{
		view[i].config.clazz = clazz;
	}
	return clipped;
}

}

#endif

