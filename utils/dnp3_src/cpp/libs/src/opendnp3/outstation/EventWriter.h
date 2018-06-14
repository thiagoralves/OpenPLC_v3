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
#ifndef OPENDNP3_EVENTWRITER_H
#define OPENDNP3_EVENTWRITER_H

#include <openpal/util/Uncopyable.h>
#include <openpal/container/LinkedList.h>

#include "opendnp3/app/HeaderWriter.h"
#include "opendnp3/outstation/SOERecord.h"
#include "opendnp3/outstation/IEventRecorder.h"

namespace opendnp3
{

class EventWriter : openpal::StaticOnly
{
public:

	static bool Write(HeaderWriter& writer, IEventRecorder& recorder, openpal::LinkedListIterator<SOERecord> iterator);

private:

	class Result
	{
	public:

		Result(bool isFragmentFull_, openpal::LinkedListIterator<SOERecord> location_) : isFragmentFull(isFragmentFull_), location(location_)
		{}

		bool isFragmentFull;
		openpal::LinkedListIterator<SOERecord> location;


	private:

		Result() = delete;
	};

	static Result LoadHeader(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation);

	static Result LoadHeaderBinary(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation);
	static Result LoadHeaderDoubleBinary(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation);
	static Result LoadHeaderCounter(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation);
	static Result LoadHeaderFrozenCounter(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation);
	static Result LoadHeaderAnalog(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation);
	static Result LoadHeaderBinaryOutputStatus(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation);
	static Result LoadHeaderAnalogOutputStatus(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation);
	static Result LoadHeaderSecurityStat(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation);

	inline static bool IsWritable(const SOERecord& record)
	{
		return record.selected && !record.written;
	}

	template <class Spec>
	static Result WriteTypeWithSerializer(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation, opendnp3::DNP3Serializer<typename Spec::meas_t> serializer, typename Spec::event_variation_t variation)
	{
		auto iter = openpal::LinkedListIterator<SOERecord>::From(pLocation);

		auto header = writer.IterateOverCountWithPrefix<openpal::UInt16, typename Spec::meas_t>(QualifierCode::UINT16_CNT_UINT16_INDEX, serializer);

		openpal::ListNode<SOERecord>* pCurrent = nullptr;

		while (recorder.HasMoreUnwrittenEvents() && (pCurrent = iter.Next()))
		{
			auto& record = pCurrent->value;

			if (IsWritable(record))
			{
				if ((record.type == Spec::EventTypeEnum) && (record.GetValue<Spec>().selectedVariation == variation))
				{
					auto evt = record.ReadEvent<Spec>();
					if (header.Write(evt.value, evt.index))
					{
						record.written = true;
						recorder.RecordWritten(record.clazz, record.type);
					}
					else
					{
						auto location = openpal::LinkedListIterator<SOERecord>::From(pCurrent);
						return Result(true, location);
					}
				}
				else
				{

					// drop out and return from current location
					break;
				}
			}
		}

		auto location = openpal::LinkedListIterator<SOERecord>::From(pCurrent);
		return Result(false, location);
	}

	template <class Spec, class CTOType>
	static Result WriteCTOTypeWithSerializer(HeaderWriter& writer, IEventRecorder& recorder, openpal::ListNode<SOERecord>* pLocation, opendnp3::DNP3Serializer<typename Spec::meas_t> serializer, typename Spec::event_variation_t variation)
	{
		auto iter = openpal::LinkedListIterator<SOERecord>::From(pLocation);

		CTOType cto;
		cto.time = pLocation->value.GetTime();

		auto header = writer.IterateOverCountWithPrefixAndCTO<openpal::UInt16, typename Spec::meas_t, CTOType>(QualifierCode::UINT16_CNT_UINT16_INDEX, serializer, cto);

		openpal::ListNode<SOERecord>* pCurrent = nullptr;

		while (recorder.HasMoreUnwrittenEvents() && (pCurrent = iter.Next()))
		{
			auto& record = pCurrent->value;

			if (IsWritable(record))
			{
				if ((record.type == Spec::EventTypeEnum) && (record.GetValue<Spec>().selectedVariation == variation))
				{
					if (record.GetTime() < cto.time)
					{
						// drop out and return from current location
						break;
					}
					else
					{
						auto diff = record.GetTime() - cto.time;
						if (diff > openpal::UInt16::Max)
						{
							// drop out and return from current location
							break;
						}
						else
						{
							auto evt = record.ReadEvent<Spec>();
							evt.value.time = DNPTime(diff);
							if (header.Write(evt.value, evt.index))
							{
								record.written = true;
								recorder.RecordWritten(record.clazz, record.type);
							}
							else
							{
								auto location = openpal::LinkedListIterator<SOERecord>::From(pCurrent);
								return Result(true, location);
							}
						}
					}
				}
				else
				{
					// drop out and return from current location
					break;
				}
			}
		}

		auto location = openpal::LinkedListIterator<SOERecord>::From(pCurrent);
		return Result(false, location);
	}

};

}

#endif
