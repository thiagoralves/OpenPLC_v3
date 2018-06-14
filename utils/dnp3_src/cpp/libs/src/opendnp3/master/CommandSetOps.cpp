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

#include "CommandSetOps.h"

#include "opendnp3/master/ICommandHeader.h"
#include "opendnp3/master/CommandTaskResult.h"

#include "opendnp3/app/parsing/APDUParser.h"

#include <algorithm>

namespace opendnp3
{

template <class T>
IINField CommandSetOps::ProcessAny(const PrefixHeader& header, const ICollection<Indexed<T>>& values)
{
	if (header.GetQualifierCode() != QualifierCode::UINT16_CNT_UINT16_INDEX)
	{
		return IINBit::PARAM_ERROR;
	}

	if (header.headerIndex >= commands->m_headers.size()) // more response headers than request headers
	{
		return IINBit::PARAM_ERROR;
	}

	if (mode == Mode::Select)
	{
		commands->m_headers[header.headerIndex]->ApplySelectResponse(values);
	}
	else
	{
		commands->m_headers[header.headerIndex]->ApplyOperateResponse(values);
	}

	return IINField::Empty();
}

CommandSetOps::CommandSetOps(Mode mode_, CommandSet& commands_) :
	mode(mode_),
	commands(&commands_)
{}

bool CommandSetOps::Write(const CommandSet& set, HeaderWriter& writer)
{
	for(auto& header : set.m_headers)
	{
		if (!header->Write(writer))
		{
			return false;
		}
	}

	return true;
}

void CommandSetOps::InvokeCallback(const CommandSet& set, TaskCompletion result, CommandCallbackT& callback)
{
	CommandTaskResult impl(result, set.m_headers);
	callback(impl);
}

CommandSetOps::SelectResult CommandSetOps::ProcessSelectResponse(CommandSet& set, const openpal::RSlice& headers, openpal::Logger* logger)
{
	CommandSetOps handler(Mode::Select, set);
	if (APDUParser::Parse(headers, handler, logger) != ParseResult::OK)
	{
		return SelectResult::FAIL_PARSE;
	}

	auto selected = [](const ICommandHeader * header) -> bool
	{
		return header->AreAllSelected();
	};
	return std::all_of(set.m_headers.begin(), set.m_headers.end(), selected) ? SelectResult::OK : SelectResult::FAIL_SELECT;
}

CommandSetOps::OperateResult CommandSetOps::ProcessOperateResponse(CommandSet& set, const openpal::RSlice& headers, openpal::Logger* logger)
{
	CommandSetOps handler(Mode::Operate, set);
	return (APDUParser::Parse(headers, handler, logger) == ParseResult::OK) ? OperateResult::OK : OperateResult::FAIL_PARSE;
}

bool CommandSetOps::IsAllowed(uint32_t headerCount, GroupVariation gv, QualifierCode qc)
{
	if (qc != QualifierCode::UINT16_CNT_UINT16_INDEX)
	{
		return false;
	}

	switch (gv)
	{
	case(GroupVariation::Group12Var1) : //	CROB
	case(GroupVariation::Group41Var1) : //	4 kinds of AO
	case(GroupVariation::Group41Var2) :
	case(GroupVariation::Group41Var3) :
	case(GroupVariation::Group41Var4) :
		return true;
	default:
		return false;
	}
}

IINField CommandSetOps::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<ControlRelayOutputBlock>>& values)
{
	return this->ProcessAny(header, values);
}

IINField CommandSetOps::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputInt16>>& values)
{
	return this->ProcessAny(header, values);
}

IINField CommandSetOps::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputInt32>>& values)
{
	return this->ProcessAny(header, values);
}

IINField CommandSetOps::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputFloat32>>& values)
{
	return this->ProcessAny(header, values);
}

IINField CommandSetOps::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<AnalogOutputDouble64>>& values)
{
	return this->ProcessAny(header, values);
}


}
