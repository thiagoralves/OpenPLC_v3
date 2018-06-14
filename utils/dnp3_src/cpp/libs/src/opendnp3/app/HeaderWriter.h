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
#ifndef OPENDNP3_HEADERWRITER_H
#define OPENDNP3_HEADERWRITER_H

#include "opendnp3/app/GroupVariationRecord.h"
#include "opendnp3/gen/QualifierCode.h"

#include "opendnp3/app/IVariableLength.h"
#include "opendnp3/app/RangeWriteIterator.h"
#include "opendnp3/app/CountWriteIterator.h"
#include "opendnp3/app/PrefixedWriteIterator.h"
#include "opendnp3/app/BitfieldRangeWriteIterator.h"

#include "opendnp3/app/DNP3Serializer.h"
#include "opendnp3/app/GroupVariationID.h"

#include <openpal/container/Settable.h>
#include <openpal/serialization/Serialization.h>

namespace opendnp3
{

// A facade for writing APDUs to an external buffer
class HeaderWriter
{
	friend class APDUWrapper;

public:

	bool WriteHeader(GroupVariationID id, QualifierCode qc);

	template <class IndexType, class WriteType>
	RangeWriteIterator<IndexType, WriteType> IterateOverRange(QualifierCode qc, const DNP3Serializer<WriteType>& serializer, typename IndexType::Type start);

	template <class IndexType>
	bool WriteRangeHeader(QualifierCode qc, GroupVariationID gvId, typename IndexType::Type start, typename IndexType::Type stop);

	template <class IndexType>
	bool WriteCountHeader(QualifierCode qc, GroupVariationID gvId, typename IndexType::Type count);

	template <class CountType, class WriteType>
	CountWriteIterator<CountType, WriteType> IterateOverCount(QualifierCode qc, const DNP3Serializer<WriteType>& serializer);

	template <class IndexType>
	BitfieldRangeWriteIterator<IndexType> IterateOverSingleBitfield(GroupVariationID id, QualifierCode qc, typename IndexType::Type start);

	template <class CountType, class ValueType>
	bool WriteSingleValue(QualifierCode qc, const DNP3Serializer<ValueType>& serializer, const ValueType&);

	bool WriteFreeFormat(const IVariableLength&);

	template <class CountType, class WriteType>
	bool WriteSingleValue(QualifierCode qc, const WriteType&);

	template <class CountType, class ValueType>
	bool WriteSingleIndexedValue(QualifierCode qc, const DNP3Serializer<ValueType>& serializer, const ValueType&, typename CountType::Type index);

	template <class PrefixType, class WriteType>
	PrefixedWriteIterator<PrefixType, WriteType> IterateOverCountWithPrefix(QualifierCode qc, const DNP3Serializer<WriteType>& serializer);

	template <class PrefixType, class WriteType, class CTOType>
	PrefixedWriteIterator<PrefixType, WriteType> IterateOverCountWithPrefixAndCTO(QualifierCode qc, const DNP3Serializer<WriteType>& serializer, const CTOType& cto);


	// record the current position in case we need to rollback
	void Mark();

	// roll back to the last mark
	bool Rollback();

	uint32_t Remaining() const;

private:

	explicit HeaderWriter(openpal::WSlice* position_);

	bool WriteHeaderWithReserve(GroupVariationID id, QualifierCode qc, uint32_t reserve);

	openpal::WSlice* position;

