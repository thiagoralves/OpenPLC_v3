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
#ifndef OPENDNP3_TYPED_COMMAND_HEADER_H
#define OPENDNP3_TYPED_COMMAND_HEADER_H

#include "opendnp3/master/ICommandHeader.h"
#include "opendnp3/master/ICommandCollection.h"

#include "opendnp3/gen/CommandStatus.h"
#include "opendnp3/gen/CommandPointState.h"

#include "opendnp3/app/HeaderWriter.h"
#include "opendnp3/app/parsing/ICollection.h"
#include "opendnp3/app/Indexed.h"


#include <vector>
#include <algorithm>

namespace opendnp3
{

template <class T>
class TypedCommandHeader final : public ICommandHeader, public ICommandCollection<T>
{
	struct Record : public CommandState
	{
		Record(const Indexed<T>& pair) : CommandState(pair.index), command(pair.value)
		{}

		T command;
	};

public:

	TypedCommandHeader(const DNP3Serializer<T>& serializer) : m_serializer(serializer)
	{}

	// --- Implement ICommandCollection ---

	virtual ICommandCollection<T>& Add(const T& command, uint16_t index) override;

	// --- Implement ICommandHeader ----

	virtual bool AreAllSelected() const override;

	virtual bool Write(HeaderWriter&) const override;

	virtual void ApplySelectResponse(const ICollection<Indexed<T>>& commands) override;

	virtual void ApplyOperateResponse(const ICollection<Indexed<T>>& commands) override;

	// --- Implement ICollection<Indexed<CommandResponse>> ----

	virtual size_t Count() const override;

	virtual void Foreach(IVisitor<CommandState>& visitor) const override;

private:

	DNP3Serializer<T> m_serializer;
	std::vector<Record> m_records;
};


template <class T>
ICommandCollection<T>& TypedCommandHeader<T>::Add(const T& command, uint16_t index)
{
	m_records.push_back(WithIndex(command, index));
	return *this;
}

template <class T>
bool TypedCommandHeader<T>::AreAllSelected() const
{
	auto isSuccess = [](const Record & rec) -> bool { return rec.state == CommandPointState::SELECT_SUCCESS; };
	return std::all_of(m_records.begin(), m_records.end(), isSuccess);
}

template <class T>
bool TypedCommandHeader<T>::Write(HeaderWriter& writer) const
{
	if (m_records.empty())
	{
		return false;
	}

	auto iter = writer.IterateOverCountWithPrefix<openpal::UInt16, T>(QualifierCode::UINT16_CNT_UINT16_INDEX, m_serializer);

	for(auto& rec : m_records)
	{
		if (!iter.Write(rec.command, rec.index))
		{
			return false;
		}
	}

	return iter.IsValid();
}

template <class T>
void TypedCommandHeader<T>::ApplySelectResponse(const ICollection<Indexed<T>>& commands)
{
	if (commands.Count() > m_records.size())
	{
		return;
	}

	uint32_t index = 0;

	auto visit = [&](const Indexed<T> item) -> void
	{
		auto& rec = m_records[index];
		++index;

		if (item.index != rec.index)
		{
			return;
		}

		if (!item.value.ValuesEqual(rec.command))
		{
			rec.state = CommandPointState::SELECT_MISMATCH;
			return;
		}

		if (item.value.status != CommandStatus::SUCCESS)
		{
			rec.state = CommandPointState::SELECT_FAIL;
			rec.status = item.value.status;
			return;
		}

		if (rec.state == CommandPointState::INIT)
		{
			rec.state = CommandPointState::SELECT_SUCCESS;
		}
	};

	commands.ForeachItem(visit);
}

template <class T>
void TypedCommandHeader<T>::ApplyOperateResponse(const ICollection<Indexed<T>>& commands)
{
	if (commands.Count() > m_records.size())
	{
		return;
	}

	uint32_t index = 0;

	auto visit = [&](const Indexed<T> item)
	{
		auto& rec = m_records[index];
		++index;

		if (item.index != rec.index)
		{
			return;
		}

		if (!item.value.ValuesEqual(rec.command))
		{
			rec.state = CommandPointState::OPERATE_FAIL;
			return;
		}

		rec.state = CommandPointState::SUCCESS;
		rec.status = item.value.status;
	};

	commands.ForeachItem(visit);
}

template <class T>
size_t TypedCommandHeader<T>::Count() const
{
	return m_records.size();
}

template <class T>
void TypedCommandHeader<T>::Foreach(IVisitor<CommandState>& visitor) const
{
	for(auto& rec : m_records)
	{
		visitor.OnValue(rec);
	}
}

}

#endif