	openpal::Settable<openpal::WSlice> mark;
};

template <class IndexType>
bool HeaderWriter::WriteRangeHeader(QualifierCode qc, GroupVariationID gvId, typename IndexType::Type start, typename IndexType::Type stop)
{
	if (WriteHeaderWithReserve(gvId, qc, 2 * IndexType::SIZE))
	{
		IndexType::WriteBuffer(*position, start);
		IndexType::WriteBuffer(*position, stop);
		return true;
	}
	else
	{
		return false;
	}
}

template <class IndexType>
bool HeaderWriter::WriteCountHeader(QualifierCode qc, GroupVariationID gvId, typename IndexType::Type count)
{
	if (WriteHeaderWithReserve(gvId, qc, IndexType::SIZE))
	{
		IndexType::WriteBuffer(*position, count);
		return true;
	}
	else
	{
		return false;
	}
}

template <class CountType, class ValueType>
bool HeaderWriter::WriteSingleValue(QualifierCode qc, const DNP3Serializer<ValueType>& serializer, const ValueType& value)
{
	auto reserveSize = CountType::SIZE + serializer.Size();
	if(this->WriteHeaderWithReserve(ValueType::ID, qc, reserveSize))
	{
		CountType::WSlice(*position, 1); //write the count
		serializer.Write(value, *position);
		return true;
	}
	else return false;
}

template <class CountType, class WriteType>
bool HeaderWriter::WriteSingleValue(QualifierCode qc, const WriteType& value)
{
	uint32_t reserveSize = CountType::SIZE + WriteType::Size();
	if(this->WriteHeaderWithReserve(WriteType::ID(), qc, reserveSize))
	{
		CountType::WriteBuffer(*position, 1); //write the count
		WriteType::Write(value, *position);
		return true;
	}
	else
	{
		return false;
	}
}

template <class CountType, class ValueType>
bool HeaderWriter::WriteSingleIndexedValue(QualifierCode qc, const DNP3Serializer<ValueType>& serializer, const ValueType& value, typename CountType::Type index)
{
	uint32_t reserveSize = 2 * CountType::SIZE + serializer.Size();
	if(this->WriteHeaderWithReserve(serializer.ID(), qc, reserveSize))
	{
		CountType::WriteBuffer(*position, 1); //write the count
		CountType::WriteBuffer(*position, index); // write the index
		serializer.Write(value, *position);
		return true;
	}
	else return false;
}

template <class IndexType, class WriteType>
RangeWriteIterator<IndexType, WriteType> HeaderWriter::IterateOverRange(QualifierCode qc, const DNP3Serializer<WriteType>& serializer, typename IndexType::Type start)
{
	uint32_t reserveSize = 2 * IndexType::SIZE + serializer.Size();
	if(this->WriteHeaderWithReserve(serializer.ID(), qc, reserveSize))
	{
		return RangeWriteIterator<IndexType, WriteType>(start, serializer, *position);
	}
	else return RangeWriteIterator<IndexType, WriteType>::Null();
}

template <class CountType, class WriteType>
CountWriteIterator<CountType, WriteType> HeaderWriter::IterateOverCount(QualifierCode qc, const DNP3Serializer<WriteType>& serializer)
{
	uint32_t reserveSize = CountType::SIZE + serializer.Size();
	if(this->WriteHeaderWithReserve(serializer.ID(), qc, reserveSize))
	{
		return CountWriteIterator<CountType, WriteType>(serializer, *position);
	}
	else return CountWriteIterator<CountType, WriteType>::Null();
}

template <class IndexType>
BitfieldRangeWriteIterator<IndexType> HeaderWriter::IterateOverSingleBitfield(GroupVariationID id, QualifierCode qc, typename IndexType::Type start)
{
	uint32_t reserveSize = 2 * IndexType::SIZE + 1; //need at least 1 byte
	if (this->WriteHeaderWithReserve(id, qc, reserveSize))
	{
		return BitfieldRangeWriteIterator<IndexType>(start, *position);
	}
	else return BitfieldRangeWriteIterator<IndexType>::Null();
}

template <class PrefixType, class WriteType>
PrefixedWriteIterator<PrefixType, WriteType> HeaderWriter::IterateOverCountWithPrefix(QualifierCode qc, const DNP3Serializer<WriteType>& serializer)
{
	uint32_t reserveSize = 2 * PrefixType::SIZE + serializer.Size(); //enough space for the count, 1 prefix + object
	if(this->WriteHeaderWithReserve(serializer.ID(), qc, reserveSize))
	{
		return PrefixedWriteIterator<PrefixType, WriteType>(serializer, *position);
	}
	else return PrefixedWriteIterator<PrefixType, WriteType>::Null();
}

template <class PrefixType, class WriteType, class CTOType>
PrefixedWriteIterator<PrefixType, WriteType> HeaderWriter::IterateOverCountWithPrefixAndCTO(QualifierCode qc, const DNP3Serializer<WriteType>& serializer, const CTOType& cto)
{
	this->Mark();
	if (this->WriteSingleValue<openpal::UInt8, CTOType>(QualifierCode::UINT8_CNT, cto))
	{
		auto iter = IterateOverCountWithPrefix<PrefixType, WriteType>(qc, serializer);
		if (!iter.IsValid())
		{
			// remove the CTO header, if there's no space to write a value
			this->Rollback();
		}
		return iter;
	}
	else
	{
		return PrefixedWriteIterator<PrefixType, WriteType>::Null();
	}
}

}

#endif
